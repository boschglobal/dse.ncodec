// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>

uint8_t __log_level__ = LOG_QUIET; /* LOG_QUIET LOG_INFO LOG_DEBUG LOG_TRACE */

extern int run_pdu_tests(void);
extern int run_pdu_can_tests(void);
extern int run_pdu_ip_tests(void);
extern int run_pdu_struct_tests(void);
extern int run_pdu_flexray_engine_tests(void);
extern int run_pdu_flexray_state_tests(void);
extern int run_pdu_flexray_startup_tests(void);
extern int run_pdu_flexray_single_node_tests(void);
extern int run_pdu_flexray_multi_node_tests(void);
extern int run_pdu_flexray_null_frame_tests(void);
extern int run_pdu_flexray_pop(void);


int main()
{
    int rc = 0;
    rc |= run_pdu_tests();
    rc |= run_pdu_can_tests();
    rc |= run_pdu_ip_tests();
    rc |= run_pdu_struct_tests();
    rc |= run_pdu_flexray_engine_tests();
    rc |= run_pdu_flexray_state_tests();
    rc |= run_pdu_flexray_startup_tests();
    rc |= run_pdu_flexray_single_node_tests();
    rc |= run_pdu_flexray_multi_node_tests();
    rc |= run_pdu_flexray_null_frame_tests();
    rc |= run_pdu_flexray_pop();
    return rc;
}
