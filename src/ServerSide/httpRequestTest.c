#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8888

#define MAX_ANSWER_SIZE 512 

struct connection_info_struct
{
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};

enum connectiontype {
    POST,
    PUT,
    GET,
    DELETE,
};


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
    return diff;
}


///////////////////
// ITERATE POST ///
///////////////////

static enum MHD_Result iterate_post (void *coninfo_cls, 
                enum MHD_ValueKind kind, const char *key,
                const char *filename, const char *content_type,
                const char *transfer_encoding, const char *data,
                uint64_t off, size_t dataSize)
{
    printf("    Iterating Post of size %lu -> Key: %s, Data: %s\n", dataSize, key, data);
    struct connection_info_struct *con_info = coninfo_cls;

    // RETURN IF KEY DOESN'T MATCH PREFERENCE
    if (0 != checkKey(key, (char*[]){"name", "test2", NULL}))
        return MHD_YES;

    // CONTINUOUSLY ADD CORRECT KEY DATAVALUES INTO ANSWERSTRING IF CORRECT SIZE
    if ((dataSize > 0) && (dataSize <= 20))
        strcat(con_info->answerstring, data);
    else 
    {
        printf("dataSize TOO BIG!!");
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
        if (con_info->answerstring) free (con_info->answerstring);
    }

    // CLEANUP
    free(con_info);
    *req_cls = NULL;
}

//////////////
// RESPOND ///
//////////////

static enum MHD_Result respond(struct MHD_Connection *connection, 
                               const char *page)
{
    // INITIALIZE RESPONSE AND RESPOSNE_RESULT VARS
    enum MHD_Result result;
    struct MHD_Response *response;

    // MAKE RESPONSE
    response = MHD_create_response_from_buffer_static (strlen (page),
                                                        page);
    // IF RESPONSE FAILED TO CREATE OR IS INVALID
    if (!response)
        return MHD_NO;

    result = MHD_queue_response (connection,
                            MHD_HTTP_OK,
                            response);
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


    printf("\nEVERY PAYLOAD: -----------------------\n");
    printf("URL: '%s'\n", url);
    printf("Method: %s \n", method);
    printf("UploadDataSize: %lu, UploadData: %s\n", *upload_data_size, upload_data);


    // INITIALIZE CONNECTION_INFO STRUCT
    struct connection_info_struct *con_info = *req_cls;

    // // IF ANSWERSTRING HASN'T BEEN INITIALIZED, INITIALIZE IT
    // if (con_info->answerstring == NULL)
    // {
    // }

    // IF FIRST CONNECTION ITERATION
    if (con_info == NULL)
    {
        con_info = malloc(sizeof(struct connection_info_struct));

        *req_cls = (void*) con_info; //SET CONNECTION_INFO SO IT CAN BE ITERATED OVER 

        if (con_info == NULL) // IF CON_INFO FAILED TO INITIATE
        {
            printf("didn't malloced con_info\n");
            return MHD_NO;
        }         

        // con_info->answerstring = NULL;
        con_info->answerstring = malloc(MAX_ANSWER_SIZE);
        // if (con_info->answerstring == NULL) // FALLBACK IF FAILED TO MALLOC
        //     return MHD_NO;
        con_info->answerstring[0] = '\0';

        if (strcmp(method, "POST") == 0)
        {
            con_info->postprocessor = MHD_create_post_processor(con, 
                                            512, iterate_post, (void*) con_info);

            if (con_info->postprocessor == NULL)
            {
                printf("postProcessor failed to create\n");
                free(con_info);
                return MHD_NO;
            }
            con_info->connectiontype = POST;
        }
        else con_info->connectiontype = GET;
        return MHD_YES;
    }

    //
    // CONNECTIONTYPE GET
    //

    // if (strcmp(method, "GET") == 0)
    if (con_info->connectiontype == GET)
    {
        char* requestURL;
    
        // IF NO PAGE -> DEFAULT TO FRONTPAGE
        if (strcmp(url, "/") == 0)
            requestURL = "/Frontpage.html";
        else 
            requestURL = (char*) url;
    
        // LOOK FOR PAGE WITH SAID NAME
        char fileName[100];
        sprintf(fileName, "../src/ClientSide/Design%s", requestURL); // STARTS FROM '${PROJECT_DIR}/build'
        printf("fileName: %s\n", fileName);
    
        // COPY PAGE FROM SERVER, TO SEND TO CLIENT
        char *page = 0;
        long length;
        FILE * f = fopen (fileName, "rb");
        if (f)
        {
            printf("fileOpened\n");
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            page = malloc (length);
            if (page)
                fread (page, 1, length, f);
            fclose (f);
        }
        else {
            printf("File does not exist: %s\n", fileName);
            return MHD_NO;
        }

        // MAKE RESPONSE TO SEND TO CLIENT
        struct MHD_Response *response;
        enum MHD_Result ret;

        response = MHD_create_response_from_buffer_static(strlen(page), page);
        ret = MHD_queue_response (con, MHD_HTTP_OK, response);
        MHD_destroy_response (response);
        printf("response successfuly made and destroyed");
        return ret;
    }


    //
    // CONNECTIONTYPE -- POST
    //
    if (con_info->connectiontype == POST)
    {
        // IF UPLOAD DATA HASN'T ALL BEEN PROCESSED
        if (*upload_data_size != 0)
        {
            printf("Posting Something::::\n    Upload Data: %s, SIZE: %lu\n", upload_data, *upload_data_size);

            int postProcessType = MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
            if (postProcessType != MHD_YES)
                return MHD_NO;
            *upload_data_size = 0;
            return MHD_YES;
        }
        else if (con_info->answerstring != NULL) // IF ALL UPLOAD DATA HAS BEEN PROCESSED
        {
            printf("\n\nPOST SUCCESS!\n\n");
            return respond(con, con_info->answerstring);
        }
        // FALLBACK - IF ALL ELSE FAILS
        printf("Post Failure!\n");
        return MHD_NO;
    }

    // IF EVERYTHING FAILS - SHOULD NEVER HAPPEN
    printf("\nSOMETHING HAS GONE WRONG!!\n");
    printf("SOMETHING HAS GONE WRONG!!\n");
    printf("SOMETHING HAS GONE WRONG!!\n");
    return MHD_NO;
}

int main(void)
{
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon (MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL,
    NULL, &answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    (void) getchar ();
    MHD_stop_daemon (daemon);

    return 0;
}