//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

int arraySize;
int **posSteps;
Wall *wallIndices[4];

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
	Maze *maze = LoadMaze(id);
	arraySize = maze->wallCount.horizontal*maze->wallCount.vertical;
	posSteps = (int**)calloc(maze->wallCount.horizontal, sizeof(int*));
	if (!posSteps) {
		printf("\nUnable to allocate memory for 2D array. Heatmap creation aborted.");
		return;
	}
	for (int i = 0; i < maze->wallCount.horizontal; i++) {
		posSteps[i] = (int*)calloc(maze->wallCount.vertical, sizeof(int));
		if (!posSteps[i]) {
			printf("\nUnable to allocate memory for array column %d in 2D array. Heatmap creation aborted.", i);
		} else {
			memset(posSteps[i], -1, sizeof(*posSteps[i]));
		}
	}



	SpotHeat mazeSpots = calloc(arraySize, sizeof(SpotHeat));
	int startPosX = maze->root.x,
		startPosY = maze->root.y,
		stepCount = 0;

	mazeStepper(startPosX, startPosY, stepCount, maze);

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
	int farthestSpot = 0;

	for (int posX = 0; posX < maze->wallCount.horizontal; posX++) {
		for (int posY = 0; posY < maze->wallCount.vertical; posY++) {
			if (posSteps[posX][posY] > farthestSpot) farthestSpot = posSteps[posX][posY];
		}
	}

	for (int i = 0; i < maze->wallCount.horizontal; i++) {
		free(posSteps[i]);
	}
	free(posSteps);
	FreeMaze(maze);
}

void printHeatmap(SpotHeat heatArray[]) {
	FILE *heatmapFile;
	heatmapFile = fopen("heatmap.txt", "w");

	fprintf(heatmapFile, "Heatmap Array for each maze spot:\n");
	for (int i = 0; i < arraySize; i++) {
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