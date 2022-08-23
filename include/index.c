#include "index.h"

#define INTEGER_OPTION_MAX 10000

binary_string_t* make_string() {
    binary_string_t* mem = malloc(sizeof(binary_string_t));
    mem->size = 0;
    mem->content = NULL;
    return mem;
}

void free_string(binary_string_t* obj) {
    if (obj && obj->content)
        free(obj->content);
    if (obj)
        free(obj);
}

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    binary_string_t* mem = (binary_string_t*)userp;
    if (mem->content == NULL) {
        mem->content = (char*)malloc(1);
    }

    mem->content = (char*)realloc(mem->content, mem->size + realsize + 1);
    if (mem->content == NULL) {
        return 0; // Out-of-memory
    }

    memcpy(&(mem->content[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->content[mem->size] = 0;
    return realsize;
}

// Curl bindings
curl_instance_t* bind_curl_easy_init() {
    curl_instance_t* instance = malloc(sizeof(curl_instance_t));
    instance->curl = curl_easy_init();
    return instance;
}

int bind_curl_easy_setopt(curl_instance_t* instance, int option, void* parameter) {
    // printf("****** hijack test begins: \n");
    // int val = curl_easy_setopt(instance->curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
    // printf("****** hijack test ends. opt: %d, val: %d, result is: %d\n", CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0, val);
    CURLoption opt_value = (CURLoption) option;
    CURLcode res = CURLE_OK;
    if (opt_value == CURLOPT_WRITEDATA) {
        res = curl_easy_setopt(instance->curl, CURLOPT_WRITEFUNCTION, write_callback);
    } else if (opt_value == CURLOPT_WRITEHEADER) {
        res = curl_easy_setopt(instance->curl, CURLOPT_HEADERFUNCTION, write_callback);
    }
    if (res != CURLE_OK) {
        return (int)res;
    }
    // printf("option: %d, setopt parameter: %d\n", option, *(int*)parameter);
    // for integer options, we need to convert param from pointers to integers
    if (option < INTEGER_OPTION_MAX) {
        return (int)curl_easy_setopt(instance->curl, (CURLoption)option, *(int*)parameter);
    }
    return (int)curl_easy_setopt(instance->curl, (CURLoption)option, parameter);
}

int bind_curl_easy_perform(curl_instance_t* instance) {
    return (int)curl_easy_perform(instance->curl);
}

int bind_curl_easy_getinfo(curl_instance_t* instance, int option, void* retValue) {
    return (int)curl_easy_getinfo(instance->curl, (CURLINFO)option, retValue);
}

void bind_curl_easy_cleanup(curl_instance_t* instance) {
    curl_easy_cleanup(instance->curl);
}

char* bind_curl_version() {
    return curl_version();
}