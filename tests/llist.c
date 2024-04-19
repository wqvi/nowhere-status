#include <check.h>
#include <swaybar.h>
#include <stdlib.h>

static struct node *heads[2] = {
	NULL
};

START_TEST(test_linked_list_create_function) {
	static struct node_info infos[2][4] = {
		{
			{
				NOWHERE_NODE_DEFAULT,
				'A',
				NULL
			},
			{
				NOWHERE_NODE_DEFAULT,
				'B',
				NULL
			}
		},
		{
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
		}
	};
	static int expects[] = {
		0,
		0
	};

	struct node *head = heads[_i];
	struct node_info *info = infos[_i];
	int expected = expects[_i];

	int ret = llist_create(&head, info, 2);

	ck_assert_int_eq(ret, expected);

	ck_assert_ptr_nonnull(head);
}
END_TEST

START_TEST(test_linked_list_get_function) {
	static char names[] = {
		'A',
		'B',
		'C',
		'D'
	};

       struct node *head = heads[_i];
       char name = names[_i];

       struct node *node = llist_get(head, name);

       //ck_assert_ptr_nonnull(node);

       free(head);

}
END_TEST

Suite *linked_list_suite(void) {
	Suite *s;
	TCase *tc_create;
	TCase *tc_free;

	s = suite_create("linked_list");

	tc_create = tcase_create("linked list create");
	tc_free = tcase_create("linked list get");

	tcase_add_loop_test(tc_create, test_linked_list_create_function, 0, 2);
	suite_add_tcase(s, tc_create);

	tcase_add_loop_test(tc_free, test_linked_list_get_function, 0, 2);
	suite_add_tcase(s, tc_free);

	return s;
}
