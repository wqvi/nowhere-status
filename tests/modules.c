#include <nowhere_status.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

void sstrr(char *_str, size_t _len);

void sstrl(char *_str, size_t _len);

int sanitize(char *_str, const char *_initial_str);

void trim_whitespace(char *_str, size_t _len);

START_TEST(test_playerctl_sanitize_function) {
	const char *phrases[] = {
		"aaaaaaaaa",
		"9\'99\" \'TTTT TTT EEE",
		"EEEEEEEEEEEEEEE",
		"RRRRRRRR AAAAA (RRRRR EEEE)",
		//"RRRRRRRR AAAA (RRRRR EEEE)",
		"DDDDDDDDDDDDDDD (MMMMMM RRRRR)",
		"DDDDDDDDDDDDDDDD (MMMMMM RRRRR)",
		"PPPPP TT TTT MMMMM",
	};
	const char *expects[] = {
		"aaaaaaaaa",
		"9\'99\\\" \'T...",
		"EEEEEEEEEEEEEEE",
		"RRRRRRRR AAAAA",
		//"RRRRRRRR AAAA",
		"DDDDDDDDDDDDDDD",
		"DDDDDDDDDDDDDDDD",
		"PPPPP TT TTT...",
	};

	char actual[64];

	for (int i = 0; i < sizeof(phrases) / sizeof(const char *); i++) {
		const char *phrase = phrases[i];
		const char *expected = expects[i];

		memset(actual, 0, sizeof(actual));
		memcpy(actual, phrase, strlen(phrase));
		actual[16] = '\0';
		sanitize(actual, phrase);

		ck_assert_str_eq(actual, expected);
	}
}
END_TEST

START_TEST(test_playerctl_sstrr_function) {
	const char *phrases[] = {
		"9\'99\\\" \'T",
		"abc",
		"(())***",
	};
	const char *expects[] = {
		"99\'99\\\" \'T",
		"aabc",
		"((())***",
	};

	char actual[64];

	for (int i = 0; i < sizeof(phrases) / sizeof(const char *); i++) {
		const char *phrase = phrases[i];
		const char *expected = expects[i];

		memset(actual, 0, sizeof(actual));
		memcpy(actual, phrase, strlen(phrase));
		actual[16] = '\0';
		sstrr(actual, strlen(actual) + 1);

		ck_assert_str_eq(actual, expected);
	}
}
END_TEST

START_TEST(test_trim_whitespace_function) {
#define PHRASE "123456789012345 12345"
#undef EXPECTED
#define EXPECTED "123456789012345"

	char actual[32] = PHRASE;

	// this assumes other things have been done
	// to the string. Usually resulting in a 16 long byte
	// working region without null termination
	trim_whitespace(actual, 15);

	ck_assert_str_eq(actual, EXPECTED);
}
END_TEST

Suite *modules_suite(void) {
	Suite *s;
	TCase *tc_san;
	TCase *tc_str;
	TCase *tc_trim;

	s = suite_create("modules");

	tc_san = tcase_create("sanitize");
	tc_str = tcase_create("shift string right");
	tc_trim = tcase_create("trim");

	tcase_add_test(tc_san, test_playerctl_sanitize_function);
	suite_add_tcase(s, tc_san);

	tcase_add_test(tc_str, test_playerctl_sstrr_function);
	suite_add_tcase(s, tc_str);

	tcase_add_test(tc_trim, test_trim_whitespace_function);
	suite_add_tcase(s, tc_trim);

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
