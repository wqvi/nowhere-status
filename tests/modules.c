#include <nowhere_status.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

void sstrr(char *_str, size_t _len);

void tidy(char *_str, size_t _initial_length);

START_TEST(test_playerctl_tidy_function) {
	const char *phrase = "4\'18\" \'Till the End";

	char actual[32];
	memcpy(actual, phrase, 16);
	tidy(actual, strlen(phrase));

	const char *expected = "4\\\'18\\\"...";
	ck_assert_str_eq(actual, expected);
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

	tcase_add_test(tc_core, test_playerctl_tidy_function);
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
