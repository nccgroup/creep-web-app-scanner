/*
 * =====================================================================================
 *
 *       Filename:  check_creep.c
 *
 *    Description:  Creep unit tests
 *
 *        Version:  1.0
 *        Created:  04/11/13 20:04:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aidan Marlin (aidan.marlin@gmail.com), 
 *   Organization:  NCC Group
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <check.h>
#include "../src/creep.h"

START_TEST(test_creep_exit_error)
{
   int errorNum = 1;

   ck_assert(exit_error(errorNum) == errorNum);
}
END_TEST

Suite * creep_suite(void)
{
   Suite *s = suite_create ("Creep");

   /* Core test case */
   TCase *tc_core = tcase_create ("Core");
   tcase_add_test (tc_core, test_creep_exit_error);
   suite_add_tcase (s, tc_core);

   return s;
}

int main(void)
{
   int number_failed;
   Suite *s = creep_suite ();
   SRunner *sr = srunner_create (s);
   srunner_run_all (sr, CK_NORMAL);
   number_failed = srunner_ntests_failed (sr);
   srunner_free (sr);
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
