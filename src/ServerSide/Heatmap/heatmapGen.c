//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

#include <math.h>
#include <unistd.h>

int arraySize;
int **posSteps;
Wall *wallIndices[4];
SpotHeat *heatIndex;
int farthestSpot;

/*
find alle felter som er main path ved at kigge på omkringliggende vægge

for alle felter der ikke er main, find væg med korteste path til main path

tjek om korteste path til main er den længste set indtil videre, og gem længden hvis ja


tjek felter ved at starte i root og gå i alle mulige retninger med recursive function
tæl antal skridt fra mainpath/root, som gives videre når recursion sker
når et felt rammes, tjekker om det er nået før
	hvis nej gem antal skridt fra main i felt globalt
	hvis ja tjek om antal skridt er højere eller lavere end eget
		hvis nej gå videre
		hvis ja stop med at gå
*/

void mazeStepper(int currentPosX, int currentPosY, int stepCount, Maze *maze) {
	// printf("\nmazeStepper placed");
	// printf("\ncurrent steps taken: %d", stepCount);
	// printf("\nsteps recorded on current spot: %d", posSteps[currentPosX][currentPosY]);
	if (posSteps[currentPosX][currentPosY] <= stepCount && posSteps[currentPosX][currentPosY] > -1) {
		// printf("\nspot already reached, ending mazeStepper");
		return;
	}
	//sleep(1);
	int neighborWallIndex;
	LoadNeighbourWallPointers(*maze, (Point){currentPosX, currentPosY}, wallIndices);

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY-1}, maze->size);
	if (maze->verticalWalls[neighborWallIndex].isSolution) {
		stepCount = 0;
	}

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX-1, currentPosY}, maze->size);
	if (maze->horizontalWalls[neighborWallIndex].isSolution) {
		stepCount = 0;
	}

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (maze->verticalWalls[neighborWallIndex].isSolution) {
		stepCount = 0;
	}

	if (maze->horizontalWalls[neighborWallIndex].isSolution) {
		stepCount = 0;
	}


	posSteps[currentPosX][currentPosY] = stepCount;
	// printf("\nSetting position x = %d,y = %d to %d",currentPosX,currentPosY,stepCount);
	// printf("\nSteps of current position is now %d", posSteps[currentPosX][currentPosY]);



	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY-1}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper up");
		mazeStepper(currentPosX, currentPosY-1, stepCount+1, maze);
	}

	neighborWallIndex = GetRightWallIndex((Point){currentPosX-1, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper left");
		mazeStepper(currentPosX-1, currentPosY, stepCount+1, maze);
	}

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper down");
		mazeStepper(currentPosX, currentPosY+1, stepCount+1, maze);
	}

	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper right");
		mazeStepper(currentPosX+1, currentPosY, stepCount+1, maze);
	}
}

void checkHeat(int id) {
	// printf("\nCreating heatmap variables, and allocating memory for 2D array: ");
	Maze *maze = LoadMaze(id);
	if (maze == NULL) {
		// printf("\nSomething Went Wrong, no maze loaded");
		return;
	}
	// printf(".");
	arraySize = maze->size.x * maze->size.y;

	// printf("\ncreating array with size %d x %d: ", (int)maze->size.x, (int)maze->size.y);
	posSteps = (int **)calloc((int)maze->size.x, sizeof(int*));
	printf(".");
	if (!posSteps) {
		// printf("Unable to allocate memory for 2D array. Heatmap creation aborted.");
		return;
	}
	// printf("Memory allocated for 1D Array");
	// printf("\nAllocating memory for Array of Arrays: ");
	for (int i = 0; i < maze->size.x; i++) {
		posSteps[i] = (int*)calloc((int)maze->size.y, sizeof(int*));
		if (!posSteps[i]) {
			// printf("Unable to allocate memory for array column %d in 2D array. Heatmap creation aborted.", i);
			return;
		}
		memset(posSteps[i], -1, maze->size.x * sizeof(*posSteps));
		// printf("\nMemory of %d array initialized as -1 for each index: ", i);
		// printf("index for x = %d, y = 0 is %d", i, posSteps[i][0]);
	}
	// printf("Memory allocated for 2D Array");

	Point root = FindRoot(*maze, maze->openings[0]);
	int startPosX = root.x,
		startPosY = root.y,
		stepCount = 0;

	// printf("\nRunning mazeStepper recursive function: ");
	mazeStepper(startPosX, startPosY, stepCount, maze);
	// printf("\nmazeStepper finished successfully");

	farthestSpot = 0;
	int k = 0;
	// printf("\nChecking farthest spot distance: ");
	for (int currentX = 0; currentX < maze->size.x; currentX++) {
		for (int currentY = 0; currentY < maze->size.y; currentY++) {
			if (posSteps[currentX][currentY] > farthestSpot) farthestSpot = posSteps[currentX][currentY];
		}
	}
	// printf("Farthest spot found with distance %d", farthestSpot);
	// printf("\nChecking for unaltered distances: ");
	for (int i = 0; i < maze->size.x; i++) {
		for (int j = 0; j < maze->size.y; j++) {
			if (posSteps[i][j] < 0) {
				// printf("unaltered distance found at x = %d, y = %d, distance is %d", i, j, posSteps[i][j]);
			}
		}
	}
	// printf("no unaltered spots found");
	// printf("\nPrinting heatmap to .txt: ");
	printHeatmap();
	// printf("complete");

	// printf("\nFreeing allocated memory:");
	for (int i = 0; i < maze->size.x; i++) {
		free(posSteps[i]);
	}
	// printf("\ninternal arrays of posSteps freed");
	free(posSteps);
	// printf("\nexternal array of posSteps freed");
	FreeMaze(maze);
	// printf("\nmaze freed");
}

void printHeatmap() {
	FILE *heatmapFile = fopen("heatmap.txt", "w");

	fprintf(heatmapFile, "Heatmap Array for each maze spot:");
	for (int checkX = 0; checkX < 10; checkX++) {
		for (int checkY = 0; checkY < 10; checkY++) {
			fprintf(heatmapFile, "\nX = %d, Y = %d, distance = %d, heatIndex = %d", checkX, checkY, posSteps[checkX][checkY], (int)ceil(((double)posSteps[checkX][checkY]/(double)farthestSpot)*255));
		}
	}
	fclose(heatmapFile);
}