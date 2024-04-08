#include <nowhere_status.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

void sstrr(char *_str, size_t _len);

void sstrl(char *_str, size_t _len);

int sanitize(char *_str, const char *_initial_str, size_t _initial_length);

START_TEST(test_playerctl_sanitize_function) {
#define PHRASE "9\'99\" \'TTTT TTT EEE"
#define EXPECTED "9\'99\\\" \'T..."

	char actual[32];
	memset(actual, 0, sizeof(actual));
	memcpy(actual, PHRASE, 16);
	sanitize(actual, PHRASE, strlen(PHRASE));

	ck_assert_str_eq(actual, EXPECTED);

	ck_assert_int_eq(strlen(actual), strlen(EXPECTED));

#undef PHRASE
#define PHRASE "RRRRRRRR AAAAA (RRRRR EEEE)"
#undef EXPECTED
#define EXPECTED "RRRRRRRR AAAAA"

	memset(actual, 0, sizeof(actual));
	memcpy(actual, PHRASE, 16);
	sanitize(actual, PHRASE, strlen(PHRASE));

	ck_assert_str_eq(actual, EXPECTED);

	ck_assert_int_eq(strlen(actual), strlen(EXPECTED));

#undef PHRASE
#define PHRASE "DDDDDDDDDD 9.9 (MMMMMM RRRRR)"
#undef EXPECTED
#define EXPECTED "DDDDDDDDDD 9.9"

	memset(actual, 0, sizeof(actual));
	memcpy(actual, PHRASE, 16);
	sanitize(actual, PHRASE, strlen(PHRASE));

	ck_assert_str_eq(actual, EXPECTED);

	ck_assert_int_eq(strlen(actual), strlen(EXPECTED));

#undef PHRASE
#define PHRASE "AA AAAA A'A AAAAAAA AA"
#undef EXPECTED
#define EXPECTED "AA AAAA A'A A..."

	memset(actual, 0, sizeof(actual));
	memcpy(actual, PHRASE, 16);
	sanitize(actual, PHRASE, strlen(PHRASE));

	ck_assert_str_eq(actual, EXPECTED);

	ck_assert_int_eq(strlen(actual), strlen(EXPECTED));

#undef PHRASE
#define PHRASE "aaaa bbbbbbbb bbbbbb on X: \"aaa bb ccc ddddddd eeeeeee ffff ggggggggg hhhh i jjjjjj kkkkk lll mnopq://r.st/uvwxyzzzzx\" / X"
#undef EXPECTED
#define EXPECTED 'X'

	memset(actual, 0, sizeof(actual));
	memcpy(actual, PHRASE, 16);
	(*(int *)&actual) = sanitize(actual, PHRASE, strlen(PHRASE));

	ck_assert_int_eq((*(int *)&(actual)), EXPECTED);
}
END_TEST

START_TEST(test_playerctl_sstrr_function) {
#undef PHRASE
#define PHRASE "9\'99\" \'TTTT TTT EEE"
#undef EXPECTED
#define EXPECTED "99\'99\" \'TTTT TTT EEE"

	char actual[32] = PHRASE;

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, EXPECTED);

#undef PHRASE
#define PHRASE "abc"
#undef EXPECTED
#define EXPECTED "aabc"

	memset(actual, 0, sizeof(actual));
	snprintf(actual, 4, PHRASE);

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, EXPECTED);

#undef PHRASE
#define PHRASE "(())***"
#undef EXPECTED
#define EXPECTED "((())***"

	memset(actual, 0, sizeof(actual));
	snprintf(actual, 8, PHRASE);

	sstrr(actual, strlen(actual) + 1);

	ck_assert_str_eq(actual, EXPECTED);
}
END_TEST

START_TEST(test_playerctl_sstrl_function) {
#undef PHRASE
#define PHRASE "abc_abbbc"
#undef EXPECTED
#define EXPECTED "abc_bbbc"

	char actual[32] = PHRASE;

	memset(actual, 0, sizeof(actual));
	snprintf(actual, sizeof(PHRASE), PHRASE);

	sstrl(actual + 4, strlen(actual) - 4);

	ck_assert_str_eq(actual, EXPECTED);

#undef PHRASE
#define PHRASE "(())***"
#undef EXPECTED
#define EXPECTED "())***"

	memset(actual, 0, sizeof(actual));
	snprintf(actual, 8, PHRASE);

	sstrl(actual, strlen(actual));

	ck_assert_str_eq(actual, EXPECTED);
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

	tcase_add_test(tc_core, test_playerctl_sstrl_function);
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
