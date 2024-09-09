/* file minunit_example.c */

#include <stdio.h>
#include <iostream>
#include <sstream>
#include "minunit.h"

int tests_run = 0;
int tests_failed = 0;

char mu_last_message[1024];
int mu_status = 0;


// static void test_mfl_expect() {
//     std::istringstream stream("} ");
//     mu_assert("close quote", MapFLib::expect(stream, "}"));

//     stream = std::istringstream("    } ");
//     mu_assert("close quote with whitespace", MapFLib::expect(stream, "}"));
    
//     stream = std::istringstream("    } }   ");
//     mu_assert("multiple times", 
//         MapFLib::expect(stream, "}") && 
//         MapFLib::expect(stream, "}"));
// }

// static void mfl_parse_tests() {
    // mu_run_test(test_mfl_expect);
// }

static void all_tests() {

}

int main(int argc, char **argv) {
    all_tests();

    printf("Tests run: %d\n", tests_run);
    printf("Tests failed: %d\n", tests_failed);

    if(tests_failed > 0)
        printf("------- FAIL!!!! ------\n");

    return tests_failed > 0;
}