//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

#include <math.h>

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
	if (posSteps[currentPosX][currentPosY] >= stepCount) {
		return;
	}
	posSteps[currentPosX][currentPosY] = stepCount;
	int neighborWallIndex;
	LoadNeighbourWallPointers(*maze, (Point){currentPosX, currentPosY}, wallIndices);

	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY-1}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		mazeStepper(currentPosX, currentPosY-1, stepCount+1, maze);
	}
	neighborWallIndex = GetRightWallIndex((Point){currentPosX-1, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		mazeStepper(currentPosX-1, currentPosY, stepCount+1, maze);
	}
	neighborWallIndex = GetLowerWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->verticalWalls[neighborWallIndex].type == AIR) {
		mazeStepper(currentPosX, currentPosY+1, stepCount+1, maze);
	}
	neighborWallIndex = GetRightWallIndex((Point){currentPosX, currentPosY}, maze->size);
	if (neighborWallIndex != -1 && maze->horizontalWalls[neighborWallIndex].type == AIR) {
		mazeStepper(currentPosX+1, currentPosY, stepCount+1, maze);
	}
}

void checkHeat(int id) {
	printf("\nCreating heatmap variables, and allocating memory for 2D array: ");
	Maze *maze = LoadMaze(id);
	if (maze == NULL) {
		printf("\nSomething Went Wrong, no maze loaded");
	}
	printf(".");
	arraySize = maze->size.x * maze->size.y;
	printf("\ncreating array with size %d x %d", (int)maze->size.x, (int)maze->size.y);
	posSteps = (int **)calloc((int)maze->size.x, sizeof(int*));
	printf(".");
	if (!posSteps) {
		printf("Unable to allocate memory for 2D array. Heatmap creation aborted.");
		return;
	}
	printf("Memory allocated for 1D Array");
	printf("\nAllocating memory for Array of Arrays: ");
	for (int i = 0; i < maze->size.x; i++) {
		posSteps[i] = (int*)calloc((int)maze->size.y, sizeof(int));
		if (!posSteps[i]) {
			printf("Unable to allocate memory for array column %d in 2D array. Heatmap creation aborted.", i);
		} else {
			memset(posSteps[i], -1, sizeof(*posSteps[i]));
		}
	}
	printf("Memory allocated for 2D Array");

	int startPosX = maze->root.x,
		startPosY = maze->root.y,
		stepCount = 0;

	printf("\nRunning mazeStepper recursive function: ");
	mazeStepper(startPosX, startPosY, stepCount, maze);
	printf("mazeStepper finished successfully");

	// int k = 0;
	// for (int i = 0; i < (int)maze.wallCount.vertical; i++) {
	// 	for (int j = 0; j < (int)maze.wallCount.horizontal; j++) {
	// 		mazeSpots[k].posX = j;
	// 		mazeSpots[k].posY = i;
	// 		mazeSpots[k].heat = ceil(((abs(j-(maze.wallCount.horizontal/2))+abs(i-(maze.wallCount.vertical/2)))/(maze.wallCount.horizontal/2+maze.wallCount.vertical))*6);
	// 		k++;
	// 	}
	// }
	// printHeatmap(mazeSpots);
	// free(mazeSpots);
	farthestSpot = 0;
	heatIndex = calloc(arraySize, sizeof(SpotHeat));
	int k = 0;

	printf("\nChecking farthest spot distance: ");
	for (int currentX = 0; currentX < maze->size.x; currentX++) {
		for (int currentY = 0; currentY < maze->size.y; currentY++) {
			if (posSteps[currentX][currentY] > farthestSpot) farthestSpot = posSteps[currentX][currentY];
		}
	}
	printf("Farthest spot found with distance %d", farthestSpot);
	printf("\nCreating heatIndex and calculating heat for spots: ");
	for (int currentX = 0; currentX < maze->size.x; currentX++) {
		for (int currentY = 0; currentY < maze->size.y; currentY++) {
			heatIndex[k].posX = currentX;
			heatIndex[k].posY = currentY;
			heatIndex[k].heat = ceil(posSteps[currentX][currentY]/farthestSpot);
			k++;
		}
	}
	printf("heatIndex created");

	printf("\nPrinting heatmap to .txt: ");
	printHeatmap(heatIndex);
	printf("complete");

	printf("\nFreeing allocated memory:");
	free(heatIndex);
	printf("\nheatIndex freed");
	for (int i = 0; i < maze->size.x; i++) {
		free(posSteps[i]);
	}
	printf("\ninternal arrays of posSteps freed");
	free(posSteps);
	printf("\nexternal array of posSteps freed");
	FreeMaze(maze);
	printf("\nmaze freed");
}

void printHeatmap(SpotHeat heatArray[]) {
	FILE *heatmapFile;
	heatmapFile = fopen("heatmap.txt", "w");

	fprintf(heatmapFile, "Heatmap Array for each maze spot:\n");
	for (int i = 0; i < 3; i++) {
		fprintf(heatmapFile, "X coordinate: %d, Y coordinate: %d, Heat Index: ", heatArray[i].posX, heatArray[i].posY);
		switch (heatArray[i].heat) {
			case WARMEST:
				fprintf(heatmapFile, "Warmest");
				break;
			case WARMER:
				fprintf(heatmapFile, "Warmer");
				break;
			case WARM:
				fprintf(heatmapFile, "Warm");
				break;
			case NEUTRAL:
				fprintf(heatmapFile, "Neutral");
				break;
			case COLD:
				fprintf(heatmapFile, "Cold");
				break;
			case COLDER:
				fprintf(heatmapFile, "Colder");
				break;
			case COLDEST:
				fprintf(heatmapFile, "Coldest");
				break;
		}
		fprintf(heatmapFile, "\n");
	}
	fclose(heatmapFile);
}