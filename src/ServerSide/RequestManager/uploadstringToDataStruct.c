#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Data {
    int id;
    double door;
    int x_size;
    int y_size;
    double branches;
    double loops;
    double straightness;
}Data;


Data saveUploadstringAsData(char *uploadstring)
{
    Data uploadstringAsData;

    sscanf(uploadstring, "%d|%lf|%d|%d|%lf|%lf|%lf", uploadstringAsData.id, uploadstringAsData.door, 
        uploadstringAsData.x_size, uploadstringAsData.y_size, uploadstringAsData.branches, 
        uploadstringAsData.loops, uploadstringAsData.straightness);

    FILE *f = fopen(uploadstringAsdata.id, "w");
    if (f == NULL)
    {
        printf("ERROR MAKING FILE FOR SAVING UPLOADSTRING");
        return uploadstringAsData;
    }

    fprintf(f, "{\n  \"SessionID\": %d,\n    \"door\": %lf,\n    \"x_size\": %d,\n    \"y_size\": %d,
        \n  \"branches\": %lf,\n  \"loops\": %lf,\n  \"straightness\": %lf\n}", uploadstringAsData.id, 
        uploadstringAsData.door, uploadstringAsData.x_size, uploadstringAsData.y_size,
        uploadstringAsData.branches, uploadstringAsData.loops, uploadstringAsData.straightness);
    fclose(f);

    return uploadstringAsData;
}