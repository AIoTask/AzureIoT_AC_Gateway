#include "http_operation.h"
#define TASK_URL "https://aiot-task-api.azurewebsites.net/api/GetENVData?env_id=1&time=2020/01/01_00:00:00"

static int uart_fd;

struct string {
    char* ptr;
    size_t len;
};

void init_string(struct string* s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

size_t env_transfer(void* ptr, size_t size, size_t nmemb, struct string* s)
{
    size_t new_len = s->len + size * nmemb;
    char* eol = "\r\n";
    int res = write(uart_fd, ptr, new_len);
    Log_Debug("Get and Transfer: %d", res);
    res = write(uart_fd, eol, 2);
    Log_Debug(" -> EOL +%d\n", res);

    return res + 2;
}

int http_get_env_data() {
	CURL* curl;
	CURLcode res;

    struct string s;
    init_string(&s);

    int return_code = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, TASK_URL);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* Perform the request, res will get the return code */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        Log_Debug("%s\n", s.ptr);
        free(s.ptr);
        /* Check for errors */
        if (res != CURLE_OK){
            fprintf(stderr, "[ENV] curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            return_code = -1;
        }   

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return return_code;
}

int http_transfer_ac_data(int uart_fdno) {

    CURL* curl;
    CURLcode res;

    char* ac_url = "https://aiot-task-api.azurewebsites.net/api/GetACData?air_id=1&time=2020/01/01_00:00:00 ";

    struct string s;
    init_string(&s);

    int return_code = 0;

    uart_fd = uart_fdno;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, ac_url);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* Perform the request, res will get the return code */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, env_transfer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);

        free(s.ptr);
        /* Check for errors */
        if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
            fprintf(stderr, "[AC] curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            return_code = -1;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return return_code;
}