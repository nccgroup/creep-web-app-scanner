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
#include <stdint.h>
#include <check.h>
#include "../src/creep.h"
#include "../src/def_error_messages.h"

START_TEST(test_creep_exit_error)
{
   int errorNum = 1;

   ck_assert(exit_error(errorNum) == errorNum);
   ck_assert(exit_error(256) == 255);
   ck_assert(exit_error(-1) == 255);
}
END_TEST

START_TEST(test_creep_exit_message)
{
   int errorNum = 1;
   char tmpErrorStr[128];

   //char errorStr[255][64];
   char **errorStr = malloc(sizeof(char) * 128);

   ck_assert(setup_error_messages(errorStr) == 0);

   strcpy(tmpErrorStr,DEF_ERROR_1);
   ck_assert(strcmp(exit_message(errorNum,errorStr),tmpErrorStr) == 0);

/*   errorNum = 2;
   strcpy(errorStr,DEF_ERROR_2);
   ck_assert(strcmp(exit_message(errorNum),errorStr) == 0);*/
}
END_TEST

Suite * creep_suite(void)
{
   Suite *s = suite_create ("Creep");

   /* Core test case */
   TCase *tc_core = tcase_create ("Core");
   tcase_add_test (tc_core, test_creep_exit_error);
   tcase_add_test (tc_core, test_creep_exit_message);
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
