#ifndef UPLOADSTRINGTODATASTRUCT_H
#define UPLOADSTRINGTODATASTRUCT_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../DataTypes/requestDataTypes.h"

void* transformRequest(char *uploadstring, bool type);

#endif