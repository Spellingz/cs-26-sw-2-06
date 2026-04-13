#include <stdio.h>
#include <stdlib.h>
#include <fileUnpacker.h>




Data unpackFile(int id)
{
    Data data;

    char filename[50];
    sprintf(filename, "%d.json", id);

    FILE *file = fopen(filename, "r");

    printf("Looking for file: %s\n", filename);

    if (file == NULL) {
        printf("Error opening file\n");
        data.id = -1;
        return data;
    }

    fscanf(file,
    "{ \"SessionID\": %d, \"door\": %lf, \"x_size\": %d, \"y_size\": %d, \"branches\": %lf, \"loops\": %lf, \"straightness\": %lf }",
    &data.id,
    &data.door,
    &data.x_size,
    &data.y_size,
    &data.branches,
    &data.loops,
    &data.straightness
);

    fclose(file);




    return data;
}
