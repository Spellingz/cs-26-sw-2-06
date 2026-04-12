#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// #include "generateMaze.h"
// #include "requestManger.h"
#pragma pack(1)

bool _isActive = true;

typedef struct Data Data;
typedef struct ExportData ExportData;

int main(void)
{

int size = 0;
    int queue[1000];


	while (_isActive)
	{
		// Is there any request?
		bool _isRequest = checkRequests(size);

		// No? go back
		if (!_isRequest) continue;

		// Get the request and save it into 'data'
		Data data = getTopRequest(&size, queue);

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