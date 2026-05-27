#include "uploadstringToDataStruct.h"

const GenerationData INVALID_GENERATION_DATA = {
    -1, -1, -1, -1, -1, -1
};

const AlterationData INVALID_ALTERATION_DATA = {
    -1, 0, 0, -1, -1
};

bool AddIntToData(char** str, const char* key, int* valueAdr) {
    *str += strlen(key) + strlen("(, ");
    char* end;
    int value = strtol(*str, &end, 10);
    if (*str == end) {
        return false;
    }
    *valueAdr = value;
    *str = end;
    *str += strlen("), ");
    return true;
}

bool AddDoubleToData(char** str, const char* key, double* valueAdr) {
    *str += strlen(key) + strlen("(, ");
    char* end;
    double value = strtod(*str, &end);
    if (*str == end) {
        return false;
    }
    *valueAdr = value;
    *str = end;
    *str += strlen("), ");
    return true;
}

GenerationData TransformGenerationRequest(char *uploadString) {
    GenerationData request;
    uploadString += strlen("{");
    if (!AddIntToData(&uploadString, "id", &request.id))  return INVALID_GENERATION_DATA;
    if (!AddIntToData(&uploadString, "x_size", &request.x_size)) return INVALID_GENERATION_DATA;
    if (!AddIntToData(&uploadString, "y_size", &request.y_size)) return INVALID_GENERATION_DATA;
    if (!AddDoubleToData(&uploadString, "branches", &request.branches)) return INVALID_GENERATION_DATA;
    if (!AddDoubleToData(&uploadString, "loops", &request.loops)) return INVALID_GENERATION_DATA;
    if (!AddDoubleToData(&uploadString, "straightness", &request.straightness)) return INVALID_GENERATION_DATA;

    if (request.x_size < 3 || request.y_size < 3 || request.x_size > 1000 || request.y_size > 1000 ||
        request.branches < 0 || request.branches > 1 || request.loops < 0 || request.loops > 1 ||
        request.straightness < 0 || request.straightness > 1)
        return INVALID_GENERATION_DATA;

    return request;
}

AlterationData TransformAlterationRequest(char* uploadString) {
    AlterationData request;
    int isHorizontal;
    int perfectMaze;
    uploadString += strlen("{");
    if (!AddIntToData(&uploadString, "id", &request.id))  return INVALID_ALTERATION_DATA;
    if (!AddIntToData(&uploadString, "isHorizontal", &isHorizontal)) return INVALID_ALTERATION_DATA;
    if (!AddIntToData(&uploadString, "wallIndex", &request.wallIndex)) return INVALID_ALTERATION_DATA;
    if (!AddIntToData(&uploadString, "alterationType", (int*)&request.alterationType)) return INVALID_ALTERATION_DATA;
    if (!AddIntToData(&uploadString, "perfectMaze", &perfectMaze)) return INVALID_ALTERATION_DATA;
    request.isHorizontal = (bool) isHorizontal;
    request.perfectMaze = (bool) perfectMaze;

    int horizontalSize;
    int verticalSize;
    int temp;

    char fileName [30] = "";
    sprintf(fileName, "../src/ServerSide/Mazes/%d.json", request.id);
    FILE* f = fopen(fileName, "r");
    if (!f) return INVALID_ALTERATION_DATA;
    fscanf(f, "{ \"status\": %d, \"horizontalMazeArraySize\": %d, \"verticalMazeArraySize\": %d",
           &temp, &horizontalSize, &verticalSize);

    if (request.wallIndex < 0 || (request.wallIndex > horizontalSize && request.isHorizontal) || (request.wallIndex > verticalSize && !request.isHorizontal))
        return INVALID_ALTERATION_DATA;
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