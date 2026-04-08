#include <stdio.h>
#include <stdlib.h>


typedef struct Data {
    int id;
    char text[1000];
    double doors;
    int x_size;
    int y_size;
    double branches;
    double loops;
    double straightness;
}Data;

Data unpackFile(int id)
{
    Data data;

    char filename[50];
    sprintf(filename, "%d.json", id);

    FILE *file = fopen(filename, "r");

    printf("Looking for file: %s\n", filename);

    if (file == NULL) {
        printf("Error opening file\n");
    }

    fread(data.text, sizeof(char), sizeof(data.text), file);
    fclose(file);


    sscanf(data.text,
        "{ \"Door\": %lf, \"x_size\": %d, \"y_size\": %d, \"branches\": %lf, \"loops\": %lf, \"straightness\": %lf }",
        &data.doors,
        &data.x_size,
        &data.y_size,
        &data.branches,
        &data.loops,
        &data.straightness
    );


    return data;
}
