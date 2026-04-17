#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 8888

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


const char* greatingpage="<html><body><h1>Welcome, %s!</center></h1></body></html>";

static enum MHD_Result
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data,
              uint64_t off, size_t size)
{
    printf("Iterating Post\n");
    printf("Key: %s, Data: %s\n",key, data);
    struct connection_info_struct *con_info = coninfo_cls;

    if (0 != strcmp (key, "name"))
        return MHD_YES;

    if ((size > 0) && (size <= 20))
    {
        char *answerstring;
        answerstring = malloc (512);
        if (!answerstring) 
            return MHD_NO;

        snprintf (answerstring, 512, greatingpage, data);
        con_info->answerstring = answerstring;
    }
    else 
    {
        con_info->answerstring = NULL;
    }

    return MHD_YES;
}

void request_completed (void *cls, struct MHD_Connection *connection, 
                        void **req_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *req_cls;

  if (NULL == con_info)
    return;
  if (con_info->connectiontype == POST)
    {
      MHD_destroy_post_processor (con_info->postprocessor);
      if (con_info->answerstring) free (con_info->answerstring);
    }

  free (con_info);
  *req_cls = NULL;
}


static enum MHD_Result 
answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **req_cls) 
{
    printf("\n\nEVERY PAYLOAD: -----------------------\n");
    printf("URL: '%s'\n", url);
    printf("Method: %s \n", method);
    printf("Version: %s\n", version);
    printf("UploadData: %s\n", upload_data);
    printf("UploadDataSize: %lu\n\n", *upload_data_size);


    struct connection_info_struct *con_info = *req_cls;

    if (con_info == NULL)
    {

        con_info = malloc(sizeof(struct connection_info_struct));
        *req_cls = (void*) con_info;
        if (con_info == NULL)
        {
            printf("didn't malloced con_info\n");
            return MHD_NO;
        }         
        con_info->answerstring = NULL;
        if (strcmp(method, "POST") == 0)
        {
            printf("Making postProcessor\n");
            const char* encoding = MHD_lookup_connection_value (connection,
                                          MHD_HEADER_KIND,
                                          MHD_HTTP_HEADER_CONTENT_TYPE);

            printf("encoding: %s\n", encoding);


            int smth1 = strlen(MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA);
            int smth = strncasecmp (MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA, encoding, smth1);
            printf("strlenth: %d\n", smth1);
            printf("smth: %d\n", smth);

            const char* boundary = &encoding[strlen (MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA)];
            printf("Boundary: %s", boundary);
               

            con_info->postprocessor = MHD_create_post_processor(connection, 512, iterate_post, (void*) con_info);
            printf("Passed postProcesser creation!\n");


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


    if (strcmp(method, "GET") == 0)
    {
        // char cwd[150];
        // if (getcwd(cwd, sizeof(cwd)) != NULL) {
        //     printf("Current working dir: %s\n", cwd);
        // }

        char* requestURL;
    
        if (strcmp(url, "/") == 0)
            requestURL = "/Frontpage.html";
        else
            requestURL = (char*) url;
    
        char fileName[100];
        sprintf(fileName, "../src/ClientSide/Design%s", requestURL);
        printf("fileName: %s\n", fileName);
    
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
        // printf("content: %s\n", page);
        struct MHD_Response *response;

        enum MHD_Result ret;

        printf("response initiated\n");
        response = MHD_create_response_from_buffer_static(strlen(page), page);
        printf("response made\n");
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);
        printf("reponse destroyed\n\n");
        return ret;
    }



    if (con_info->connectiontype == POST)
    {
        printf("answerString: %s\n", con_info->answerstring);
        if (*upload_data_size != 0)
        {
            printf("\n\nPOSTING SOMETHING::::\nUpload Data: %s, SIZE: %lu\n", upload_data, *upload_data_size);
            int postProcessType = MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
            printf("PostProcessType: %d", postProcessType);
            if (postProcessType != MHD_YES)
                return MHD_NO;
            printf("did return MHD_YES");
            *upload_data_size = 0;
            return MHD_YES;
        }
        else if (con_info->answerstring != NULL)
        {
            printf("\n\nPOST SUCCESS!\n\n");
            return MHD_NO;
        }
        printf("Post Failure!\n");
        return MHD_NO;
    }

    // else if (strcmp(method, "POST")== 0)
    // {
    //     printf("Starting Post\n");
    //     struct connection_info_struct *con_info = *req_cls;

    //     if (*upload_data_size != 0)
    //     {
    //         printf("Size != 0 (from post)\n");
    //         MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);

    //         *upload_data_size = 0;
            
    //         return MHD_YES;
    //     }
    //     else if (con_info->answerstring != NULL)
    //     {
    //         printf("Answer available\n");
    //     }
    // } 
    // else 
    // {
    //     return MHD_NO;
    // }
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