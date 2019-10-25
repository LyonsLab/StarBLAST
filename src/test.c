
#include "CuTest.h"
#include <stdlib.h>
#include <stdio.h>

CuSuite* socket_comm_suite();

void RunAllTests(void) {
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    CuSuite* s1 = socket_comm_suite();
    CuSuiteAddSuite(suite, s1);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    free(s1);
    CuSuiteDelete(suite);
    CuStringDelete(output);
}

int main(void)
{
    RunAllTests();
}
