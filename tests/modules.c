#include <nowhere_status.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

void sstrr(char *_str, size_t _len);

void sanitize(char *_str, size_t _initial_length);

START_TEST(test_playerctl_sanitize_function) {
	char actual[32];
	memset(actual, 0, sizeof(actual));
	memcpy(actual, "4\'18\" \'Till the End", 16);
	sanitize(actual, strlen("4\'18\" \'Till the End"));

	ck_assert_str_eq(actual, "4\'18\\\" \'Till...");

	ck_assert_int_eq(strlen(actual), strlen("4\'18\\\" \'Till..."));

	memset(actual, 0, sizeof(actual));
	memcpy(actual, "Reckless Ardor (Radio Edit)", 16);
	sanitize(actual, strlen("Reckless Ardor (Radio Edit)"));

	ck_assert_str_eq(actual, "Reckless Ardor");

	ck_assert_int_eq(strlen(actual), strlen("Reckless Ardor"));

	memset(actual, 0, sizeof(actual));
	memcpy(actual, "Discopolis 2.0 (MEDUZA Remix)", 16);
	sanitize(actual, strlen("Discopolis 2.0 (MEDUZA Remix)"));

	ck_assert_str_eq(actual, "Discopolis 2.0");

	ck_assert_int_eq(strlen(actual), strlen("Discopolis 2.0"));

}
END_TEST

START_TEST(test_playerctl_sstrr_function) {
	char actual[32] = "4\'18\" \'Till the End";

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, "44\'18\" \'Till the End");

	memset(actual, 0, sizeof(actual));
	snprintf(actual, 4, "abc");

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, "aabc");

	memset(actual, 0, sizeof(actual));
	snprintf(actual, 8, "(())***");

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, "((())***");
}
END_TEST

Suite *modules_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("modules");

	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_playerctl_sanitize_function);
	suite_add_tcase(s, tc_core);

	tcase_add_test(tc_core, test_playerctl_sstrr_function);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void) {
	Suite *s;
	SRunner *sr;

	s = modules_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	int failed_num = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed_num == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
