#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// #include "generateMaze.h"
 #include "requestManager.h"
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
		Data data = getTopRequest(queue);

		
		// make temporary data variable to recieve processed data
		ExportData dataToExport;

		// which type of data want's to be processed?
		switch(data.type)
		{
			case "generation":
				dataToExport = generateMaze(data);
				break;
			case "alteration":
				dataToExport = alterMaze(data);
				break;
			default; //Fallback
			printf("Reject Request - no type");
				break;
		}

		// Send processed data back to client
		exportData(dataToExport);
	}

	return 0;
}