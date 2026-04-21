#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// #include "mazeDataTypes.c"


typedef struct ExportData {int x;} ExportData;
typedef struct Data {} Data;

typedef enum {
    UP = 0,
    DOWN = 1,
    LEFT = 0,
    RIGHT = 1,
} Direction;

typedef enum {
	AIR = 0,
	WALL = 1,
} Type;

typedef struct {
	bool type;
	bool direction;
    union {
        bool isLoop;
        char closedSides;
    };
} Wall;

const Wall DefaultWall = {
    .type = WALL,
    .direction = 0,
    .closedSides = 0,
};

typedef struct {short x,y;} Point;

typedef struct {
	char dir;
	Point start;
	Point end;
	Wall* walls;
} Path;

typedef struct {
    unsigned short x, y;
} MazeSize;

typedef struct {
    long horizontal, vertical;
} MazeWallCount;

typedef struct {
    MazeWallCount wallCount;
    Wall *horizontalArr;
    Wall *verticalArr;
} MazeStruct;

void printMaze(MazeStruct maze, MazeSize size) {
    int h = 0, v = 0;
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
    printf("+\n");
    for (int i = 0; i < size.y * 2 - 1; i++) {
        if (i % 2 == 0) {
            printf("|");
            for(int j = 0; j < size.x - 1; j++, h++) {
                Wall wall = maze.horizontalArr[h];
                printf(" %c", wall.type ? wall.closedSides ? 'I' : '|' : wall.direction ? '>' : '<');
            }
            printf(" |");
        }
        else {
            printf("+");
            for(int j = 0; j < size.x; j++, v++) {
                Wall wall = maze.verticalArr[(v % size.x) * (size.y - 1) + v/size.x];
                printf("%c+", wall.type ? wall.closedSides ? '~' : '-' : wall.direction ? 'v' : '^');
            }
        }
        printf("\n");
    }
    printf("+");
    for (int i = 0; i < size.x * 2 - 1; i++) printf("-");
    printf("+\n\n");
}

void freeMemory(MazeStruct* maze, Wall** frontiers) {
    if (maze && maze->horizontalArr) free(maze->horizontalArr);
    if (maze) free(maze);
    if (frontiers) free(frontiers);
}

MazeStruct *fillWalls(MazeSize size) {
    //Amount of horizontal walls are (width-1) times height. Opposite for vertical
    MazeWallCount wallCount = {(size.x - 1) * size.y, (size.y - 1) * size.x};
    MazeStruct *maze = malloc(sizeof(MazeStruct));
    if (!maze) return freeMemory(maze, NULL), NULL;
    //Allocating a memory block that can hold both arrays. This increases cache friendliness
    Wall *block = malloc(sizeof(Wall) * wallCount.horizontal + sizeof(Wall) * wallCount.vertical);
    if (!block) return freeMemory(maze, NULL), NULL;
    maze->wallCount = wallCount;
    maze->horizontalArr = block; //First array begins at the start of the block
    maze->verticalArr = block + maze->wallCount.horizontal; //Second array begins right after the first one

    for (int i = 0; i < maze->wallCount.horizontal; i++) {
        maze->horizontalArr[i] = DefaultWall;
    }
    for (int j = 0; j < maze->wallCount.vertical ; j++) {
        maze->verticalArr[j] = DefaultWall;
    }
    return maze;
}

int getRightWallIndex(Point pos, MazeSize size) {
    //Returns the index in horizontalArr of the wall to the right of the point
    if (pos.x < 0 || pos.x >= size.x - 1 || pos.y < 0 || pos.y >= size.y)
        //Out of bounds
        return -1;

    return pos.x + pos.y * (size.x - 1);
}

int getLowerWallIndex(Point pos, MazeSize size) {
    //Returns the index in verticalArr of the wall below the point
    if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y - 1)
        //Out of bounds
        return -1;

    return pos.y + pos.x * (size.y - 1);
}

Wall **getNeighbourWalls(MazeStruct *maze, MazeSize size, Point pos) {
    Wall **neighbourWalls = malloc(sizeof(Wall*)*4);

    //Gets the indexes of the neighbouring walls in their respective arrays
    int index[4] = {
        getRightWallIndex((Point){pos.x, pos.y}, size),
        getRightWallIndex((Point){pos.x-1, pos.y}, size),
        getLowerWallIndex((Point){pos.x, pos.y}, size),
        getLowerWallIndex((Point){pos.x, pos.y-1}, size),
    };

    //Uses the indexes to get pointers to the neighbouring walls
    for (int i = 0; i < 4; i++) {
        if (index[i] == -1) 
            neighbourWalls[i] = NULL;
        else if (i<2)
            neighbourWalls[i] = &maze->horizontalArr[index[i]];
        else
            neighbourWalls[i] = &maze->verticalArr[index[i]];
    }
    return neighbourWalls;
}


void addNeighboursToFrontier(Wall **frontier, int *frontierSize, MazeStruct *maze, MazeSize size, Point pos) {
    //Adds the up to four neighbour walls of a point to the frontier array if they have not been added already
    Wall **neighbourWalls = getNeighbourWalls(maze, size, pos);
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


Wall *popRandomFrontier(Wall **frontier, int *frontierSize, MazeStruct *maze, MazeSize size, float straightnessPriority, float BranchPriority) {

float *weights = Weight(frontier, *frontierSize, maze, size, straightnessPriority, BranchPriority);

double temp[*frontierSize];     //create a temporary array, should probably give it a better name but meh

float total = 0.0;

for (int i = 0; i < *frontierSize; i++) { //now pay attention, we are adding each weight to the total
    total += weights[i];
    temp[i] = total;           //while that is happening each temp[] is getting a value that is higher and higher, increased each time by the next weight value
}

float random = ((float)rand() / RAND_MAX) * total;    //now we get a random number between 0 and the result of our total

int chosenIndex = 0;
for (int i = 0; i < *frontierSize; i++) {      //whatever number we got, we are now checking here and the index of the number which matches is our chosenindex 
    if (random <= temp[i]) {
        chosenIndex = i;                       //Assume we got:
        break;                                 //total: 1.93,   random number: 0.73
    }                                          //Indexes: 1: 0.35, 2: 0.74, 3: 1.12, 4: 1.37 etc
}                                              // 0.73 is below index 2 0.74. So our chosen index is 2.


    Wall *poppedFrontier = frontier[chosenIndex];  //now the frontier with index 2 is the one to get removed
    //Moves the last element in the frontier to the deleted element's place
    frontier[chosenIndex] = frontier[--(*frontierSize)];

    free(weights);

    return poppedFrontier;
}

typedef struct ArrIndexResult {
    bool isHorizontal;
    int index;
} ArrIndexResult;

ArrIndexResult getArrayIndex(MazeStruct *maze, Wall *frontierWall) {
    //Returns the index of the wall in its respective array, and whether it is in the horizontal array or not

    //If the pointer is between the first and last pointer in the vertical array
    if (frontierWall >= maze->verticalArr && frontierWall < maze->verticalArr + maze->wallCount.vertical)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){0, (int)(frontierWall - maze->verticalArr)};

    //If the pointer is between the first and last pointer in the horizontal array
    else if (frontierWall >= maze->horizontalArr && frontierWall < maze->horizontalArr + maze->wallCount.horizontal)
        //Difference between the pointer and the address of the start of the array is the position in the array
        return (ArrIndexResult){1, (int)(frontierWall - maze->horizontalArr)};
    else {
        printf("No Index Found!");
        return (ArrIndexResult){0, -1}; //FallBack
    }
}

Point indexToPos(bool isHorizontal, int index, MazeSize mazeSize) {
    if (isHorizontal)
        //Returns the position of the point just left of the wall.
        return (Point){index % (mazeSize.x-1), index / (mazeSize.x-1)};
    else
        //Returns the position of the point just above the wall.
        return (Point){index / (mazeSize.y-1), index % (mazeSize.y-1)};
}

ExportData generateMaze(Data data) {
    // MazeSize size = {data.size.x, data.size.y};
    MazeSize size = {4,3};
    MazeStruct *maze = fillWalls(size);
    if (!maze) return (ExportData){-1}; //crash

    int frontierSize = 0;
    Wall** frontiers = malloc(
        sizeof(Wall*) * maze->wallCount.horizontal +
        sizeof(Wall*) * maze->wallCount.vertical);
    if (!frontiers)  return freeMemory(maze, frontiers), (ExportData){-1}; //crash

    Point startPos = {rand() % size.x, rand() % size.y};
    addNeighboursToFrontier(frontiers, &frontierSize, maze, size, startPos);
    while (frontierSize > 0) {
        Wall *rndFrontier = popRandomFrontier(frontiers, &frontierSize);
        //If both sides of the wall is in the closed maze, it is not actually a frontier, and should not be removed
        if (rndFrontier->closedSides >= 2) continue;

        //Gets the index of the wall in its corresponding array
        ArrIndexResult arrIndex = getArrayIndex(maze, rndFrontier);
        if (arrIndex.index == -1) return freeMemory(maze, frontiers), (ExportData){-1}; //crash

        //Uses the index to get the position just above/to the left of the wall.
        Point frontierPos = indexToPos(arrIndex.isHorizontal, arrIndex.index, size);
        //If the wall has a positive direction, shift the point 1 down/to the right
        if (arrIndex.isHorizontal && rndFrontier->direction == RIGHT) frontierPos.x++;
        if (!arrIndex.isHorizontal && rndFrontier->direction == DOWN) frontierPos.y++;
        addNeighboursToFrontier(frontiers, &frontierSize, maze, size, frontierPos);

        rndFrontier->type = AIR;
    }
    freeMemory(maze, frontiers);

    return (ExportData){1};
}













float *Weight(Wall **frontier, int frontierSize, MazeStruct *maze, MazeSize size, float straightnessPriority, float BranchPriority) 
{

    float *FrontierWeight = malloc(sizeof(float) * frontierSize);    //We create an array with weights for each frontier, its just as big as the frontier array and each matches
    if (FrontierWeight == NULL) return NULL;

    int branchPotential;
    float straightnessPotential;

    int max_len = 0; 
    int max_len_count = 0; 

    for (int y = 0; y<size.y; y++){      //calculating max_len, problem with this is that it checks the WALLS, not the cells, so three cells open are seperated by 2 walls. We fix this later by just adding 1

        max_len_count = 0; 

        for (int x = 0; x<size.x; x++){

            Point position = {x,y};                

            Wall **neighbors = getNeighbourWalls(maze, size, position);

            Wall *right = neighbors[0];
            Wall *left = neighbors[1];
            Wall *down = neighbors[2];
            Wall *up = neighbors[3];

           if (right != NULL && right->type == AIR){    //After getting all neighbors we are checking if if the right side, cuz we are moving right, is open
                max_len_count++;           //if yes, max_len_count is increased
            } else { 

                if (max_len_count > max_len){     //if not, max len if it is smaller than the max_len_count gets max_len_count's value, and max_len_count is reset
                    max_len = max_len_count;
                }

                max_len_count = 0;

            }

            free(neighbors);

        }

        if (max_len_count > max_len){
            max_len = max_len_count;           //if we are going out of bounds, like moving straight towards the exit, we need this to ensure the last one is also added.
        }

    }

    max_len_count = 0;



    for (int x = 0; x<size.x; x++){    //same as before but now we are checking vertically instead

        max_len_count = 0; 

        for (int y = 0; y<size.y; y++){

            Point position = {x,y};

            Wall **neighbors = getNeighbourWalls(maze, size, position);

            Wall *right = neighbors[0];
            Wall *left = neighbors[1];
            Wall *down = neighbors[2];
            Wall *up = neighbors[3];

            if (down != NULL && down->type == AIR){ 
                max_len_count++;
            } else {

                if (max_len_count > max_len){
                    max_len = max_len_count;
                }

                max_len_count = 0;
            }

            free(neighbors);

        }

        if (max_len_count > max_len){
            max_len = max_len_count;
        }
    }

    max_len_count = 0;




    for (int i = 0; i < frontierSize; i++)     //getting branch potential
    {


            ArrIndexResult arr = getArrayIndex(maze, frontier[i]);       //we use the get ArrindexResult to get the index of our frontier
        
            Point position = indexToPos(arr.isHorizontal, arr.index, size);   //Then we get our position by using the indexToPos function

            if (arr.isHorizontal && frontier[i]->direction == RIGHT)    //now the indexToPos automatically shifts us to the left side, however, if our direction is right, we need to be on the right side
            {
                position.x++;                                       //So we shift to the right cell
            }
            if (!arr.isHorizontal && frontier[i]->direction == DOWN)   //same idea here, just vertically
            {
                position.y++;
            }

            Wall **neighbors = getNeighbourWalls(maze, size, position);     

            Wall *right = neighbors[0];
            Wall *left = neighbors[1];
            Wall *down = neighbors[2];
            Wall *up = neighbors[3];

            int count4 = 0;
            for (int j = 0; j < 4; j++)    //We are counting all 4 neighbor walls to check how many are open and closed
            {
                if (neighbors[j] != NULL && neighbors[j]->type == AIR)
                {
                    count4++;
                }
            }

            if (count4 <= 1)           //if its 1 or under its a deadend
            {
                branchPotential = 0;
            }
            else
            {
                branchPotential = 1;
            }

            free(neighbors);

        

        int straightnesscount = 0;            //Calculate Straightness potential


            ArrIndexResult arr = getArrayIndex(maze, frontier[i]);

            Point position = indexToPos(arr.isHorizontal, arr.index, size);

            bool keepGoing = true;


            while (keepGoing)
            {
                Wall **neighbors = getNeighbourWalls(maze, size, position);

                Wall *right = neighbors[0];
                Wall *left = neighbors[1];
                Wall *down = neighbors[2];
                Wall *up = neighbors[3];

                if (arr.isHorizontal && frontier[i]->direction == LEFT)    // four copies of the same function, I know its inefficient but dont touch.
                {
                    if (right != NULL && right->type == AIR)     //we are checking if the next step is open, if yes, we keep going and counting
                    {
                        position.x++;
                        straightnesscount++;
                    }
                    else
                    {
                        keepGoing = false;        //once we hit a wall we stop
                    }
                }

                if (arr.isHorizontal && frontier[i]->direction == RIGHT)
                {
                    if (left != NULL && left->type == AIR)
                    {
                        position.x--;
                        straightnesscount++;
                    }
                    else
                    {
                        keepGoing = false;
                    }
                }

                if (!arr.isHorizontal && frontier[i]->direction == UP)
                {
                    if (down != NULL && down->type == AIR)
                    {
                        position.y++;
                        straightnesscount++;
                    }
                    else
                    {
                        keepGoing = false;
                    }
                }

                if (!arr.isHorizontal && frontier[i]->direction == DOWN)
                {
                    if (up != NULL && up->type == AIR)
                    {
                        position.y--;
                        straightnesscount++;
                    }
                    else
                    {
                        keepGoing = false;
                    }
                }

                free(neighbors);
            }

            straightnesscount += 1;    // same issue with max_len, we are counting the walls, not the cells, so we have to +1
                if (max_len == 0)
            {
             straightnessPotential = 0;
            }
            else
            {
                int true_max = max_len + 1;
                straightnessPotential = (float)straightnesscount / true_max;
            }

        // our weight function = e^((straight_priority - 0.5) * (straight_potential - 0.5)) * e^((branch_priority - 0.5) * (branch_potential - 0.5))
        FrontierWeight[i] = exp((straightnessPriority-0.5)*(straightnessPotential-0.5)) * exp((BranchPriority-0.5)*(branchPotential-0.5));
    
    }

    return FrontierWeight;
}