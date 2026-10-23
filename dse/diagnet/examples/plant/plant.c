// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>
#include <dse/diagnet/examples/plant/plant.h>


enum {
    PLANT_ERROR_NONE = 0,
    PLANT_ERROR_INVALID_ARGUMENT = 1,
    PLANT_ERROR_BUFFER_TOO_SMALL = 2,
};

static uint8_t  plant_buffer[PLANT_MAX_DATA_SIZE];
static uint32_t plant_buffer_len;


static uint8_t _plant_fail(uint8_t* error_code, uint8_t code)
{
    if (error_code) *error_code = code;
    return code;
}


uint8_t plant_clear(uint8_t status, PlantMessage* msg, uint8_t* error_code)
{
    (void)status;
    (void)msg;

    memset(plant_buffer, 0, sizeof(plant_buffer));
    plant_buffer_len = 0;
    printf("Clear (0 bytes):\n");

    if (error_code) *error_code = PLANT_ERROR_NONE;
    return PLANT_ERROR_NONE;
}


uint8_t plant_append(uint8_t status, PlantMessage* msg, uint8_t* error_code)
{
    (void)status;

    if (msg == NULL || (msg->req_data_len && msg->req_data == NULL)) {
        return _plant_fail(error_code, PLANT_ERROR_INVALID_ARGUMENT);
    }

    uint32_t append_len = msg->req_data_len;
    uint32_t available = PLANT_MAX_DATA_SIZE - plant_buffer_len;
    uint8_t  result = PLANT_ERROR_NONE;

    if (append_len > available) {
        append_len = available;
        result = PLANT_ERROR_BUFFER_TOO_SMALL;
    }

    if (append_len) {
        memcpy(plant_buffer + plant_buffer_len, msg->req_data, append_len);
    }
    plant_buffer_len += append_len;
    printf("Append (%u bytes): \"%.*s\"\n", (unsigned int)append_len,
        (int)append_len, append_len ? (const char*)msg->req_data : "");

    if (error_code) *error_code = result;
    return result;
}


uint8_t plant_read(uint8_t status, PlantMessage* msg, uint8_t* error_code)
{
    (void)status;

    if (msg == NULL || (plant_buffer_len && msg->res_data == NULL)) {
        return _plant_fail(error_code, PLANT_ERROR_INVALID_ARGUMENT);
    }

    uint32_t read_len = plant_buffer_len;
    uint8_t  result = PLANT_ERROR_NONE;
    if (read_len > msg->res_max_data_len) {
        read_len = msg->res_max_data_len;
        result = PLANT_ERROR_BUFFER_TOO_SMALL;
    }
    if (read_len) memcpy(msg->res_data, plant_buffer, read_len);
    msg->res_data_len = read_len;
    printf("Read (%u bytes): \"%.*s\"\n", (unsigned int)read_len, (int)read_len,
        (const char*)plant_buffer);
    printf(
        "Read Message: \"%.*s\"\n", (int)read_len, (const char*)msg->res_data);

    if (error_code) *error_code = result;
    return result;
}
