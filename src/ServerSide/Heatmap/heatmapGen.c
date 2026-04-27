//
// Created by Sullz on 27/04/2026.
//

#include "heatmapGen.h"

void checkHeat(MazeStruct maze) {
	spotHeat mazeSpots[maze.wallCount.vertical*maze.wallCount.horizontal];
	int k = 0;
	for (int i = 0; i < maze.wallCount.vertical; i++) {
		for (int j = 0; i < maze.wallCount.horizontal; j++) {
			mazeSpots[k].posX = j;
			mazeSpots[k].posY = i;
			mazeSpots[k].heat = ceil((abs(i-floor(maze.wallCount.horizontal/2))+abs(j-floor(maze.wallCount.vertical/2)))/((floor(maze.wallCount.horizontal/2)+floor(maze.wallCount.vertical))*6));
			k++;
		}
	}
}