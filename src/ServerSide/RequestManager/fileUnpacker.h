#ifndef FILEUNPACKER_H
#define FILEUNPACKER_H

typedef struct Data {
    int id;
    double Door;
    int Size_Width;
    int Size_Height;
    double Branches;
    double Loops;
    double Straightness;
}Data;

Data unpackFile(int id);

#endif