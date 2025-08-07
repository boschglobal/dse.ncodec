// Copyright 2025 Robert Bosch GmbH
//
// SPDX-License-Identifier: Apache-2.0

#include <dse/testing.h>

extern int run_can_fbs_tests(void);

int main()
{
    int rc = 0;
    rc |= run_can_fbs_tests();
    return rc;
}
