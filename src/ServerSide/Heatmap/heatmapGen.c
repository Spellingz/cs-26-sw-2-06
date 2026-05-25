//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

#include <math.h>
#include <unistd.h>
#include <time.h>

int arraySize;
int mazeWidth, mazeHeight;
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

void mazeStepper(int currentPosX, int currentPosY, int stepCount, Maze *maze, int resetType) {
	// printf("\nmazeStepper placed");
	// printf("\ncurrent steps taken: %d", stepCount);
	// printf("\nsteps recorded on current spot: %d", posSteps[currentPosY][currentPosX]);
	if (posSteps[currentPosY][currentPosX] <= stepCount && posSteps[currentPosY][currentPosX] > -1) {
		// printf("\nspot already reached, ending mazeStepper");
		return;
	}
	//sleep(1);
	int neighborWallIndex;
	LoadNeighbourWallPointers(*maze, (Point){currentPosX, currentPosY}, wallIndices);
	// printf("Running Reset Type %d", resetType);
	if (resetType == 0) {
		neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY-1}, maze->size);
		if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].isSolution) {
			// printf("\nPosition x: %d, y: %d is part of the main path", currentPosX, currentPosY);
			stepCount = 0;
		}

		neighborWallIndex = GetRightWallIndex((Point){currentPosX-1, currentPosY}, maze->size);
		if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].isSolution) {
			// printf("\nPosition x: %d, y: %d is part of the main path", currentPosX, currentPosY);
			stepCount = 0;
		}

		neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY}, maze->size);
		if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].isSolution) {
			// printf("\nPosition x: %d, y: %d is part of the main path", currentPosX, currentPosY);
			stepCount = 0;
		}

		neighborWallIndex = GetRightWallIndex((Point){currentPosX, currentPosY}, maze->size);
		if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].isSolution) {
			// printf("\nPosition x: %d, y: %d is part of the main path", currentPosX, currentPosY);
			stepCount = 0;
		}
	} else if (resetType == 1) {
		//printf("\nChecking if spot is start or end of maze at position %d, %d: ", currentPosX, currentPosY);
		if ((currentPosX+1 == maze->size.x && currentPosY+1 == maze->size.y) || (currentPosX == 0 && currentPosY == 0)) {
			//printf("position is start or end, reset count.");
			stepCount = 0;
		}
	}

	posSteps[currentPosY][currentPosX] = stepCount;
	// printf("\nSetting position x = %d,y = %d to %d",currentPosX,currentPosY,stepCount);
	// printf("\nSteps of current position is now %d", posSteps[currentPosY][currentPosX]);



	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY-1}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper up");
		mazeStepper(currentPosX, currentPosY-1, stepCount+1, maze, resetType);
	}

	neighborWallIndex = GetRightWallIndex((Point){currentPosX-1, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper left");
		mazeStepper(currentPosX-1, currentPosY, stepCount+1, maze, resetType);
	}

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper down");
		mazeStepper(currentPosX, currentPosY+1, stepCount+1, maze, resetType);
	}

	neighborWallIndex = GetRightWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		// printf("\nsending mazeStepper right");
		mazeStepper(currentPosX+1, currentPosY, stepCount+1, maze, resetType);
	}
}

char* checkHeat(int id, int resetType, bool proxType) {
	double timer = clock();

	// printf("\nCreating heatmap variables, and allocating memory for 2D array: ");
	Maze *maze = LoadMaze(id);
	if (maze == NULL) {
		// printf("\nSomething Went Wrong, no maze loaded");
		return nullptr;
	}
	// printf(".");
	mazeHeight = maze->size.y;
	mazeWidth = maze->size.x;
	arraySize = maze->size.x * maze->size.y;

	// printf("\ncreating array with size %d x %d: ", (int)maze->size.x, (int)maze->size.y);
	posSteps = (int **)calloc((int)maze->size.y, sizeof(int*));
	// printf(".");
	if (!posSteps) {
		// printf("Unable to allocate memory for 2D array. Heatmap creation aborted.");
		return nullptr;
	}
	// printf("Memory allocated for 1D Array");
	// printf("\nAllocating memory for Array of Arrays: ");
	for (int i = 0; i < maze->size.y; i++) {
		// printf("\nAttempting to allocate memory for array column %d: ", i);
		// printf("%d", maze->size.x);
		posSteps[i] = (int*)calloc((int)maze->size.x, sizeof(int*));
		if (!posSteps[i]) {
			// printf("Unable to allocate memory for array column %d in 2D array. Heatmap creation aborted.", i);
			return nullptr;
		}
		// printf(" Column Allocated.");
		memset(posSteps[i], -1, maze->size.x * sizeof(*posSteps));
		// printf("\nMemory of %d array initialized as -1 for each index: ", i);
		// printf("index for x = %d, y = 0 is %d", i, posSteps[i][0]);
	}
	// printf("Memory allocated for 2D Array");

	Point start = maze->openings[0];
	int startPosX = start.x,
		startPosY = start.y,
		stepCount = 0;

	// printf("\nRunning mazeStepper recursive function: ");
	mazeStepper(startPosX, startPosY, stepCount, maze, resetType);
	// printf("\nmazeStepper finished successfully");

	farthestSpot = 0;
	int k = 0;
	// printf("\nChecking farthest spot distance: ");
	for (int currentY = 0; currentY < maze->size.y; currentY++) {
		for (int currentX = 0; currentX < maze->size.x; currentX++) {
			if (posSteps[currentY][currentX] > farthestSpot) farthestSpot = posSteps[currentY][currentX];
		}
	}
	// printf("Farthest spot found with distance %d", farthestSpot);
	// printf("\nChecking for unaltered distances: ");
	for (int i = 0; i < maze->size.y; i++) {
		for (int j = 0; j < maze->size.x; j++) {
			if (posSteps[i][j] < 0) {
				// printf("unaltered distance found at x = %d, y = %d, distance is %d", i, j, posSteps[i][j]);
			} else {
				if (proxType == 0) {
					posSteps[i][j] = ceil(((double)posSteps[i][j]/(double)farthestSpot)*255);
				} else if (proxType > 0) {
					posSteps[i][j] = ceil(255-(255*((double)posSteps[i][j]/(double)farthestSpot)));
				}
			}
		}
	}
	// printf("no unaltered spots found");
	// printf("\nPrinting heatmap to .txt: ");
	//printHeatmap();
	// printf("complete");

	char* returnString = exportHeatmap();

	// printf("\nFreeing allocated memory:");
	for (int i = 0; i < maze->size.y; i++) {
		free(posSteps[i]);
	}
	// printf("\ninternal arrays of posSteps freed");
	free(posSteps);
	// printf("\nexternal array of posSteps freed");
	FreeMaze(maze);
	// printf("\nmaze freed");
	double secondTimer = clock();
	// printf("\nConputation time: %lf", (secondTimer-timer)/CLOCKS_PER_SEC);
	return returnString;
}

char* exportHeatmap() {
	FILE *heatmapFile = fopen("heatmap.json", "w");
	char* heatmapData = calloc(sizeof(char), arraySize*10);
	char spotData;
	char* buffer = calloc(sizeof(char), 10);

	strcat(heatmapData, "{ ");
	strcat(heatmapData, "\n\"heatIndex\": [");
	for (int y = 0; y < mazeHeight; y++) {
		if (y != 0) {
			strcat(heatmapData, ", \n");
		}
		strcat(heatmapData, "[");
		for (int x = 0; x < mazeWidth; x++) {
			if (x != 0) {
				strcat(heatmapData, ", ");
			}
			sprintf(buffer, "%d", posSteps[y][x]);
			strcat(heatmapData, buffer);
		}
		strcat(heatmapData, "]");
	}
	strcat(heatmapData, "]\n}");
	// printf("\n%s", heatmapData);
	fprintf(heatmapFile, "%s", heatmapData);
	free(buffer);
	fclose(heatmapFile);
	return heatmapData;
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