#include "uploadstringToDataStruct.h"

GenerationData TransformGenerationRequest(char *uploadString) {
    GenerationData request;
    // sscanf(uploadstring, "%d|%lf|%d|%d|%lf|%lf|%lf", &request.id, &request.door, &request.x_size, &request.y_size, &request.branches, &request.loops, &request.straightness);
    sscanf(uploadString, "{(id, %d), (door, %lf), (x_size, %d), (y_size, %d), (branches, %lf), (loops, %lf), (straightness, %lf), }", &request.id, &request.door, &request.x_size, &request.y_size, &request.branches, &request.loops, &request.straightness);
    // printf("scanf: %d\n", sscanf(uploadstring, "{(id, %d), (door, %lf), (x_size, %d), (y_size, %d), (branches, %lf), (loops, %lf), (straightness, %lf), }", &request.id, &request.door, &request.x_size, &request.y_size, &request.branches, &request.loops, &request.straightness));
    return request;
}

AlterationData TransformAlterationRequest(char* uploadString) {
    AlterationData request;
    int _perfectMaze;
    int successes = sscanf(uploadString, "{(id, %d), (isHorizontal, %d), (wallIndex, %ld), (alterationType, %d), (perfectMaze, %d), }", &request.id, (int*)&request.isHorizontal, &request.wallIndex, &request.alterationType, &_perfectMaze);
    printf("Successes: %d", successes);
    request.perfectMaze = (bool)_perfectMaze;
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

    // return NULL;
// }