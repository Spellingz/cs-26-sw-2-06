#include <stdio.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#define SERVERTYPE 1
#else
#define SERVERTYPE 2
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "./MazeGeneration/generateMaze.h"
#include "./Heatmap/heatmapGen.h"

#include "DataTypes/requestDataTypes.h"
#include "RequestManager/uploadstringToDataStruct.h"
#include "MazeAlteration/changeMaze.h"
#include "MazeGeneration/generateMaze.h"

enum verbosity {
    NONE,
    MINIMAL,
    WARNINGS,
    ALL
};
#define VERBOSITY ALL

enum fileTypeEnum {
    HTML,
    JS,
    CSS,
    JPG
};

typedef struct {
    enum fileTypeEnum type;
    char* extension;
    char* contentType;
} fileTypeInfoStruct;

#define SUPPORTED_FILE_TYPE_COUNT 4
const fileTypeInfoStruct SUPPORTED_FILE_TYPES[SUPPORTED_FILE_TYPE_COUNT] = {
    {HTML, ".html", "text/html"},
    {JS, ".js", "text/javascript"},
    {CSS, ".css", "text/css"},
    {JPG, ".jpg", "image/jpeg"}
};

#define PORT 8888
#define MAX_ANSWER_SIZE 512


enum connectionType {
    POST,
    PUT,
    GET,
    //DELETE,
};

typedef struct {
    enum connectionType connectionType;
    const fileTypeInfoStruct *fileTypeInfo;
    char *jsonData;
    struct MHD_PostProcessor *postProcessor;
    bool requestType;
} connection_info_struct;

typedef struct {
    char* contentType;
    char* cacheControl;
} headersStruct;

//////////////////////
// STRING ENDS WITH //
//////////////////////

int stringEndsWith(const char* str, const char* end) {
    if(strlen(str) >= strlen(end))
    {
        const char* strEnd = str + strlen(str);
        return !strcmp(strEnd - strlen(end), end);
    }
    return 0;
}

/////////////////////////////
// FILE TYPE INFO FROM URL //
/////////////////////////////

const fileTypeInfoStruct *fileTypeInfoFromUrl (const char* url) {
    for (int i = 0; i < SUPPORTED_FILE_TYPE_COUNT; i++) {
        if (stringEndsWith(url, SUPPORTED_FILE_TYPES[i].extension)) {
            return &SUPPORTED_FILE_TYPES[i];
        }
    }
    return NULL;
}

///////////////
// FILE SIZE///
///////////////

int sizeOfFile(FILE* f) {
    if (SERVERTYPE == 2)
    {
        fseek (f, 0L, SEEK_SET);
        int count = 0;
        while (getc(f) != EOF) {
            count++;
        }
        return count + 1;
    }
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f,0,SEEK_SET);
    return length;
}


///////////////
// READ FILE //
///////////////

void readTextFile(FILE* f, char* str) {
    fseek (f, 0L, SEEK_SET);
    int c, i = 0;
    while ((c = getc(f)) != EOF) {
        //Makes sure that all \n are preceded by \r
        if (c == '\n' && (i == 0 || str[i - 1] != '\r')) {
            str[i++] = '\r';
            if (VERBOSITY == ALL) printf("Added \\r to string.\n");
        }
        str[i++] = (char) c;
    }
    str[i] = '\0';
}

void readFile(FILE* f, char* buffer) {
    fseek (f, 0L, SEEK_SET);
    int c, i = 0;
    while ((c = getc(f)) != EOF) {
        buffer[i++] = (char) c;
    }
}


///////////////
// FIND KEY ///
///////////////

int findKey(const char* key, char** keys) {
    for(int i = 0; keys[i] != NULL; i++) {
        if (strcmp(key, keys[i]) == 0)
            return i;
    }
    return -1;
}


///////////////////
// PROCESS POST ///
///////////////////

static enum MHD_Result process_post (void *coninfo_cls,
                enum MHD_ValueKind kind, const char *key,
                const char *filename, const char *content_type,
                const char *transfer_encoding, const char *data,
                uint64_t off, size_t dataSize)
{
    if(VERBOSITY == ALL) printf("    Partially processing post of size %lu -> Key: %s, Data: %s\n", dataSize, key, data);
    connection_info_struct *con_info = coninfo_cls;

    // RETURN IF KEY DOESN'T MATCH PREFERENCE
    char *keyArray[9] = {
        "type", 
        "id", 
        "door", 
        "x_size", 
        "y_size", 
        "branches", 
        "loops", 
        "straightness", 
        NULL
        };
    int keyIndex = findKey(key, keyArray);
    if (keyIndex == -1)
        return MHD_YES;

    printf("kvp: %s, %s\n", key, data);

    // CONTINUOUSLY ADD CORRECT KEY DATAVALUES INTO jsonData IF CORRECT SIZE
    if ((dataSize > 0) && (dataSize <= 20))
    {
        if (0 == strcmp(key, "type"))
        {
            con_info->requestType = data[0]-48;
            return MHD_YES;
        }
        // char _addString[strlen(key) + 10 + dataSize];
        // sprintf(_addString, "\"%s\": %s, ", key, data);
        char _addString[strlen(key)+7+dataSize];
        sprintf(_addString, "(%s, %s), ", key, data);
        strcat(con_info->jsonData, _addString); // "Key: Data"
    } else 
    {
        if(VERBOSITY >= WARNINGS) printf("dataSize TOO BIG!!");
        return MHD_NO;
    }

    return MHD_YES;
}


char *exportDataToString(ExportData data)
{
    int horizontalArrStringSize = (data.horizontalMazeArraySize*3);
    int verticalArrStringSize= (data.verticalMazeArraySize*3);


    char *responseString = malloc(sizeof(char)*(80 + 2 + horizontalArrStringSize + 2 + verticalArrStringSize + 3));
    responseString[0] = '\0';
    // LOOP FOR HORIZONTAL ARRAY
    char horizontalArrString[horizontalArrStringSize];
    horizontalArrString[0] = '\0';
    for (int i = 0; i < data.horizontalMazeArraySize-1; i++)
    {
        char _addString[4];
        sprintf(_addString, "%d, ", (int)data.horizontalMazeArr[i]);
        strcat(horizontalArrString, _addString);
    }    
    char _addString[2];
    sprintf(_addString, "%d", (int)data.horizontalMazeArr[data.horizontalMazeArraySize-1]);
    strcat(horizontalArrString, _addString);

    char verticalArrString[verticalArrStringSize];
    verticalArrString[0] = '\0';
    for (int i = 0; i < data.verticalMazeArraySize-1; i++)
    {
        char _addString[4];
        sprintf(_addString, "%d, ", (int)data.verticalMazeArr[i]);
        strcat(verticalArrString, _addString);
    }
    sprintf(_addString, "%d", (int)data.verticalMazeArr[data.verticalMazeArraySize-1]);
    strcat(verticalArrString, _addString);


    sprintf(responseString, "{\"id\": %d, \"horiArr\": [%s], \"vertArr\":[%s]}", data.id, horizontalArrString, verticalArrString);

    return responseString;
}

char* alterationExportDataToString(AlterationExportData data) {
    char* responseString = malloc(sizeof(char) * (data.wallCount * 8 + 200));
    if (!responseString) return "";
    char* _buffer = malloc(sizeof(char) * (data.wallCount * 8 + 200));
    if (!_buffer) {
        free(responseString);
        return "";
    }
    responseString[0] = _buffer[0] = '\n';

    sprintf(responseString, "{\n  \"walls\": [");

    for (int i = 0; i < data.wallCount; i++) {
        sprintf(_buffer, i != data.wallCount - 1 ? "[%d, %d], " : "[%d, %d]",
            data.walls[i].isHorizontal, data.walls[i].index);
        strcat(responseString, _buffer);
    }
    strcat(responseString,  "]\n}");
    free(_buffer);
    return responseString;
}


//////////////////////////////////
// COMPLETED REQUEST - CLEANUP ///
//////////////////////////////////

void request_completed (void *cls, struct MHD_Connection *connection, 
                        void **req_cls, enum MHD_RequestTerminationCode toe)
{
    connection_info_struct *con_info = *req_cls;

    // IF THE REQUEST DOESN'T EXIST/HAVE CONTENT
    if (con_info == NULL)
        return;

    if (con_info->connectionType == POST)
    {
        MHD_destroy_post_processor(con_info->postProcessor);
        if (con_info->jsonData) free (con_info->jsonData);
    }

    // CLEANUP
    free(con_info);
    *req_cls = NULL;
}

//////////////
// RESPOND ///
//////////////

static enum MHD_Result respond_error(struct MHD_Connection *connection, int errorCode) {
    // INITIALIZE RESPONSE AND RESPONSE_RESULT VARS
    enum MHD_Result result;
    struct MHD_Response *response;

    // MAKE RESPONSE
    response = MHD_create_response_from_buffer_static (0, NULL);
    // IF RESPONSE FAILED TO CREATE OR IS INVALID
    if (!response)
        return MHD_NO;

    result = MHD_queue_response (connection, errorCode, response);
    MHD_destroy_response (response);
    return result;
}

static enum MHD_Result respond(struct MHD_Connection *con,
                               char* buffer, size_t bufferLen,
                               headersStruct headers,
                               enum MHD_ResponseMemoryMode memoryMode)
{
    // INITIALIZE RESPONSE AND RESPONSE_RESULT VARS
    enum MHD_Result result;
    struct MHD_Response *response;

    // MAKE RESPONSE
    if (SERVERTYPE == 1)
        response = MHD_create_response_from_buffer_static(bufferLen, buffer);
    else
        response = MHD_create_response_from_buffer(bufferLen, buffer, memoryMode);
    // IF RESPONSE FAILED TO CREATE OR IS INVALID
    if (!response)
        return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
    // ADD HEADERS
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, headers.contentType);
    MHD_add_response_header(response, MHD_HTTP_HEADER_CACHE_CONTROL, headers.cacheControl);
    //SEND RESPONSE
    result = MHD_queue_response (con, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return result;
}

///////////////////////////
// MAIN CONNECTION POINT //
///////////////////////////

static enum MHD_Result answer_to_connection (void *cls,
                      struct MHD_Connection *con,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **req_cls)
{
    //
    // CONNECTIONTYPE GET: GET PAGES
    // CONNECTIONTYPE POST: SEND & RECIEVE DATA
    //


    if(VERBOSITY == ALL) printf("\nEVERY PAYLOAD: -----------------------\n");
    if(VERBOSITY == ALL) printf("URL: '%s'\n", url);
    if(VERBOSITY == ALL) printf("Method: %s \n", method);
    if(VERBOSITY == ALL) printf("UploadDataSize: %lu, UploadData: %s\n", *upload_data_size, upload_data);


    // INITIALIZE CONNECTION_INFO STRUCT
    connection_info_struct *con_info = *req_cls;

    // IF FIRST CONNECTION ITERATION
    if (con_info == NULL) {
        con_info = malloc(sizeof(connection_info_struct));
        *req_cls = (void*) con_info; //SET CONNECTION_INFO SO IT CAN BE ITERATED OVER 

        if (con_info == NULL) { // IF CON_INFO FAILED TO INITIATE
            if(VERBOSITY >= WARNINGS) printf("Didn't malloc con_info\n");
            return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
        }

        con_info->fileTypeInfo = fileTypeInfoFromUrl(url);

        if (strcmp(method, "POST") == 0) {
            con_info->jsonData = malloc(MAX_ANSWER_SIZE);
            if (con_info->jsonData == NULL) // FALLBACK IF FAILED TO MALLOC
                 return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
            con_info->jsonData[0] = '{';
            con_info->jsonData[1] = '\0';
            con_info->postProcessor = MHD_create_post_processor(con,
                                            512, process_post, (void*) con_info);

            if (con_info->postProcessor == NULL) {
                if(VERBOSITY >= WARNINGS) printf("postProcessor failed to create\n");
                free(con_info);
                return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
            }
            con_info->connectionType = POST;
            return MHD_YES;
        }
        else if (strcmp(method, "GET") == 0) {
            con_info->connectionType = GET;
            return MHD_YES;
        }
        else {
            if (VERBOSITY >= WARNINGS) printf("Unsupported fetch type: %s\n", method);
            free(con_info);
            return respond_error(con, MHD_HTTP_BAD_REQUEST);
        }
    }


    //
    // CONNECTIONTYPE GET
    //
    if (con_info->connectionType == GET) {
        char* requestURL;
    
        // IF NO PAGE -> DEFAULT TO FRONTPAGE
        if (strcmp(url, "/") == 0) {
            requestURL = "/Frontpage.html";
            con_info->fileTypeInfo = &SUPPORTED_FILE_TYPES[HTML];
        }
        // IF FILE TYPE IS UNSUPPORTED
        else if (con_info->fileTypeInfo == NULL) {
            if (VERBOSITY >= WARNINGS) printf("Unsupported file format for %s\n", url);
            return respond_error(con, MHD_HTTP_NOT_FOUND);
        }
        else 
            requestURL = (char*) url;
    
        // LOOK FOR PAGE WITH SAID NAME
        char fileName[100];
        sprintf(fileName, "../src/ClientSide/Design%s", requestURL); // STARTS FROM '${PROJECT_DIR}/build'
        if(VERBOSITY == ALL) printf("fileName: %s\n", fileName);
    
        // COPY PAGE FROM SERVER, TO SEND TO CLIENT
        if (con_info->fileTypeInfo->type == HTML ||
            con_info->fileTypeInfo->type == JS ||
            con_info->fileTypeInfo->type == CSS) {

            char *page = 0;
            long length;
            FILE * f = fopen (fileName, "rb");
            if (!f) {
                if(VERBOSITY >= WARNINGS) printf("File does not exist: %s\n", fileName);
                return respond_error(con, MHD_HTTP_NOT_FOUND);
            }

            if (VERBOSITY == ALL) printf("fileOpened\n");

            length = sizeOfFile(f);
            page = malloc (length * sizeof(char));

            if (!page) {
                fclose(f);
                if (VERBOSITY >= WARNINGS) printf("Not enough memory for response page\n");
                return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
            }

            if (SERVERTYPE == 1)
                fread(page, 1, length, f);
            else
                readTextFile(f, page);
            fclose (f);

            // struct  MHD_Response *response;
            // enum MHD_Result ret;
            // response = MHD_create_response_from_buffer_static(strlen(page),page);
            // ret = MHD_queue_response(con, MHD_HTTP_OK, response);
            // MHD_destroy_response(response);
            // return ret;

            // MAKE HEADERS FOR RESPONSE
            headersStruct headers = {
                .contentType = con_info->fileTypeInfo->contentType,
                .cacheControl = "no-cache" //should probably be changed to max-age=x later
            };

            // MAKE RESPONSE TO SEND TO CLIENT
            return respond(con, page, strlen(page), headers, MHD_RESPMEM_MUST_FREE);
        }
        else if (con_info->fileTypeInfo->type == JPG) {
            char *buffer = NULL;
            int length;
            FILE * f = fopen (fileName, "rb");
            if (!f) {
                if(VERBOSITY >= WARNINGS) printf("File does not exist: %s\n", fileName);
                return respond_error(con, MHD_HTTP_NOT_FOUND);
            }

            if (VERBOSITY == ALL) printf("image opened\n");
            length = sizeOfFile(f);
            buffer = malloc (length * sizeof(char));

            if (!buffer) {
                fclose(f);
                if (VERBOSITY == WARNINGS) printf("Not enough memory for image buffer\n");
                return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
            }

            readFile(f, buffer);
            fclose (f);

            //MAKE HEADERS
            headersStruct headers = {
                .contentType = con_info->fileTypeInfo->contentType,
                .cacheControl = "max-age=3600"
            };

            // MAKE RESPONSE TO SEND TO CLIENT
            return respond(con, buffer, length, headers, MHD_RESPMEM_MUST_FREE);
        }
    }


    //
    // CONNECTIONTYPE -- POST
    //
    if (con_info->connectionType == POST)
    {
        // IF UPLOAD DATA HASN'T ALL BEEN PROCESSED
        if (*upload_data_size != 0)
        {
            if(VERBOSITY == ALL) printf("Posting Something::::\n    Upload Data: %s, SIZE: %lu\n", upload_data, *upload_data_size);

            int postProcessType = MHD_post_process(con_info->postProcessor, upload_data, *upload_data_size);
            if (postProcessType != MHD_YES)
                return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
            *upload_data_size = 0;
            return MHD_YES;
        }
        else if (con_info->jsonData != NULL) // IF ALL UPLOAD DATA HAS BEEN PROCESSED
        {
            strcat(con_info->jsonData, "}");
            // Don't Respond - save data
            if(VERBOSITY == ALL) printf("\n\nPOST SUCCESS!\n\n");

            // Process data
            // void* request = transformRequest(con_info->jsonData, con_info->requestType);
            char* responseString;
            if (con_info->requestType == 0)     // generationData
            {
                GenerationData *request = transformRequest(con_info->jsonData, con_info->requestType);
                ExportData responseData = GenerateMaze(*request);
                responseString = exportDataToString(responseData);
            }
            else {                          // alterationData
                AlterationData *request = transformRequest(con_info->jsonData, con_info->requestType);
                AlterationExportData responseData = alterMaze(*request);
                responseString = alterationExportDataToString(responseData);
            }


            headersStruct headers = {
                .contentType = "application/json",
                .cacheControl = "no-store"
            };

            return respond(con, responseString, strlen(responseString), headers, MHD_RESPMEM_MUST_FREE);
        }
        // FALLBACK - IF ALL ELSE FAILS
        if(VERBOSITY >= WARNINGS) printf("conInfo has not allocated response string!\n");
        return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
    }

    // IF EVERYTHING FAILS - SHOULD NEVER HAPPEN
    if(VERBOSITY >= WARNINGS) printf("conInfo exists, but has no valid connectionType\n");
    return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
}

int main(void)
{
    /*
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL,
    NULL, &answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    if(VERBOSITY >= MINIMAL) printf("Server running at http://localhost:%d", PORT);
    (void) getchar ();
    MHD_stop_daemon (daemon);
    */

    Data mazeData = {
        0, // ID
        0, // door
        5, // x_size
        5, // y_size
        0, // branches
        0, // loops
        0, // straightness
    };

    generateMaze(mazeData);

    return 0;
}