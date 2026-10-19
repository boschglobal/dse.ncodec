// Copyright 2026 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>
#include <dse/logger.h>

uint8_t __log_level__ = LOG_QUIET; /* LOG_QUIET LOG_INFO LOG_DEBUG LOG_TRACE */

extern int run_codec_tests(void);
extern int run_uds_tests(void);
extern int run_tp_tests(void);
extern int run_srpc_tests(void);
extern int run_xil_tests(void);

int main()
{
    int rc = 0;
    rc |= run_codec_tests();
    rc |= run_uds_tests();
    rc |= run_tp_tests();
    rc |= run_srpc_tests();
    rc |= run_xil_tests();
    return rc;
}
