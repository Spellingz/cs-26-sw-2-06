#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../DataTypes/requestDataTypes.h"
#include "../Maze/maze.h"
#include "generateMaze.h"


typedef struct GenerationWall {
    char type;
    char direction;
    bool isSolution;
    union {
        bool isLoop;
        char closedSides;
    };
    char branchPotential;
    short corridorLength;
} GenerationWall;

const GenerationWall DEFAULT_WALL = {
    .type = WALL,
    .direction = 0,
    .closedSides = 0,
    .branchPotential = 0,
    .corridorLength = 1
};

typedef struct {
    MazeSize size;
    MazeWallCount wallCount;
    GenerationWall *horizontalWalls;
    GenerationWall *verticalWalls;
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
    //if (maze && maze->horizontalWalls) free(maze->horizontalWalls);
    if (maze) free(maze);
    if (frontiers) free(frontiers);
}

GenerationMaze *FillWalls(MazeSize size) {
    //Amount of horizontal walls are (width-1) times height. Opposite for vertical
    MazeWallCount wallCount = {(size.x - 1) * size.y, (size.y - 1) * size.x};
    GenerationMaze *maze = malloc(sizeof(GenerationMaze));
    if (!maze) {
        FreeMemory(maze, NULL);
        return NULL;
    }
    //Allocating a memory block that can hold both arrays. This increases cache friendliness
    GenerationWall *block = malloc(sizeof(GenerationWall) * wallCount.horizontal + sizeof(GenerationWall) * wallCount.vertical);
    if (!block) {
        FreeMemory(maze, NULL);
        return NULL;
    }
    maze->size = size;
    maze->wallCount = wallCount;
    maze->horizontalWalls = block + 0; //First array begins at the start of the block
    maze->verticalWalls = block + maze->wallCount.horizontal; //Second array begins right after the first one

    for (int i = 0; i < maze->wallCount.horizontal; i++) {
        maze->horizontalWalls[i] = DEFAULT_WALL;
    }
    for (int j = 0; j < maze->wallCount.vertical ; j++) {
        maze->verticalWalls[j] = DEFAULT_WALL;
    }
    return maze;
}

GenerationWall **LoadNeighbourWalls(GenerationMaze maze, Point pos, GenerationWall *neighbourWalls[4]) {
    int indices[4];
    LoadNeighbourWallIndices(maze.size, pos, indices);

    //Uses the indexes to get pointers to the neighbouring walls
    for (int i = 0; i < 4; i++) {
        if (indices[i] == -1)
            neighbourWalls[i] = NULL;
        else if (i < 2)
            neighbourWalls[i] = &maze.horizontalWalls[indices[i]];
        else
            neighbourWalls[i] = &maze.verticalWalls[indices[i]];
    }
    return neighbourWalls;
}

void AddNeighboursToFrontier(GenerationWall **frontier, int *frontierSize, GenerationMaze maze, Point pos) {
    //Adds the up to four neighbour walls of a point to the frontier array if they have not been added already
    GenerationWall *neighbourWalls[4];
    LoadNeighbourWalls(maze, pos, neighbourWalls);
    for (int i = 0; i < 4; i++) {
        //Skips the wall if it does not exist, or if it has at least one side in the maze.
        //Also increases the closed sides of the wall by one, since "pos" point is now becoming closed.
        if (neighbourWalls[i] == NULL || neighbourWalls[i]->closedSides++ > 0) continue;

        //The order of neighbourWalls is right, left, bottom, top. Every other wall is a positive direction.
        neighbourWalls[i]->direction = (i + 1) % 2;
        frontier[(*frontierSize)++] = neighbourWalls[i];
    }
}

WallReference GetArrayIndex(GenerationMaze maze, GenerationWall *frontierWall) {
    //Returns the index of the wall in its respective array, and whether it is in the horizontal array or not

    //If the pointer is between the first and last pointer in the vertical array
    if (frontierWall >= maze.verticalWalls && frontierWall < maze.verticalWalls + maze.wallCount.vertical)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (WallReference) {false, (long)(frontierWall - maze.verticalWalls)};

    //If the pointer is between the first and last pointer in the horizontal array
    else if (frontierWall >= maze.horizontalWalls && frontierWall < maze.horizontalWalls + maze.wallCount.horizontal)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (WallReference) {true, (long)(frontierWall - maze.horizontalWalls)};
    else {
        printf("No Index Found!\n");
        return (WallReference) {false, -1}; //FallBack
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
        wallBlock + 0,
        wallBlock + genMaze.wallCount.horizontal,
        genMaze.size,
        genMaze.wallCount,
        {genMaze.openings[0], genMaze.openings[1]}
    };
    return maze;
}


float *FrontierWeights(GenerationWall **frontier, int frontierSize, GenerationMaze maze,
                       float straightnessPriority, float branchPriority, int longestCorridor) {
    float *frontierWeights = malloc(sizeof(float) * frontierSize);
    //We create an array with weights for each frontier, its just as big as the frontier array and each matches
    if (frontierWeights == NULL) return NULL;


    for (int i = 0; i < frontierSize; i++) {

        float straightnessPotential = (float)frontier[i]->corridorLength / (float)longestCorridor;

        // our weight function = priority * potential + (priority - 1) * (potential - 1)
        frontierWeights[i] = (straightnessPriority *  straightnessPotential + (straightnessPriority - 1) *  (straightnessPotential - 1)) *
                             (branchPriority * frontier[i]->branchPotential + (branchPriority - 1) * (frontier[i]->branchPotential - 1));
    }

    return frontierWeights;
}


void SetCorridorLength(GenerationMaze maze, bool isHorizontal, int index) {
    //Line as in collumn or row.
    int lineLength = isHorizontal ? maze.size.x : maze.size.y;
    int lineNumber = index / (lineLength - 1);
    GenerationWall* walls = isHorizontal ? maze.horizontalWalls : maze.verticalWalls;
    //Should we move forwards or backwards?
    int wallDirection = walls[index].direction == 1 ? 1 : -1;

    int pointedToIndex = index + wallDirection;
    if (pointedToIndex / (lineLength - 1) == lineNumber) {
        walls[pointedToIndex].corridorLength = walls[index].corridorLength + 1;
    }

    int otherEndIndex = index - wallDirection * walls[index].corridorLength;
    if (otherEndIndex / (lineLength - 1) == lineNumber) {
        walls[otherEndIndex].corridorLength = walls[index].corridorLength + 1;
    }
}

GenerationWall *PopRandomFrontier(GenerationWall **frontier, int *frontierSize, GenerationMaze maze,
                                  float straightnessPriority, float branchPriority, int longestCorridor) {
    float *weights = FrontierWeights(frontier, *frontierSize, maze, straightnessPriority, branchPriority, longestCorridor);

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
    AddNeighboursToFrontier(frontiers, &frontierSize, *maze, startPoint);
    int longestCorridor = 1;

    bool hasRemovedOneWall = false;
    while (frontierSize > 0) {
        GenerationWall *rndFrontier = PopRandomFrontier(frontiers, &frontierSize, *maze, data.straightness, data.branches, longestCorridor); // change so the numbers are branch potential and straightness potential
        //If both sides of the wall is in the closed maze, it is not actually a frontier, and should not be removed
        if (rndFrontier->closedSides >= 2) continue;
        //printf("Popped frontier with length: %hd, branch: %hhd\n", rndFrontier->corridorLength, rndFrontier->branchPotential);

        //Gets the index of the wall in its corresponding array
        WallReference wallRef = GetArrayIndex(*maze, rndFrontier);
        if (wallRef.index == -1) {
            FreeMemory(maze, frontiers);
            return (ExportData){-1}; //crash
        }

        if (hasRemovedOneWall) {
            Point fromPoint = PointPointedFrom(wallRef.isHorizontal, wallRef.index, size, rndFrontier->direction);
            GenerationWall* neighbours[4];
            LoadNeighbourWalls(*maze, fromPoint, neighbours);
            for (int i = 0; i < 4; i++) {
                if (neighbours[i])
                    neighbours[i]->branchPotential = 1;
            }
        }
        hasRemovedOneWall = true;

        SetCorridorLength(*maze, wallRef.isHorizontal, wallRef.index);
        if (rndFrontier->corridorLength + 1 > longestCorridor)
            longestCorridor = rndFrontier->corridorLength + 1;

        Point frontierPos = PointPointedTo(wallRef.isHorizontal, wallRef.index, size, rndFrontier->direction);

        AddNeighboursToFrontier(frontiers, &frontierSize, *maze, frontierPos);

        rndFrontier->type = AIR;
        //PrintGenerationMaze(*maze);
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
        GenerationWall *neighbours[4];
        LoadNeighbourWalls(maze, point, neighbours);
        Direction neighbourDirections[4] = {
            neighbours[0] && neighbours[0]->type != WALL ? neighbours[0]->direction : -1,
            neighbours[1] && neighbours[1]->type != WALL ? neighbours[1]->direction : -1,
            neighbours[2] && neighbours[2]->type != WALL ? neighbours[2]->direction : -1,
            neighbours[3] && neighbours[3]->type != WALL ? neighbours[3]->direction : -1,
        };
        chosenIndex = StepTowardsRoot(neighbourDirections, &point);
        if (chosenIndex != -1) {
            neighbours[chosenIndex]->isSolution = !neighbours[chosenIndex]->isSolution;
        }
    } while (chosenIndex != -1);
}

void MarkSolution(GenerationMaze maze, Point start, Point goal) {
    FlipSolutionMarkingToRoot(maze, start);
    FlipSolutionMarkingToRoot(maze, goal);
}

void AddLoops(GenerationMaze maze, double loopInput) {
    int maxloopAttempts = (int)((maze.wallCount.horizontal + maze.wallCount.vertical) * loopInput);
    int maxSmallLoops = (int)(maxloopAttempts * 0.6); // total amount of "small loops" allowed
    int maxMidLoops = (int)(maxloopAttempts * 0.25);
    int maxBigLoops = (int)(maxloopAttempts * 0.10);
    int maxGiantLoops = maxloopAttempts - maxSmallLoops - maxMidLoops - maxBigLoops;
    int loopAmountSmall = 0;
    int loopAmountMid = 0;
    int loopAmountBig = 0;
    int loopAmountGiant = 0;

    int smallMinSize = 6; // definitions for each
    int midMinSize = 20;
    int bigMinSize = 60;
    int giantMinSize = 150;

    for (int i = 0; i < maze.wallCount.horizontal; i++) {
        maze.horizontalWalls[i].isLoop = false;
    }

    for (int i = 0; i < maze.wallCount.vertical; i++) {
        maze.verticalWalls[i].isLoop = false;
    }

    for (int loopAttempts = 0; loopAttempts < maxloopAttempts; loopAttempts++) {
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
            GenerationWall *neighbours[4];
            LoadNeighbourWalls(maze, pointA, neighbours);
            Direction neighbourDirections[4];
            for (int i = 0; i < 4; i++) {
                neighbourDirections[i] = neighbours[i] && neighbours[i]->type == AIR ? neighbours[i]->direction : -1;
            }

            followedIndex = StepTowardsRoot(neighbourDirections, &pointA);
            if (followedIndex != -1) {
                ancestorsA[rootDistanceA++] = neighbours[followedIndex];
            }
        } while (followedIndex != -1);


        GenerationWall **ancestorsB = malloc(maze.size.x * maze.size.y * sizeof(Point)); // array to hold cells from start to root
        int rootDistanceB = 0;

        do {
            GenerationWall *neighbours[4];
            LoadNeighbourWalls(maze, pointB, neighbours);
            Direction neighbourDirections[4];
            for (int i = 0; i < 4; i++) {
                neighbourDirections[i] = neighbours[i] && neighbours[i]->type == AIR ? neighbours[i]->direction : -1;
            }

            followedIndex = StepTowardsRoot(neighbourDirections, &pointB);
            if (followedIndex != -1) {
                ancestorsB[rootDistanceB++] = neighbours[followedIndex];
            }
        } while (followedIndex != -1);

        int sharedPathLength = 0;
        while (sharedPathLength <= rootDistanceA && sharedPathLength <= rootDistanceB &&
               ancestorsA[rootDistanceA - sharedPathLength - 1] == ancestorsB[rootDistanceB - sharedPathLength - 1]) {
            sharedPathLength++;
        }

        int loopSize = rootDistanceA + rootDistanceB - 2 * sharedPathLength + 1;

        bool loopSizeApproved = false;
        if (loopSize >= smallMinSize && loopSize < midMinSize && loopAmountSmall < maxSmallLoops)
        { // if our count is within small, then the wall is changed to air
            loopAmountSmall++;                  // and we can move on, if not since there is too many small loops
            loopSizeApproved = true;            // we just skip and try another one. Now that I am reading this
                                                // myself, I realize that this whole thing of forcing there to be a
                                                // a set number of small, mid, big and giant loops which is what caused
                                                // this cursed function to be so long is pretty pointless since even
                                                // if we just took random walls, it would spawn different sized loops
                                                // anyway. So why the fuck did I make it like this. But I already spent
                                                // a weekend making it so DO NOT DELETE. Wait I remember, Alex said
                                                // something about root and parents when making the loops, like going
                                                // from parent back to root to make loops. Damn you Alex. Give me my
                                                // weekend back.
        }
        else if (loopSize >= midMinSize && loopSize < bigMinSize && loopAmountMid < maxMidLoops)
        { // same here with mid sized
            loopAmountMid++;
            loopSizeApproved = true;
        }
        else if (loopSize >= bigMinSize && loopSize < giantMinSize && loopAmountBig < maxBigLoops) {
            loopAmountBig++;
            loopSizeApproved = true;
        }
        else if (loopSize >= giantMinSize && loopAmountGiant < maxGiantLoops) {
            loopAmountGiant++;
            loopSizeApproved = true;
        }

        if (loopSizeApproved) {
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

            free(ancestorsA);
            free(ancestorsB);

            chosenWall->type = AIR;
            chosenWall->isLoop = true;
            if (solutionInLoop)
                chosenWall->isSolution = true;
        }
    }
}
