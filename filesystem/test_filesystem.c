#include <check.h>
#include <stdlib.h>
#include "filesystem.h"  // Assuming your is_problematic_name() is declared here

START_TEST(test_normal_names)
{
    ck_assert_int_eq(is_problematic_name("normal.txt"), 0);
    ck_assert_int_eq(is_problematic_name("another_normal_file"), 0);
}
END_TEST

START_TEST(test_problematic_names)
{
    ck_assert_int_eq(is_problematic_name("file:with:colons"), 1);
    ck_assert_int_eq(is_problematic_name("file*with*stars"), 1);
    ck_assert_int_eq(is_problematic_name("file?with?question"), 1);
    ck_assert_int_eq(is_problematic_name("file\"with\"quotes"), 1);
    ck_assert_int_eq(is_problematic_name("file<with>brackets"), 1);
    ck_assert_int_eq(is_problematic_name("file|with|pipes"), 1);
    ck_assert_int_eq(is_problematic_name("file&with&ampersand"), 1);
}
END_TEST

START_TEST(test_non_printable_chars)
{
    ck_assert_int_eq(is_problematic_name("file\nwith\nnewlines"), 1);
    ck_assert_int_eq(is_problematic_name("file\twith\ttabs"), 1);
}
END_TEST

Suite * problematic_name_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Problematic Name");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_normal_names);
    tcase_add_test(tc_core, test_problematic_names);
    tcase_add_test(tc_core, test_non_printable_chars);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = problematic_name_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}