#ifndef HTTP_OPERATION
#define HTTP_OPERATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <applibs/log.h>

#include <applibs/storage.h>  // required only if you supply a certificate in the image package
#include <tlsutils/deviceauth_curl.h> // required only for mutual authentication
#include <curl/curl.h>

int http_get_env_data();

int http_transfer_ac_data(int);

#endif