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
    "{ \"SessionID\": %d, \"Door\": %lf, \"Size_Width\": %d, \"Size_Height\": %d, \"Branches\": %lf, \"Loops\": %lf, \"Straightness\": %lf }",
    &data.id,
    &data.Door,
    &data.Size_Width,
    &data.Size_Height,
    &data.Branches,
    &data.Loops,
    &data.Straightness
);

    fclose(file);




    return data;
}
