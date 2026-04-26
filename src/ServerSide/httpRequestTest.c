#include <stdio.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

enum verbosity {
    NONE,
    MINIMAL,
    WARNINGS,
    ALL
};
#define VERBOSITY ALL

#define SUPPORTED_FILE_TYPE_COUNT 4
const char* SUPPORTED_FILE_TYPES[] = {
    ".html",
    ".js",
    ".css",
    ".jpg"
};

#define PORT 8888
#define MAX_ANSWER_SIZE 512

struct connection_info_struct
{
  int connectiontype;
  char *jsonData;
  struct MHD_PostProcessor *postprocessor;
};

enum connectionType {
    POST,
    PUT,
    GET,
    //DELETE,
};

struct keyStruct {
    int diff, keyIndex;
};

typedef struct {
    char* contentType;
    char* cacheControl;
} headersStruct;

///////////////
// FILE SIZE///
///////////////

int stringEndsWith(const char* str, const char* end) {
    if(strlen(str) >= strlen(end))
    {
        const char* strEnd = str + strlen(str);
        return !strcmp(strEnd - strlen(end), end);
    }
    return 0;
}

int isSupportedFileType(const char* str) {
    for (int i = 0; i < SUPPORTED_FILE_TYPE_COUNT; i++) {
        if (stringEndsWith(str, SUPPORTED_FILE_TYPES[i])) return 1;
    }
    if (VERBOSITY >= WARNINGS) printf("Unsupported file format for %s\n", str);
    return 0;
}

int sizeOfFile(FILE* f) {
    fseek (f, 0L, SEEK_SET);
    int count = 0;
    while (getc(f) != EOF) {
        count++;
    }
    return count;
}

void readTextFile(FILE* f, char* str) {
    fseek (f, 0L, SEEK_SET);
    int c, i = 0;
    while ((c = getc(f)) != EOF) {
        //Makes sure that all \n are preceded by \r
        if (c == '\n' && str[i - 1] != '\r') str[i++] = '\r';
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


////////////////////
// CHECK DATA KEY///
////////////////////

int checkKey(const char* key, char** values)
{
    int counter = 0;
    int diff = 1;
    while(values[counter] != NULL && diff != 0)
    {
        diff = strcmp(key, values[counter++]);
    }
    if (diff != 0) return -1;
    return counter;
}


///////////////////
// ITERATE POST ///
///////////////////

static enum MHD_Result process_post (void *coninfo_cls,
                enum MHD_ValueKind kind, const char *key,
                const char *filename, const char *content_type,
                const char *transfer_encoding, const char *data,
                uint64_t off, size_t dataSize)
{
    enum KeyIndexDatas {
        Name,
        Test2,
    };

    if(VERBOSITY == ALL) printf("    Iterating Post of size %lu -> Key: %s, Data: %s\n", dataSize, key, data);
    struct connection_info_struct *con_info = coninfo_cls;

    // RETURN IF KEY DOESN'T MATCH PREFERENCE
    char *dataArray[3] = {"name", "test2", NULL};
    int keyIndex = checkKey(key, dataArray);
    if (keyIndex == -1)
        return MHD_YES;

    // CONTINUOUSLY ADD CORRECT KEY DATAVALUES INTO jsonData IF CORRECT SIZE
    if ((dataSize > 0) && (dataSize <= 20))
    {
        char _addString[strlen(key) + 10 + dataSize];
        sprintf(_addString, "\"%s\": %s, ", key, data);
        strcat(con_info->jsonData, _addString); // "Key: Data"
    } else 
    {
        if(VERBOSITY >= WARNINGS) printf("dataSize TOO BIG!!");
        return MHD_NO;
    }

    return MHD_YES;
}


//////////////////////////////////
// COMPLETED REQUEST - CLEANUP ///
//////////////////////////////////

void request_completed (void *cls, struct MHD_Connection *connection, 
                        void **req_cls, enum MHD_RequestTerminationCode toe)
{
    struct connection_info_struct *con_info = *req_cls;

    // IF THE REQUEST DOESN'T EXIST/HAVE CONTENT
    if (con_info == NULL)
        return;

    if (con_info->connectiontype == POST)
    {
        MHD_destroy_post_processor(con_info->postprocessor);
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
    struct connection_info_struct *con_info = *req_cls;

    // // IF jsonData HASN'T BEEN INITIALIZED, INITIALIZE IT
    // if (con_info->jsonData == NULL)
    // {
    // }

    // IF FIRST CONNECTION ITERATION
    if (con_info == NULL)
    {
        con_info = malloc(sizeof(struct connection_info_struct));
        *req_cls = (void*) con_info; //SET CONNECTION_INFO SO IT CAN BE ITERATED OVER 

        if (con_info == NULL) // IF CON_INFO FAILED TO INITIATE
        {
            if(VERBOSITY >= WARNINGS) printf("Didn't malloc con_info\n");
            return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
        }

        if (strcmp(method, "POST") == 0) {
            con_info->jsonData = malloc(MAX_ANSWER_SIZE);
            if (con_info->jsonData == NULL) // FALLBACK IF FAILED TO MALLOC
                 return respond_error(con, MHD_HTTP_INSUFFICIENT_STORAGE);
            con_info->jsonData[0] = '{';
            con_info->jsonData[1] = '\0';
            con_info->postprocessor = MHD_create_post_processor(con, 
                                            512, process_post, (void*) con_info);

            if (con_info->postprocessor == NULL) {
                if(VERBOSITY >= WARNINGS) printf("postProcessor failed to create\n");
                free(con_info);
                return respond_error(con, MHD_HTTP_INTERNAL_SERVER_ERROR);
            }
            con_info->connectiontype = POST;
            return MHD_YES;
        }
        else if (strcmp(method, "GET") == 0) {
            con_info->connectiontype = GET;
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
    if (con_info->connectiontype == GET)
    {
        char* requestURL;
    
        // IF NO PAGE -> DEFAULT TO FRONTPAGE
        if (strcmp(url, "/") == 0)
            requestURL = "/Frontpage.html";
        else if (!isSupportedFileType(url))
            return respond_error(con, MHD_HTTP_NOT_FOUND);
        else 
            requestURL = (char*) url;
    
        // LOOK FOR PAGE WITH SAID NAME
        char fileName[100];
        sprintf(fileName, "../src/ClientSide/Design%s", requestURL); // STARTS FROM '${PROJECT_DIR}/build'
        if(VERBOSITY == ALL) printf("fileName: %s\n", fileName);
    
        // COPY PAGE FROM SERVER, TO SEND TO CLIENT
        if (stringEndsWith(requestURL, ".html") ||
            stringEndsWith(requestURL, ".js") ||
            stringEndsWith(requestURL, ".css")) {

            char *page = NULL;
            int length;
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

            readTextFile(f, page);
            fclose (f);
            //MAKE HEADERS FOR RESPONSE
            headersStruct headers = {
                .contentType = stringEndsWith(requestURL, ".html") ? "text/html" :
                               stringEndsWith(requestURL, ".css")  ? "text/css" :
                                                                     "text/javascript",
                .cacheControl = "no-cache" //should probably be changed to maxage=x later
            };

            // MAKE RESPONSE TO SEND TO CLIENT
            return respond(con, page, strlen(page), headers, MHD_RESPMEM_MUST_FREE);
        }
        else if (stringEndsWith(requestURL, ".jpg")) {
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
                .contentType = CFSTR_MIME_JPEG,
                .cacheControl = "max-age=3600"
            };

            // MAKE RESPONSE TO SEND TO CLIENT
            return respond(con, buffer, length, headers, MHD_RESPMEM_MUST_FREE);
        }
    }


    //
    // CONNECTIONTYPE -- POST
    //
    if (con_info->connectiontype == POST)
    {
        // IF UPLOAD DATA HASN'T ALL BEEN PROCESSED
        if (*upload_data_size != 0)
        {
            if(VERBOSITY == ALL) printf("Posting Something::::\n    Upload Data: %s, SIZE: %lu\n", upload_data, *upload_data_size);

            int postProcessType = MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
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

            headersStruct headers = {
                .contentType = "application/json",
                .cacheControl = "no-store"
            };

            return respond(con, con_info->jsonData, strlen(con_info->jsonData), headers, MHD_RESPMEM_MUST_FREE);
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
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL,
    NULL, &answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    if(VERBOSITY >= MINIMAL) printf("Server running at http://localhost:%d", PORT);
    (void) getchar ();
    MHD_stop_daemon (daemon);

    return 0;
}