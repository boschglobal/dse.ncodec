// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSE_DIAGNET_EXAMPLES_PLANT_PLANT_H_
#define DSE_DIAGNET_EXAMPLES_PLANT_PLANT_H_

#include <stdint.h>

#define PLANT_MAX_DATA_SIZE 256


typedef struct PlantAddressInfo {
    uint8_t reqType;
    uint8_t suppressPosResponse;
    uint8_t sourceofRequest;
} PlantAddressInfo;

typedef struct PlantMessage {
    const uint8_t* req_data;
    uint32_t       req_data_len;
    uint8_t*       res_data;
    uint32_t       res_data_len;
    uint32_t       res_max_data_len;
    uint8_t        context_id;
    int8_t         rx_pdu_id;

    PlantAddressInfo address_info;
} PlantMessage;


uint8_t plant_clear(uint8_t status, PlantMessage* msg, uint8_t* error_code);
uint8_t plant_append(uint8_t status, PlantMessage* msg, uint8_t* error_code);
uint8_t plant_read(uint8_t status, PlantMessage* msg, uint8_t* error_code);

#endif  // DSE_DIAGNET_EXAMPLES_PLANT_PLANT_H_
