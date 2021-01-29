#include "http_operation.h"
#define TASK_URL "https://aiot-task-api.azurewebsites.net/api/GetACData?air_id=1&time=2020/01/21_00:00:00"

int http_get_env_data() {
	CURL* curl;
	CURLcode res;

    int return_code = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, TASK_URL);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            return_code = -1;
        }   
        else
            Log_Debug("GET DATA OK!\n");

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return return_code;
}

int http_get_ac_data() {
    return 0;
}