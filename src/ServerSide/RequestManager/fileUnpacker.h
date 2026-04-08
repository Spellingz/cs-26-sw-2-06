#ifndef FILEUNPACKER_H
#define FILEUNPACKER_H

typedef struct Data {
    int id;
    double doors;
    int x_size;
    int y_size;
    double branches;
    double loops;
    double straightness;
}Data;


Data unpackFile(int id);

#endif