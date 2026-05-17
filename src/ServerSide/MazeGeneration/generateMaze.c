#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../DataTypes/requestDataTypes.h"
#include "../Maze/maze.h"
#include "generateMaze.h"


typedef struct {
    char type;
    char direction;
    bool isSolution;
    union {
        bool isLoop;
        char closedSides;
    };
} GenerationWall;

const GenerationWall DEFAULT_WALL = {
    .type = WALL,
    .direction = 0,
    .closedSides = 0,
};

typedef struct {
    MazeSize size;
    MazeWallCount wallCount;
    GenerationWall *horizontalWalls;
    GenerationWall *verticalWalls;
    Point root;
    Point openings[2];
} GenerationMaze;


void AddLoops(GenerationMaze maze, double loopInput);
void MarkSolution(GenerationMaze maze, Point start, Point goal);

void PrintGenerationMaze(GenerationMaze maze) {
    int h = 0, v = 0;
    printf("+");
    for (int i = 0; i < maze.size.x * 2 - 1; i++) printf("-");
    printf("+\n");
    for (int i = 0; i < maze.size.y * 2 - 1; i++) {
        if (i % 2 == 0) {
            printf("|");
            for(int j = 0; j < maze.size.x - 1; j++, h++) {
                GenerationWall wall = maze.horizontalWalls[h];
                printf(" %c", wall.type ? '|' : wall.isSolution ? wall.direction ? '}' : '{': wall.direction ? '>' : '<');
            }
            printf(" |");
        }
        else {
            printf("+");
            for(int j = 0; j < maze.size.x; j++, v++) {
                GenerationWall wall = maze.verticalWalls[(v % maze.size.x) * (maze.size.y - 1) + v/maze.size.x];
                printf("%c+", wall.type ? '-' : wall.isSolution ? wall.direction ? 'Y' : 'A': wall.direction ? 'v' : '^');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < maze.size.x * 2 - 1; i++) printf("-");
    printf("+\n\n");
}

void FreeMemory(GenerationMaze* maze, GenerationWall** frontiers) {
    if (maze && maze->horizontalWalls) free(maze->horizontalWalls);
    if (maze) free(maze);
    if (frontiers) free(frontiers);
}

GenerationMaze *FillWalls(MazeSize size) {
    //Amount of horizontal walls are (width-1) times height. Opposite for vertical
    MazeWallCount wallCount = {(size.x - 1) * size.y, (size.y - 1) * size.x};
    GenerationMaze *maze = malloc(sizeof(GenerationMaze));
    if (!maze) return FreeMemory(maze, NULL), NULL;
    //Allocating a memory block that can hold both arrays. This increases cache friendliness
    GenerationWall *block = malloc(sizeof(GenerationWall) * wallCount.horizontal + sizeof(GenerationWall) * wallCount.vertical);
    if (!block) return FreeMemory(maze, NULL), NULL;
    maze->size = size;
    maze->wallCount = wallCount;
    maze->horizontalWalls = block; //First array begins at the start of the block
    maze->verticalWalls = block + maze->wallCount.horizontal; //Second array begins right after the first one

    for (int i = 0; i < maze->wallCount.horizontal; i++) {
        maze->horizontalWalls[i] = DEFAULT_WALL;
    }
    for (int j = 0; j < maze->wallCount.vertical ; j++) {
        maze->verticalWalls[j] = DEFAULT_WALL;
    }
    return maze;
}

GenerationWall **GetNeighbourWalls(GenerationMaze maze, MazeSize size, Point pos) {
    int indices[4];
    LoadNeighbourWallIndices(size, pos, indices);

    GenerationWall **neighbourWalls = malloc(sizeof(GenerationWall*)*4);
    //Uses the indexes to get pointers to the neighbouring walls
    for (int i = 0; i < 4; i++) {
        if (indices[i] == -1)
            neighbourWalls[i] = NULL;
        else if (i<2)
            neighbourWalls[i] = &maze.horizontalWalls[indices[i]];
        else
            neighbourWalls[i] = &maze.verticalWalls[indices[i]];
    }
    return neighbourWalls;
}

void AddNeighboursToFrontier(GenerationWall **frontier, int *frontierSize, GenerationMaze maze, MazeSize size, Point pos) {
    //Adds the up to four neighbour walls of a point to the frontier array if they have not been added already
    GenerationWall **neighbourWalls = GetNeighbourWalls(maze, size, pos);
    for (int i = 0; i < 4; i++) {
        //Skips the wall if it does not exist, or if it has at least one side in the maze.
        //Also increases the closed sides of the wall by one, since "pos" point is now becoming closed.
        if (neighbourWalls[i] == NULL || neighbourWalls[i]->closedSides++ > 0) continue;

        //The order of neighbourWalls is right, left, bottom, top. Every other wall is a positive direction.
        neighbourWalls[i]->direction = (i+1)%2;
        frontier[(*frontierSize)++] = neighbourWalls[i];
    }
    free(neighbourWalls);
}

typedef struct {
    bool isHorizontal;
    int index;
} ArrIndexResult;

ArrIndexResult GetArrayIndex(GenerationMaze maze, GenerationWall *frontierWall) {
    //Returns the index of the wall in its respective array, and whether it is in the horizontal array or not

    //If the pointer is between the first and last pointer in the vertical array
    if (frontierWall >= maze.verticalWalls && frontierWall < maze.verticalWalls + maze.wallCount.vertical)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){false, (int)(frontierWall - maze.verticalWalls)};

    //If the pointer is between the first and last pointer in the horizontal array
    else if (frontierWall >= maze.horizontalWalls && frontierWall < maze.horizontalWalls + maze.wallCount.horizontal)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){true, (int)(frontierWall - maze.horizontalWalls)};
    else {
        printf("No Index Found!\n");
        return (ArrIndexResult){false, -1}; //FallBack
    }
}

ExportWall *GenerationWallArrToExportWallArr(const GenerationWall* arr, int size) {
    ExportWall *exportWallArr = malloc(sizeof(ExportWall) * size);
    if (!exportWallArr) return NULL;
    for (int i = 0; i < size; i++)
        exportWallArr[i] = (ExportWall) {arr[i].type, arr[i].isSolution};
    return exportWallArr;
}

Wall *GenerationWallArrToWallArr(const GenerationWall* generationWalls, Wall* walls, unsigned long size) {
    for (int i = 0; i < size; i++)
        walls[i] = (Wall) {generationWalls[i].type, generationWalls[i].direction, generationWalls[i].isSolution};
    return walls;
}

Maze GenerationMazeToMaze(GenerationMaze genMaze, bool isPerfect) {
    Wall* wallBlock = malloc(sizeof(Wall) * (genMaze.wallCount.horizontal + genMaze.wallCount.vertical));
    GenerationWallArrToWallArr(genMaze.horizontalWalls, &wallBlock[0], genMaze.wallCount.horizontal);
    GenerationWallArrToWallArr(genMaze.verticalWalls, &wallBlock[genMaze.wallCount.horizontal], genMaze.wallCount.vertical);

    Maze maze = (Maze) {
        isPerfect ? PERFECT : NOT_PERFECT,
        &wallBlock[0],
        &wallBlock[genMaze.wallCount.horizontal],
        genMaze.size,
        genMaze.wallCount,
        {genMaze.openings[0], genMaze.openings[1]}
    };
    return maze;
}


float *FrontierWeights(GenerationWall **frontier, int frontierSize, GenerationMaze maze, MazeSize size,
                       float straightnessPriority, float branchPriority) {
    float *frontierWeights = malloc(sizeof(float) * frontierSize);
    //We create an array with weights for each frontier, its just as big as the frontier array and each matches
    if (frontierWeights == NULL) return NULL;

    float branchPotential;
    float straightnessPotential;

    int max_len = 0;

    for (int y = 0; y < size.y; y++) {
        //calculating max_len, problem with this is that it checks the WALLS, not the cells, so three cells open are seperated by 2 walls. We fix this later by just adding 1
        int max_len_count = 0;

        for (int x = 0; x < size.x; x++) {
            Point position = {x, y};
            int index = GetRightWallIndex(position, size);

            if (index != -1 && maze.horizontalWalls[index].type == AIR) {
                //After getting all neighbors we are checking if if the right side, cuz we are moving right, is open
                max_len_count++; //if yes, max_len_count is increased
            }
            else {
                if (max_len_count > max_len) {
                    //if not, max len if it is smaller than the max_len_count gets max_len_count's value, and max_len_count is reset
                    max_len = max_len_count;
                }
                max_len_count = 0;
            }
        }
    }

    for (int x = 0; x < size.x; x++) {
        //same as before but now we are checking vertically instead
        int max_len_count = 0;

        for (int y = 0; y < size.y; y++) {
            Point position = {x, y};
            int index = GetLowerWallIndex(position, size);

            if (index != -1 && maze.verticalWalls[index].type == AIR) {
                max_len_count++;
            } else {
                if (max_len_count > max_len) {
                    max_len = max_len_count;
                }
                max_len_count = 0;
            }
        }
    }


    for (int i = 0; i < frontierSize; i++)
    {
        //getting branch potential
        ArrIndexResult wallRef = GetArrayIndex(maze, frontier[i]);
        //we use the get ArrindexResult to get the index of our frontier

        Point position = LeftUpperPoint(wallRef.isHorizontal, wallRef.index, size);
        //Then we get our position by using the LeftUpperPoint function

        GenerationWall **neighbors = GetNeighbourWalls(maze, size, position);

        if (wallRef.isHorizontal && frontier[i]->direction == LEFT)
        //now the indexToPos automatically shifts us to the left side, however, if our direction is left, we need to be on the right side
        {
            position.x++; //So we shift to the right cell
        }
        if (!wallRef.isHorizontal && frontier[i]->direction == UP) //same idea here, just vertically
        {
            position.y++;
        }

        int openNeighbours = 0;
        for (int j = 0; j < 4; j++) //We are counting all 4 neighbor walls to check how many are open and closed
        {
            if (neighbors[j] != NULL && neighbors[j]->type == AIR) {
                openNeighbours++;
            }
        }

        if (openNeighbours <= 1) //if its 1 or under its a dead end
        {
            branchPotential = 0;
        } else {
            branchPotential = 1;
        }
        free(neighbors);


        //Calculate straightness potential
        int straightnesscount = 0;
        bool keepGoing = true;

        int index = wallRef.index;
        while (keepGoing) {
            if (wallRef.isHorizontal && frontier[i]->direction == LEFT)
            // four copies of the same function, I know its inefficient but dont touch.
            {
                if ((index + 1) / (size.x-1) == index / (size.x-1) && maze.horizontalWalls[++index].type == AIR)
                //we are checking if the next step is open, if yes, we keep going and counting
                {
                    straightnesscount++;
                } else {
                    keepGoing = false; //once we hit a wall we stop
                }
            }

            else if (wallRef.isHorizontal && frontier[i]->direction == RIGHT) {
                if ((index - 1) / (size.x-1) == index / (size.x-1) && maze.horizontalWalls[--index].type == AIR) {
                    straightnesscount++;
                } else {
                    keepGoing = false;
                }
            }

            else if (!wallRef.isHorizontal && frontier[i]->direction == UP) {
                if ((index + 1) / (size.y-1) == index / (size.y-1) && maze.verticalWalls[++index].type == AIR) {
                    straightnesscount++;
                } else {
                    keepGoing = false;
                }
            }

            else if (!wallRef.isHorizontal && frontier[i]->direction == DOWN) {
                if ((index - 1) / (size.y-1) == index / (size.y-1) && maze.verticalWalls[--index].type == AIR) {
                    straightnesscount++;
                } else {
                    keepGoing = false;
                }
            }
        }
        straightnesscount += 1;
        // same issue with max_len, we are counting the walls, not the cells, so we have to +1
        if (max_len == 0) {
            straightnessPotential = 0;
        } else {
            int true_max = max_len + 1;
            straightnessPotential = (float)straightnesscount / (float)true_max;
        }

        // our weight function = e^((straight_priority - 0.5) * (straight_potential - 0.5)) * e^((branch_priority - 0.5) * (branch_potential - 0.5))
        frontierWeights[i] = exp((4 * straightnessPriority - 2) * (4 * straightnessPotential - 2)) * exp(
                                (4 * branchPriority - 2) * (4 * branchPotential - 2));
    }

    return frontierWeights;
}

GenerationWall *PopRandomFrontier(GenerationWall **frontier, int *frontierSize, GenerationMaze maze, MazeSize size,
                                  float straightnessPriority, float branchPriority) {
    float *weights = FrontierWeights(frontier, *frontierSize, maze, size, straightnessPriority, branchPriority);

    float total = 0.0f;

    for (int i = 0; i < *frontierSize; i++) {
        //now pay attention, we are adding each weight to the total
        total += weights[i];
        weights[i] = total;
    }


    float random = ((float) rand() / RAND_MAX) * total;
    //now we get a random number between 0 and the result of our total

    int chosenIndex = 0;
    for (int i = 0; i < *frontierSize; i++) {
        //whatever number we got, we are now checking here and the index of the number which matches is our chosenindex
        if (random <= weights[i]) {
            chosenIndex = i;
            break;
        }
    }
    //Assume we got:
    //total: 1.93,   random number: 0.73
    //Indexes: 0: 0.35, 1: 0.74, 2: 1.12, 3: 1.37 etc
    // 0.73 is below index 1 0.74. So our chosen index is 1.

    GenerationWall *poppedFrontier = frontier[chosenIndex]; //now the frontier with index 2 is the one to get removed
    //Moves the last element in the frontier to the deleted element's place
    frontier[chosenIndex] = frontier[--(*frontierSize)];

    free(weights);

    return poppedFrontier;
}


ExportData GenerateMaze(GenerationData data) {
    MazeSize size = {data.x_size, data.y_size};
    // MazeSize size = {24,26};
    GenerationMaze *maze = FillWalls(size);
    if (!maze) return (ExportData){-1}; //crash

    int frontierSize = 0;
    GenerationWall** frontiers = malloc(
        sizeof(GenerationWall*) * maze->wallCount.horizontal +
        sizeof(GenerationWall*) * maze->wallCount.vertical);
    if (!frontiers) {
        FreeMemory(maze, frontiers);
        return (ExportData){-1}; //crash
    }

    Point startPoint = (Point) {rand() % size.x, rand() % size.y};
    maze->root = startPoint;
    AddNeighboursToFrontier(frontiers, &frontierSize, *maze, size, maze->root);
    while (frontierSize > 0) {
        GenerationWall *rndFrontier = PopRandomFrontier(frontiers, &frontierSize, *maze, size, data.straightness, data.branches); // change so the numbers are branch potential and straightness potential
        //If both sides of the wall is in the closed maze, it is not actually a frontier, and should not be removed
        if (rndFrontier->closedSides >= 2) continue;

        //Gets the index of the wall in its corresponding array
        ArrIndexResult arrIndex = GetArrayIndex(*maze, rndFrontier);
        if (arrIndex.index == -1) {
            FreeMemory(maze, frontiers);
            return (ExportData){-1}; //crash
        }

        //Uses the index to get the position just above/to the left of the wall.
        Point frontierPos = LeftUpperPoint(arrIndex.isHorizontal, arrIndex.index, size);
        //If the wall has a positive direction, shift the point 1 down/to the right
        if (arrIndex.isHorizontal && rndFrontier->direction == RIGHT) frontierPos.x++;
        if (!arrIndex.isHorizontal && rndFrontier->direction == DOWN) frontierPos.y++;
        AddNeighboursToFrontier(frontiers, &frontierSize, *maze, size, frontierPos);

        rndFrontier->type = AIR;
    }

    maze->openings[0] = (Point) {0, 0};
    maze->openings[1] = (Point) {maze->size.x - 1, maze->size.y - 1};

    MarkSolution(*maze, maze->openings[0], maze->openings[1]);
    AddLoops(*maze, data.loops);


    Maze properMaze = GenerationMazeToMaze(*maze, data.loops == 0);
    if (properMaze.horizontalWalls) SaveMaze(properMaze, data.id);
    free(properMaze.horizontalWalls);

    ExportWall* horizontalExportWalls = GenerationWallArrToExportWallArr(maze->horizontalWalls, maze->wallCount.horizontal);
    ExportWall* verticalExportWalls = GenerationWallArrToExportWallArr(maze->verticalWalls, maze->wallCount.vertical);

    ExportData exportMaze = {data.id, maze->wallCount.horizontal, maze->wallCount.vertical, horizontalExportWalls, verticalExportWalls};

    FreeMemory(maze, frontiers);
    return exportMaze;
}

void FlipSolutionMarkingToRoot(GenerationMaze maze, Point point) {
    int chosenIndex;
    do {
        GenerationWall **neighbours = GetNeighbourWalls(maze, maze.size, point);
        Direction neighbourDirections[4] = {
            neighbours[0] && neighbours[0]->type != WALL ? neighbours[0]->direction : -1,
            neighbours[1] && neighbours[1]->type != WALL ? neighbours[1]->direction : -1,
            neighbours[2] && neighbours[2]->type != WALL ? neighbours[2]->direction : -1,
            neighbours[3] && neighbours[3]->type != WALL ? neighbours[3]->direction : -1,
        };
        chosenIndex = StepTowardsRoot(neighbourDirections, &point);
        if (chosenIndex != -1) {
            neighbours[chosenIndex]->isSolution = !neighbours[chosenIndex]->isSolution;
            free(neighbours);
        }
    } while (chosenIndex != -1);
}

void MarkSolution(GenerationMaze maze, Point start, Point goal) {
    FlipSolutionMarkingToRoot(maze, start);
    FlipSolutionMarkingToRoot(maze, goal);
}

void AddLoops(GenerationMaze maze, double loopInput) {
    int loopAmount = (int)((maze.wallCount.horizontal + maze.wallCount.vertical) * loopInput);
    int maxSmallLoops = (int)(loopAmount * 0.6); // total amount of "small loops allowed"
    int maxMidLoops = (int)(loopAmount * 0.25);  // same etc
    int maxBigLoops = (int)(loopAmount * 0.10);
    int maxGiantLoops = loopAmount - maxSmallLoops - maxMidLoops - maxBigLoops;
    int loopAmountSmall = 0;
    int loopAmountMid = 0;
    int loopAmountBig = 0;
    int loopAmountGiant = 0;

    int smallMaxSize = 20; // definitions for each
    int midMaxSize = 60;
    int bigMaxSize = 150;

    for (int i = 0; i < maze.wallCount.horizontal; i++) {
        maze.horizontalWalls[i].isLoop = false;
    }

    for (int i = 0; i < maze.wallCount.vertical; i++) {
        maze.verticalWalls[i].isLoop = false;
    }

    for (int loopAttempts = 0; loopAttempts < loopAmount; loopAttempts++) {
        GenerationWall *chosenWall;
        int isHorizontal;
        size_t wallIndex = rand() % (maze.wallCount.horizontal + maze.wallCount.vertical); // we create a random number that is at most as big as there are walls
        //Exploiting the fact that horizontalWalls and verticalWalls are one contiguous block in memory, and treating them as one large array
        if (maze.horizontalWalls[wallIndex].type == WALL) {
            chosenWall = &maze.horizontalWalls[wallIndex]; // if the wall is NOT a loop already, and if its a wall, we remove it
            isHorizontal = wallIndex < maze.wallCount.horizontal;
            if (!isHorizontal) wallIndex -= maze.wallCount.horizontal;
        }
        else {
            continue;
        }

        Point pointA = LeftUpperPoint(isHorizontal, wallIndex, maze.size); // we get the two cells next two our wall
        Point pointB = RightLowerPoint(isHorizontal, wallIndex, maze.size);

        GenerationWall **ancestorsA = malloc(maze.size.x * maze.size.y * sizeof(Point)); // array to hold paths from start to root
        int rootDistanceA = 0;

        int followedIndex;
        do {
            GenerationWall **neighbours = GetNeighbourWalls(maze, maze.size, pointA);
            if (!neighbours) return;
            Direction neighbourDirections[4];
            for (int i = 0; i < 4; i++) {
                neighbourDirections[i] = neighbours[i] && neighbours[i]->type == AIR ? neighbours[i]->direction : -1;
            }

            followedIndex = StepTowardsRoot(neighbourDirections, &pointA);
            if (followedIndex != -1) {
                ancestorsA[rootDistanceA++] = neighbours[followedIndex];
            }
            free(neighbours);
        } while (followedIndex != -1);


        GenerationWall **ancestorsB = malloc(maze.size.x * maze.size.y * sizeof(Point)); // array to hold cells from start to root
        int rootDistanceB = 0;

        do {
            GenerationWall **neighbours = GetNeighbourWalls(maze, maze.size, pointB);
            if (!neighbours) return;
            Direction neighbourDirections[4];
            for (int i = 0; i < 4; i++) {
                neighbourDirections[i] = neighbours[i] && neighbours[i]->type == AIR ? neighbours[i]->direction : -1;
            }

            followedIndex = StepTowardsRoot(neighbourDirections, &pointB);
            if (followedIndex != -1) {
                ancestorsB[rootDistanceB++] = neighbours[followedIndex];
            }
            free(neighbours);
        } while (followedIndex != -1);

        int sharedPathLength = 0;
        while (sharedPathLength <= rootDistanceA && sharedPathLength <= rootDistanceB &&
               ancestorsA[rootDistanceA - sharedPathLength - 1] == ancestorsB[rootDistanceB - sharedPathLength - 1]) {
            sharedPathLength++;
        }

        int loopSize = rootDistanceA + rootDistanceB - 2 * sharedPathLength + 1;

        bool solutionInLoop = false;
        bool loopInLoop = false;
        for (int i = 0; i < rootDistanceA - sharedPathLength; i++) {
            if (ancestorsA[i]->isSolution)
                solutionInLoop = true;
            if (ancestorsA[i]->isLoop)
                loopInLoop = true;
        }
        for (int i = 0; i < rootDistanceB - sharedPathLength; i++) {
            if (ancestorsB[i]->isSolution)
                solutionInLoop = true;
            if (ancestorsB[i]->isLoop)
                loopInLoop = true;
        }


        if (loopInLoop) {
            free(ancestorsA);
            free(ancestorsB);
            continue;
        }

        for (int i = 0; i < rootDistanceA - sharedPathLength; i++) {
            if (solutionInLoop) ancestorsA[i]->isSolution = true;
            ancestorsA[i]->isLoop = true;
        }
        for (int i = 0; i < rootDistanceB - sharedPathLength; i++) {
            if (solutionInLoop) ancestorsB[i]->isSolution = true;
            ancestorsB[i]->isLoop = true;
        }



        if (loopSize <= smallMaxSize && loopAmountSmall < maxSmallLoops)
        { // if our count is within small, then the wall is changed to air
                                                // and we can move on, if not since there is too many small loops
            chosenWall->type = AIR;             // we just skip and try another one. Now that I am reading this
            chosenWall->isLoop = true;          // myself, I realize that this whole thing of forcing there to be a
            if (solutionInLoop)                 // a set number of small, mid, big and giant loops which is what caused
                chosenWall->isSolution = true;  // this cursed function to be so long is pretty pointless since even
            loopAmountSmall++;                  // if we just took random walls, it would spawn different sized loops
                                                // anyway. So why the fuck did I make it like this. But I already spent
                                                // a weekend making it so DO NOT DELETE. Wait I remember, Alex said
                                                // something about root and parents when making the loops, like going
                                                // from parent back to root to make loops. Damn you Alex. Give me my
                                                // weekend back.
        }
        else if (loopSize > smallMaxSize && loopSize <= midMaxSize && loopAmountMid < maxMidLoops)
        { // same here with mid sized
            chosenWall->type = AIR;
            chosenWall->isLoop = true;
            if (solutionInLoop)
                chosenWall->isSolution = true;
            loopAmountMid++;
        }
        else if (loopSize > midMaxSize && loopSize <= bigMaxSize && loopAmountBig < maxBigLoops) {
            chosenWall->type = AIR;
            chosenWall->isLoop = true;
            if (solutionInLoop)
                chosenWall->isSolution = true;
            loopAmountBig++;
        }
        else if (loopSize > bigMaxSize && loopAmountGiant < maxGiantLoops) {
            chosenWall->type = AIR;
            chosenWall->isLoop = true;
            if (solutionInLoop)
                chosenWall->isSolution = true;
            loopAmountGiant++;
        }

        free(ancestorsA);
        free(ancestorsB);
    }
}
