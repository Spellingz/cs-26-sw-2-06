#ifndef UPLOADSTRINGTODATASTRUCT_H
#define UPLOADSTRINGTODATASTRUCT_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../DataTypes/requestDataTypes.h"

GenerationData TransformGenerationRequest(char *uploadString);
AlterationData TransformAlterationRequest(char *uploadString);

#endif