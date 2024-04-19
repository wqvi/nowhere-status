#include <nowhere_status.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <check.h>

Suite *linked_list_suite(void);

void sstrr(char *_str, size_t _len);

void sstrl(char *_str, size_t _len);

int sanitize(char *_str, const char *_initial_str);

void trim_whitespace(char *_str, size_t _len);

START_TEST(test_playerctl_sanitize_function) {
	const char *phrases[] = {
		"aaaaaaaaa",
		"0\'34\" \'0123 567 901",
		"EEEEEEEEEEEEEEE",
		"RRRRRRRR AAAAA (RRRRR EEEE)",
		"01234 67 901 34567",
		"ADDDDDDDDDDDDDD (MMMMMM RRRRR)",
		"BDDDDDDDDDDDDDDD (MMMMMM RRRRR)",
		"CDDDDDDDDDDDDDD  (MMMMMM RRRRR)",
		"01234 67 9011 - 1234",
		"AAAAAAA ZZZZZZZ, AAA AAAAA AAAA AAAAAAA A AAAA",
		"AAAAAAAAAA AAAAA AAAAAAAAAAA , AAAAAAA AAAAA AAAAAAAAAAA",
		"OOO-AAA-AAAA || CCCCCCCCCC 546",
		"AAAA: 12 AAAA AA AAAAA | AAAAAAAAA'A AAAAAAA AAA AAAAAAAAAA | AAA AAAAAA AAA",
	};
	const char *expects[] = {
		"aaaaaaaaa",
		"0\'34\\\" \'0123 56-",
		"EEEEEEEEEEEEEEE",
		"RRRRRRRR AAAAA",
		"01234 67 901 34-",
		"ADDDDDDDDDDDDDD-",
		"BDDDDDDDDDDDDDD-",
		"CDDDDDDDDDDDDDD-",
		"01234 67 9011-",
		"AAAAAAA ZZZZZZZ-",
		"AAAAAAAAAA AAAA-",
		"OOO-AAA-AAAA-",
		"L",
	};

	char actual[64];

	for (int i = 0; i < sizeof(phrases) / sizeof(const char *); i++) {
		const char *phrase = phrases[i];
		const char *expected = expects[i];

		memset(actual, 0, sizeof(actual));
		int code = sanitize(actual, phrase);
		if (code != 0) {
			ck_assert_int_eq(code, expected[0]);
			continue;
		}

		size_t expected_length = strlen(expected);
		if (strlen(actual) > 16) {
			expected_length = 16;
			memset(actual + 16, 0, sizeof(actual) - 16);
		}

		ck_assert_str_eq(actual, expected);

		ck_assert_int_eq(strlen(actual), expected_length);
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
		sstrr(actual, 16);

		ck_assert_str_eq(actual, expected);
	}
}
END_TEST

START_TEST(test_trim_whitespace_function) {
#define PHRASE "123456789012345 12345"
#undef EXPECTED
#define EXPECTED "123456789012345"

	const char *phrases[] = {
		"01234567 901234 (78901 3456)",
		"0123456789012345 7890",
	};
	const char *expects[] = {
		"01234567 901234",
		"0123456789012345",
	};

	char actual[64];

	for (int i = 0; i < sizeof(phrases) / sizeof(const char *); i++) {
		const char *phrase = phrases[i];
		const char *expected = expects[i];

		memset(actual, 0, sizeof(actual));
		memcpy(actual, phrase, strlen(phrase));
		trim_whitespace(actual, 16);

		ck_assert_str_eq(actual, expected);
	}
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
	SRunner *sr;

	sr = srunner_create(modules_suite());
	srunner_add_suite(sr, linked_list_suite());

	srunner_run_all(sr, CK_NORMAL);
	int failed_num = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed_num == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
