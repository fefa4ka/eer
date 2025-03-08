#include <criterion/criterion.h>
#include <criterion/options.h>
#include <criterion/redirect.h>

// Test runner configuration
int main(int argc, char *argv[]) {
    struct criterion_test_set *tests = criterion_initialize();
    criterion_options.jobs = 1; // Single process for better debug output
    
    int result = 0;
    if (criterion_handle_args(argc, argv, true))
        result = !criterion_run_all_tests(tests);
    
    criterion_finalize(tests);
    return result;
}
