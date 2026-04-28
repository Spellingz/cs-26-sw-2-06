#include "uploadstringToDataStruct.h"

void* transformRequest(char *uploadstring, bool type)
{
    if (type == 0)
    {
        generationData *request = malloc(sizeof(generationData));
        // sscanf(uploadstring, "%d|%lf|%d|%d|%lf|%lf|%lf", &request->id, &request->door, &request->x_size, &request->y_size, &request->branches, &request->loops, &request->straightness);
        sscanf(uploadstring, "{(id, %d), (door, %lf), (x_size, %d), (y_size, %d), (branches, %lf), (loops, %lf), (straightness, %lf), }", &request->id, &request->door, &request->x_size, &request->y_size, &request->branches, &request->loops, &request->straightness);
        // printf("scanf: %d\n", sscanf(uploadstring, "{(id, %d), (door, %lf), (x_size, %d), (y_size, %d), (branches, %lf), (loops, %lf), (straightness, %lf), }", &request->id, &request->door, &request->x_size, &request->y_size, &request->branches, &request->loops, &request->straightness));
        return request;
    }
    else {
        alterationData *request = malloc(sizeof(alterationData));
        sscanf(uploadstring, "(id, %d), (isHorizontal, %d), (wallIndex, %d), (alterationType, %d), ", &request->id, (int*)&request->isHorizontal, &request->wallIndex, &request->alterationType);
        return request;
    }

    // char nameOfFile[16];
    // sprintf(nameOfFile, "%d->json", request->id);

    // FILE *f = fopen(nameOfFile, "w");
    // if (f == NULL)
    // {
    //     printf("ERROR MAKING FILE FOR SAVING UPLOADSTRING");
    //     return request;
    // }

    // fprintf(f, "{\n  \"SessionID\": %d,\n    \"door\": %lf,\n    \"x_size\": %d,\n    \"y_size\": %d,\n  \"branches\": %lf,\n  \"loops\": %lf,\n  \"straightness\": %lf\n}", request->id, 
    //     request->door, request->x_size, request->y_size,
    //     request->branches, request->loops, request->straightness);
    // fclose(f);

    return NULL;
}