//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

int arraySize;

/*
find alle felter som er main path ved at kigge på omkringliggende vægge

for alle felter der ikke er main, find væg med korteste path til main path

tjek om korteste path til main er den længste set indtil videre, og gem længden hvis ja
*/



void checkHeat(MazeStruct maze) {
	arraySize = maze.wallCount.horizontal*maze.wallCount.vertical;
	SpotHeat *mazeSpots = calloc(arraySize, sizeof(SpotHeat));
	int k = 0;
	for (int i = 0; i < (int)maze.wallCount.vertical; i++) {
		for (int j = 0; j < (int)maze.wallCount.horizontal; j++) {
			mazeSpots[k].posX = j;
			mazeSpots[k].posY = i;
			mazeSpots[k].heat = ceil(((abs(j-(maze.wallCount.horizontal/2))+abs(i-(maze.wallCount.vertical/2)))/((maze.wallCount.horizontal/2)+(maze.wallCount.vertical)))*6);
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