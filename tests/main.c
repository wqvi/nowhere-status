#include <check.h>
#include <stdlib.h>

extern Suite *modules_suite(void);

extern Suite *linked_list_suite(void);

int main(void) {
	SRunner *sr;

	sr = srunner_create(modules_suite());
	srunner_add_suite(sr, linked_list_suite());

	srunner_run_all(sr, CK_NORMAL);
	int failed_num = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed_num == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
