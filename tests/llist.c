#include <check.h>
#include <swaybar.h>
#include <stdlib.h>

static struct node *head;

void setup(void) {
	static struct node_info infos[4] = {
		{
			NOWHERE_NODE_DEFAULT,
			'A',
			NULL
		},
		{
			NOWHERE_NODE_DEFAULT,
			'B',
			NULL
		},
		{
			NOWHERE_NODE_DEFAULT,
			'C',
			NULL,
		},
		{
			NOWHERE_NODE_DEFAULT,
			'D',
			NULL
		}
	};

	llist_create(&head, infos, 4);
}

void teardown(void) {
	free(head);
}

START_TEST(test_llist_get) {
	static char names[] = {
		'A',
		'D',
		'C',
		'B'
	};

	struct node *expects[] = {
		head + 0,
		head + 3,
		head + 2,
		head + 1
	};

	struct node *node = llist_get(head, names[_i]);

	ck_assert_ptr_nonnull(node);

	ck_assert_ptr_eq(node, expects[_i]);

}
END_TEST

Suite *linked_list_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("linked_list");

	tc_core = tcase_create("core");

	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_loop_test(tc_core, test_llist_get, 0, 4);
	suite_add_tcase(s, tc_core);

	return s;
}
