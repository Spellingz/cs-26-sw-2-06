#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../DataTypes/requestDataTypes.h"
#include "../Maze/maze.h"
#include "generateMaze.h"


typedef struct {
    bool type;
    bool direction;
    bool isLoop;
    char closedSides;   //removed union
} GenerationWall;

const GenerationWall DEFAULT_WALL = {
    .type = WALL,
    .direction = 0,
    .isLoop = false,
    .closedSides = 0,
};

typedef struct {
    MazeSize size;
    MazeWallCount wallCount;
    GenerationWall *horizontalWalls;
    GenerationWall *verticalWalls;
    Point root;
} GenerationMaze;



void PrintMaze(GenerationMaze maze, MazeSize size) {
    int h = 0, v = 0;
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
    printf("+\n");
    for (int i = 0; i < size.y * 2 - 1; i++) {
        if (i % 2 == 0) {
            printf("|");
            for(int j = 0; j < size.x - 1; j++, h++) {
                GenerationWall wall = maze.horizontalWalls[h];
                printf(" %c", wall.type ? wall.closedSides == 1 ? 'I' : '|' : wall.direction ? '>' : '<');
            }
            printf(" |");
        }
        else {
            printf("+");
            for(int j = 0; j < size.x; j++, v++) {
                GenerationWall wall = maze.verticalWalls[(v % size.x) * (size.y - 1) + v/size.x];
                printf("%c+", wall.type ? wall.closedSides == 1 ? '~' : '-' : wall.direction ? 'v' : '^');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
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
    int* indices = GetNeighbourWallIndices(size, pos);
    if (!indices) return NULL;

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
    free(indices);
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
        printf("No Index Found!");
        return (ArrIndexResult){false, -1}; //FallBack
    }
}

bool *GenerationWallArrToBoolArr(const GenerationWall* arr, int size) {
    bool *boolArr = malloc(sizeof(bool) * size);
    if (!boolArr) return NULL;
    for (int i = 0; i < size; i++)
        boolArr[i] = arr[i].type;
    return boolArr;
}

Wall *GenerationWallArrToWallArr(const GenerationWall* arr, unsigned long size) {
    Wall *wallArr = malloc(sizeof(Wall) * size);
    if (!wallArr) return NULL;
    for (int i = 0; i < size; i++)
        wallArr[i] = (Wall) {arr[i].type, arr[i].direction};
    return wallArr;
}

Maze *GenerationMazeToMaze(GenerationMaze generationMaze) {
    Maze* maze = malloc(sizeof(maze));
    if (!maze) return NULL;
    *maze = (Maze) {
        generationMaze.size,
        GenerationWallArrToWallArr(generationMaze.horizontalWalls, generationMaze.wallCount.horizontal),
        GenerationWallArrToWallArr(generationMaze.verticalWalls,   generationMaze.wallCount.vertical),
        generationMaze.wallCount,
        generationMaze.root
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


    //getting branch potential
    for (int i = 0; i < frontierSize; i++)
    {
        ArrIndexResult arr = GetArrayIndex(maze, frontier[i]);
        //we use the get ArrindexResult to get the index of our frontier

        Point position = IndexToPos(arr.isHorizontal, arr.index, size);
        //Then we get our position by using the indexToPos function

        GenerationWall **neighbors = GetNeighbourWalls(maze, size, position);

        if (arr.isHorizontal && frontier[i]->direction == LEFT)
        //now the indexToPos automatically shifts us to the left side, however, if our direction is left, we need to be on the right side
        {
            position.x++; //So we shift to the right cell
        }
        if (!arr.isHorizontal && frontier[i]->direction == UP) //same idea here, just vertically
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
        arr = GetArrayIndex(maze, frontier[i]);
        bool keepGoing = true;

        int index = arr.index;
        while (keepGoing) {
            if (arr.isHorizontal && frontier[i]->direction == LEFT)
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

            else if (arr.isHorizontal && frontier[i]->direction == RIGHT) {
                if ((index - 1) / (size.x-1) == index / (size.x-1) && maze.horizontalWalls[--index].type == AIR) {
                    straightnesscount++;
                } else {
                    keepGoing = false;
                }
            }

            else if (!arr.isHorizontal && frontier[i]->direction == UP) {
                if ((index + 1) / (size.y-1) == index / (size.y-1) && maze.verticalWalls[++index].type == AIR) {
                    straightnesscount++;
                } else {
                    keepGoing = false;
                }
            }

            else if (!arr.isHorizontal && frontier[i]->direction == DOWN) {
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

    //printf("Changed weights.\n");
    for (int i = 0; i < *frontierSize; i++) {
        //now pay attention, we are adding each weight to the total
        total += weights[i];
        weights[i] = total;
        //printf("%f\n",weights[i]);

        //while that is happening each temp[] is getting a value that is higher and higher, increased each time by the next weight value
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
    if (!frontiers)  return FreeMemory(maze, frontiers), (ExportData){-1}; //crash

    maze->root = (Point) {rand() % size.x, rand() % size.y};
    AddNeighboursToFrontier(frontiers, &frontierSize, *maze, size, maze->root);
    while (frontierSize > 0) {
        GenerationWall *rndFrontier = PopRandomFrontier(frontiers, &frontierSize, *maze, size, data.straightness, data.branches); // change so the numbers are branch potential and straightness potential
        //If both sides of the wall is in the closed maze, it is not actually a frontier, and should not be removed
        if (rndFrontier->closedSides >= 2) continue;

        //Gets the index of the wall in its corresponding array
        ArrIndexResult arrIndex = GetArrayIndex(*maze, rndFrontier);
        if (arrIndex.index == -1) return FreeMemory(maze, frontiers), (ExportData){-1}; //crash

        //Uses the index to get the position just above/to the left of the wall.
        Point frontierPos = IndexToPos(arrIndex.isHorizontal, arrIndex.index, size);
        //If the wall has a positive direction, shift the point 1 down/to the right
        if (arrIndex.isHorizontal && rndFrontier->direction == RIGHT) frontierPos.x++;
        if (!arrIndex.isHorizontal && rndFrontier->direction == DOWN) frontierPos.y++;
        AddNeighboursToFrontier(frontiers, &frontierSize, *maze, size, frontierPos);

        rndFrontier->type = AIR;
    }

    AddLoops(maze,size,data.loops);


    // printMaze(*maze, size);


    Maze *properMaze = GenerationMazeToMaze(*maze);
    if (properMaze && properMaze->horizontalWalls && properMaze->verticalWalls) SaveMaze(*properMaze, 7);

    FreeMaze(properMaze);


    bool* horizontalBoolArr = GenerationWallArrToBoolArr(maze->horizontalWalls, maze->wallCount.horizontal);
    bool* verticalBoolArr = GenerationWallArrToBoolArr(maze->verticalWalls, maze->wallCount.vertical);

    ExportData exportMaze = {data.id, maze->wallCount.horizontal, maze->wallCount.vertical, horizontalBoolArr, verticalBoolArr};

    FreeMemory(maze, frontiers);
    return exportMaze;
}

void AddLoops(GenerationMaze *maze, MazeSize size, double loopInput, double loopSizeInput)
{                                    // Should be optimized, takes like a billion years to generate a 200x200 maze
    int cellCount = size.x * size.y; // or we should limit maze size to 100x100
    int count = 0;
    int maxLoops = cellCount / 10;
    int loopAmount = (int)(maxLoops * loopInput); // how many loops we max can have
    int outerAttempts = 0;
    int maxOuterAttempts = loopAmount * 50;

    int loopAmountSmall = (int)loopAmount * 0.6; // total amount of "small loops allowed"
    int loopAmountMid = (int)loopAmount * 0.25;  // same etc
    int loopAmountBig = (int)loopAmount * 0.10;
    int loopAmountGiant = loopAmount - loopAmountSmall - loopAmountMid - loopAmountBig;
    int loopAmountSmalltemp = 0;
    int loopAmountMidtemp = 0;
    int loopAmountBigtemp = 0;
    int loopAmountGianttemp = 0;

    bool switcheroo = true; // to switch between vertical and horizontal

    int smallMin = 4; // definitions for each
    int smallMax = 15;

    int midMin = smallMax + 1;
    int midMax = 50;

    int bigMin = midMax + 1;
    int bigMax = 150;

    int giantMin = bigMax + 1;
    int giantMax = bigMax + 1 + cellCount / 4;

    while (count < loopAmount && outerAttempts < maxOuterAttempts)
    { // we are choosing which kind of loop(size) we want
        outerAttempts++;
        int roll = rand() % 100 + 1;
        int currentLoopSize = 0;

        if (roll <= 60)
        {
            currentLoopSize = 1;
        }
        else if (roll <= 85)
        {
            currentLoopSize = 2;
        }
        else if (roll <= 95)
        {
            currentLoopSize = 3;
        }
        else if (roll <= 100)
        {
            currentLoopSize = 4;
        }
        else if (currentLoopSize == 0)
        {
            continue;
        }

        int wallArraySize;
        GenerationWall *wallArray;

        if (switcheroo)
        {

            wallArraySize = maze->wallCount.horizontal;
            wallArray = maze->horizontalWalls;
        }
        else
        {
            wallArraySize = maze->wallCount.vertical;
            wallArray = maze->verticalWalls;
        }

        // we start horizontally
        // horizontal ie side to side
        int attempts = 0;
        int maxAttempts = wallArraySize * 50; // to ensure it stops at some point incase something breaks (its my code so its gonna break eventually)
        // picking 50 times the amount, incase it chooses the same wall multiple times
        bool tempbreak = true;
        int randomNumber = -1;
        GenerationWall *chosenWall = NULL;

        while (tempbreak)
        {
            randomNumber = rand() % wallArraySize; // we create a random number that is at most as big as there are horizontal walls
            if (wallArray[randomNumber].isLoop == false && wallArray[randomNumber].type == WALL)
            {
                chosenWall = &wallArray[randomNumber]; // if the horizontal wall is NOT a loop already, and if its a wall, we remove it
                tempbreak = false;                     // and we stop
            }
            attempts++;
            if (attempts >= maxAttempts)
            { // if we keep trying but its always AIR or already a loop, we give up
                break;
            }
        }

        if (chosenWall == NULL)
        { // if we dont get a valid wall, we move on and just go to vertical
            switcheroo = false;
            continue;
        }

        ArrIndexResult arr = GetArrayIndex(*maze, chosenWall); // we get our wall info like isHorizontal and index

        Point position = IndexToPos(arr.isHorizontal, arr.index, size); // we get the two cells next two our wall

        Point position2 = position; // second cell

        if (switcheroo)
        {
            position2.x++;
        }
        else
        {
            position2.y++;
        }

        int trueCount = -1; // to count how long till shared ancestor

        bool foundFirst = false; // NOW WE MOVE
        int oneCount = 0;
        Point *initialArr = malloc(cellCount * sizeof(Point)); // array to hold cells from start to root
        int initialCount = 0;

        while (!foundFirst)
        {
            bool moved = false;

            GenerationWall **oneNeighbors = GetNeighbourWalls(*maze, size, position); // we get our neighbors of the FIRST cell ie the left one

            if (position.x == maze->root.x && position.y == maze->root.y)
            { // if our current cell is the root, we add it to our array and break loop
                if (initialCount < cellCount)
                {
                    initialArr[initialCount++] = position;
                }
                free(oneNeighbors);
                foundFirst = true;
                break;
            }

            oneNeighbors[0]; // right wall of position
            oneNeighbors[1]; // left wall of position
            oneNeighbors[2]; // down wall of position
            oneNeighbors[3]; // up wall of position

            if (oneNeighbors[0] != NULL && oneNeighbors[0]->type == AIR && oneNeighbors[0]->direction == LEFT &&
                oneNeighbors[0]->isLoop == false)
            {
                // parent is to the RIGHT
                initialArr[initialCount] = position;
                initialCount++;
                position.x++;
                oneCount++;
                moved = true;
            }
            else if (oneNeighbors[1] != NULL && oneNeighbors[1]->type == AIR && oneNeighbors[1]->direction == RIGHT && oneNeighbors[1]->isLoop == false)
            {
                // parent is to the LEFT
                initialArr[initialCount] = position;
                initialCount++;
                position.x--;
                oneCount++;
                moved = true;
            }
            else if (oneNeighbors[2] != NULL && oneNeighbors[2]->type == AIR && oneNeighbors[2]->direction == UP && oneNeighbors[2]->isLoop == false)
            {
                // parent is to the DOWN
                initialArr[initialCount] = position;
                initialCount++;
                position.y++;
                oneCount++;
                moved = true;
            }
            else if (oneNeighbors[3] != NULL && oneNeighbors[3]->type == AIR && oneNeighbors[3]->direction == DOWN && oneNeighbors[3]->isLoop == false)
            {
                // parent is to the UP
                initialArr[initialCount] = position;
                initialCount++;
                position.y--;
                oneCount++;
                moved = true;
            }

            free(oneNeighbors);

            if (!moved)
            { // if we didnt move, stop cuz something is wrong
                break;
            }
        }

        bool foundSecond = false;
        int twoCount = 0;
        Point *secondaryArr = malloc(cellCount * sizeof(Point));
        int secondaryCount = 0;

        while (!foundSecond)
        { // now we repeat the exact same process but with the right cell
            bool moved = false;
            GenerationWall **twoNeighbors = GetNeighbourWalls(*maze, size, position2);

            if (position2.x == maze->root.x && position2.y == maze->root.y)
            {
                if (secondaryCount < cellCount)
                {
                    secondaryArr[secondaryCount++] = position2;
                }
                free(twoNeighbors);
                foundSecond = true;
                break;
            }

            if (twoNeighbors[0] != NULL && twoNeighbors[0]->type == AIR && twoNeighbors[0]->direction == LEFT &&
                twoNeighbors[0]->isLoop == false)
            {
                // parent is to the RIGHT
                secondaryArr[secondaryCount] = position2;
                secondaryCount++;
                position2.x++;
                twoCount++;
                moved = true;
            }
            else if (twoNeighbors[1] != NULL && twoNeighbors[1]->type == AIR && twoNeighbors[1]->direction == RIGHT && twoNeighbors[1]->isLoop == false)
            {
                // parent is to the LEFT
                secondaryArr[secondaryCount] = position2;
                secondaryCount++;
                position2.x--;
                twoCount++;
                moved = true;
            }
            else if (twoNeighbors[2] != NULL && twoNeighbors[2]->type == AIR && twoNeighbors[2]->direction == UP && twoNeighbors[2]->isLoop == false)
            {
                // parent is to the DOWN
                secondaryArr[secondaryCount] = position2;
                secondaryCount++;
                position2.y++;
                twoCount++;
                moved = true;
            }
            else if (twoNeighbors[3] != NULL && twoNeighbors[3]->type == AIR && twoNeighbors[3]->direction == DOWN && twoNeighbors[3]->isLoop == false)
            {
                // parent is to the UP
                secondaryArr[secondaryCount] = position2;
                secondaryCount++;
                position2.y--;
                twoCount++;
                moved = true;
            }

            free(twoNeighbors);

            if (!moved)
            {
                break;
            }
        }

        int i = 1;
        while (i <= initialCount && i <= secondaryCount &&
               initialArr[initialCount - i].x == secondaryArr[secondaryCount - i].x &&
               initialArr[initialCount - i].y == secondaryArr[secondaryCount - i].y)
        {
            i++;
        }
        trueCount = initialCount - (i - 1) + secondaryCount - (i - 1) + 1;

        if (foundFirst && foundSecond)
        { // these two are seperated for literally no reason other than because I can
            if (currentLoopSize == 1)
            { // now we check what loopsize we are on again, in this case small
                if (trueCount >= smallMin && trueCount <= smallMax)
                { // if our count is within small, then the wall is changed to air
                    if (loopAmountSmalltemp < loopAmountSmall)
                    {                              // and we can move on, if not since there is too many small loops
                        chosenWall->type = AIR;    // we just skip and try another one. Now that I am reading this
                        chosenWall->isLoop = true; // myself, I realize that this whole thing of forcing there to be a
                        loopAmountSmalltemp++;     // a set number of small, mid, big and giant loops which is what caused
                        count++;                   // this cursed function to be so long is pretty pointless since even
                        switcheroo = !switcheroo;  // if we just took random walls, it would spawn different sized loops
                                                   // anyway. So why the fuck did I make it like this. But I already spent
                                                   // a weekend making it so DO NOT DELETE. Wait I remember, Alex said
                        free(initialArr);          // something about root and parents when making the loops, like going
                        free(secondaryArr);        // from parent back to root to make loops. Damn you Alex. Give me my
                                                   // weekend back.
                        continue;
                    };
                }
            }
            else if (currentLoopSize == 2)
            { // same here with mid sized
                if (trueCount >= midMin && trueCount <= midMax)
                {
                    if (loopAmountMidtemp < loopAmountMid)
                    {
                        chosenWall->type = AIR;
                        chosenWall->isLoop = true;
                        loopAmountMidtemp++;
                        switcheroo = !switcheroo;
                        count++;

                        free(initialArr);
                        free(secondaryArr);

                        continue;
                    };
                }
            }
            else if (currentLoopSize == 3)
            {
                if (trueCount >= bigMin && trueCount <= bigMax)
                {
                    if (loopAmountBigtemp < loopAmountBig)
                    {
                        chosenWall->type = AIR;
                        chosenWall->isLoop = true;
                        loopAmountBigtemp++;
                        switcheroo = !switcheroo;
                        count++;

                        free(initialArr);
                        free(secondaryArr);

                        continue;
                    };
                }
            }
            else if (currentLoopSize == 4)
            {
                if (trueCount >= giantMin && trueCount <= giantMax)
                {
                    if (loopAmountGianttemp < loopAmountGiant)
                    {
                        chosenWall->type = AIR;
                        chosenWall->isLoop = true;
                        loopAmountGianttemp++;
                        switcheroo = !switcheroo;
                        count++;

                        free(initialArr);
                        free(secondaryArr);

                        continue;
                    };
                }
            }
        }

        free(initialArr);
        free(secondaryArr);

        switcheroo = !switcheroo; // after all of that, we set switcheroo to false so it moved on to vertical on the next iteration, to ensure an even balance of both vertical and horizontal
    }
}