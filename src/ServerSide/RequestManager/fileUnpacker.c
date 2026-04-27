#include "fileUnpacker.h"

void unpackFile(bool type, int id, void* data)
{
    char filename[50];
    sprintf(filename, "%d.json", id);

    FILE *file = fopen(filename, "r");

    printf("Looking for file: %s\n", filename);

    if (file == NULL) {
        printf("Error opening file - Data corrupt or does not exist\n");
        free(data);
        data = NULL;
        return;
    }

    if (!type) // generationData
    {
        generationData* data = (generationData*)data;

        fscanf(file,
        "{ \"SessionID\": %d, \"door\": %lf, \"x_size\": %d, \"y_size\": %d, \"branches\": %lf, \"loops\": %lf, \"straightness\": %lf }",
            &data->id,
            &data->door,
            &data->x_size,
            &data->y_size,
            &data->branches,
            &data->loops,
            &data->straightness
            );
    } 
    else { //alterationData
        alterationData* data = (alterationData*)data;

        // fscanf(file,
        // "{ \"SessionID\": %d, \"door\": %lf, \"x_size\": %d, \"y_size\": %d, \"branches\": %lf, \"loops\": %lf, \"straightness\": %lf }",
        //     &data->id,
        //     &data->door,
        //     &data->x_size,
        //     &data->y_size,
        //     &data->branches,
        //     &data->loops,
        //     &data->straightness
        //     );
    }
    fclose(file);
}
