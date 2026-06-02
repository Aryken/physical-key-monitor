/*
 * REST API 实现
 */
#include <stdlib.h>
#include "rest_api.h"
#include "../utils/logger.h"

struct RestApi {
    int dummy;
};

RestApi* rest_api_create(void) {
    RestApi *api = malloc(sizeof(RestApi));
    if (!api) {
        LOG_ERROR("Failed to allocate memory for REST API");
        return NULL;
    }
    return api;
}

void rest_api_destroy(RestApi *api) {
    if (api) {
        free(api);
    }
}
