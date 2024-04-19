#include <check.h>
#include <swaybar.h>
#include <stdlib.h>

START_TEST(test_linked_list_create_function) {
	struct node *heads[] = {
		NULL
	};
	struct node_info infos[2][2] = {
		{
			{
				NOWHERE_NODE_DEFAULT,
				'T',
				NULL
			},
			{
				NOWHERE_NODE_DEFAULT,
				'T',
				NULL
			}
		},
		{
			{
				NOWHERE_NODE_DEFAULT,
				'T',
				NULL,
			},
			{
				NOWHERE_NODE_DEFAULT,
				'T',
				NULL
			}
		}
	};
	int expects[] = {
		0
	};

	for (int i = 0; i < sizeof(heads) / sizeof(struct node *); i++) {
		struct node *head = heads[i];
		struct node_info *info = infos[i];
		int expected = expects[i];

		int ret = llist_create(&head, info, 2);

		ck_assert_int_eq(ret, expected);

		ck_assert_ptr_nonnull(head);

		free(head);
	}
}
END_TEST

Suite *linked_list_suite(void) {
	Suite *s;
	TCase *tc_create;

	s = suite_create("linked_list");

	tc_create = tcase_create("linked list create");

	tcase_add_test(tc_create, test_linked_list_create_function);
	suite_add_tcase(s, tc_create);

	return s;
}
