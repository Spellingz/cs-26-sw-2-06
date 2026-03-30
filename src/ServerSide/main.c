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
	while (_isActive)
	{
		bool isRequest = checkRequests();
		if (!isRequest) continue;

		Data data = getTopRequest();

		ExportData dataToExport;

		switch(data.type)
		{
			case "generation":
				dataToExport = generateMaze(data);
				break;
			case "alteration":
				dataToExport = alterMaze(data);
				break;
			default;
			Printf("Reject Request - no type");
				break;
		}

		exportData(dataToExport);
	}

	return 0;
}