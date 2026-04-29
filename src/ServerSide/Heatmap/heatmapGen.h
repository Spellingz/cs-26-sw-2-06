//
// Created by Sullz on 27/04/2026.
//

#ifndef HTTP_TEST_HEATMAPGEN_H
#define HTTP_TEST_HEATMAPGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../MazeGeneration/generateMaze.h"

typedef struct {
	int posX, posY;	// X and Y coordinates of the spot
	enum {
		WARMEST,	// 0
		WARMER,		// 1
		WARM,		// 2
		NEUTRAL,	// 3
		COLD,		// 4
		COLDER,		// 5
		COLDEST,	// 6
	} heat;			// Heat index of the spot
} SpotHeat;

int generateHeatmap();

void checkHeat(MazeStruct maze);

void printHeatmap(SpotHeat heatArray[]);



#endif //HTTP_TEST_HEATMAPGEN_H
