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
#include <event.h>
#include <evhttp.h>
#include "../src/creep.h"
#include "../src/def_error_messages.h"

/*START_TEST(test_creep_exit_error)
{
   int errorNum = 1;

//   ck_assert(exit_error(errorNum) == errorNum);
//   ck_assert(exit_error(255) == 255);
}
END_TEST*/

START_TEST(test_creep_exit_message)
{
   int errorNum = 1;
   Error error[255];

/*typedef struct {
   int number;
   char message[128];
   uint8_t log;
} Error;*/

   ck_assert(setup_error_messages(error) == 0);
//   printf("error msg == %s\nDEF_ERROR_1 == %s",error[1].message,DEF_ERROR_1);
   ck_assert(error[1].number == 1);
   ck_assert(strcmp(error[1].message,DEF_ERROR_1) == 0);
   ck_assert(error[1].log == 0x0);

   ck_assert(error[10].number == 10);
   ck_assert(strcmp(error[10].message,DEF_ERROR_10) == 0);
   ck_assert(error[10].log == 0x0);
}
END_TEST

Suite * creep_suite(void)
{
   Suite *s = suite_create ("Creep");

   /* Core test case */
   TCase *tc_core = tcase_create ("Core");
   //tcase_add_test (tc_core, test_creep_exit_error);
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
