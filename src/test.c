
#include "CuTest.h"
#include <stdlib.h>
#include <stdio.h>

CuSuite* socket_comm_suite();

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    CuSuiteAddSuite(suite, socket_comm_suite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    free(suite);
    free(output);
}

int main(void)
{
    RunAllTests();
}
