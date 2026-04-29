//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

int arraySize;

void checkHeat(MazeStruct maze) {
	arraySize = maze.wallCount.horizontal*maze.wallCount.vertical;
	SpotHeat *mazeSpots = calloc(arraySize, sizeof(SpotHeat));
	int k = 0;
	for (int i = 0; i < (int)maze.wallCount.vertical; i++) {
		for (int j = 0; j < (int)maze.wallCount.horizontal; j++) {
			mazeSpots[k].posX = j;
			mazeSpots[k].posY = i;
			mazeSpots[k].heat = ceil(((abs(j-floor(maze.wallCount.horizontal/2))+abs(i-floor(maze.wallCount.vertical/2)))/(floor(maze.wallCount.horizontal/2)+floor(maze.wallCount.vertical)))*6);
			if (i == 19 && j == 19) {
				printf("\n\nMax width divided by two: %ld", maze.wallCount.horizontal/2);
				printf("\nRounded down: %f", floor(maze.wallCount.horizontal/2));
				printf("\nSubtract horizontal position: %f", j-floor(maze.wallCount.horizontal/2));
				printf("\nAbsolute value: %d", abs(j-floor(maze.wallCount.horizontal/2)));
				printf("\n\nMax height divided by two: %ld", maze.wallCount.vertical/2);
				printf("\nRounded down: %f", floor(maze.wallCount.vertical/2));
				printf("\nSubtract vertical position: %f", i-floor(maze.wallCount.vertical/2));
				printf("\nAbsolute value: %d", abs(i-floor(maze.wallCount.vertical/2)));
				
				printf("\n\nRounded up value of %f", ((abs(j-floor(maze.wallCount.horizontal/2))+abs(i-floor(maze.wallCount.vertical/2)))/(floor(maze.wallCount.horizontal/2)+floor(maze.wallCount.vertical)))*6);
				printf("\nResulting in heat index %d", mazeSpots[k].heat);
			}
			k++;
		}
	}
	printHeatmap(mazeSpots);
	free(mazeSpots);
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