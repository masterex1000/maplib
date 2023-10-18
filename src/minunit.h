#define MU_STR1(x) #x
#define MU_STR(x) MU_STR1(x)

#define MU_EPSILON 1E-12

// #define mu_assert(message, test) do { if (!(test)) return (message " : " __FILE__ ":" MU_STR(__LINE__) ); } while (0)

/*
#define mu_assert_float(message, expected, actual)              \
    do { if(!(expected - actual <= MU_EPSILON))                 \
        return snprintf(message " : " __FILE__ ":" MU_STR(__LINE__) " expected \"%f\", returned \"%f\"", expected, actual);  \
    } while (0)
*/

/*
#define mu_run_test(test) do { const char *message = test(); tests_run++; \
                            if (message) return message; } while (0)
*/

extern int tests_run;
extern int tests_failed;

extern char mu_last_message[1024];
extern int mu_status;

#define mu_assert(message, test) do {           \
    if (!(test)) {                              \
        mu_status = 1;                          \
        snprintf(mu_last_message, 1024, message " : " __FILE__ ":" MU_STR(__LINE__));   \
        return;                                 \
    } } while (0)


#define mu_assert_float(message, expected, actual)                  \
    do { if(!(expected - actual <= MU_EPSILON))  {                  \
        mu_status = 1;                                              \
        snprintf(mu_last_message, 1024, message " expected \"%f\", returned \"%f\"" " : " __FILE__ ":" MU_STR(__LINE__), (double) expected, (double) actual);  \
        return;                                                     \
    }                                                               \
    } while (0)


#define mu_run_test(test) do {                          \
    mu_status = 0;                                      \
    test();                                             \
    tests_run++;                                        \
    if (mu_status) {                                    \
        printf("F %s %s\n", #test, mu_last_message);    \
        tests_failed++;                                 \
    }                                                   \
    } while (0)

