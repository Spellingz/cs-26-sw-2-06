#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataTypes/requestDataTypes.h"
#include "MazeGeneration/generateMaze.h"
#include "RequestManager/requestManager.h"
#include "MazeAlteration/changeMaze.h"

#pragma pack(1)

bool _isActive = true;

typedef struct Data Data;
typedef struct ExportData ExportData;

int main(void)
{

Queue *queue = malloc(sizeof(Queue));
queue->first = NULL  ;
queue->last = NULL;

	while (_isActive)
	{

		// Is there any request?
		bool _isRequest = checkRequests(*queue);
		// No? go back
		if (!_isRequest) continue;

		// Get the request and save it into 'data'
		TopRequest request = popTopRequest(queue);

		if (request.data == NULL) continue;
		
		// make temporary data variable to recieve processed data
		ExportData dataToExport;

		// which type of data want's to be processed?
		if (!request.type) 	// generationData
			dataToExport = generateMaze(*(generationData*)request.data);
		else 				// alterationData
			dataToExport = alterMaze(*(alterationData*)request.data);

		// Send processed data back to client
		// exportData(dataToExport);
	}

	return 0;
}