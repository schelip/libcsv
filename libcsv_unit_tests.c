#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <CUnit/Basic.h>

#include "libcsv.h"

#define TEST_CSV "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9"
#define REDIRECT_FILE "test.txt"
#define REOPEN_PATH "/dev/tty"

void test_processCsv_1_column_selected(void)
{
  char buf[BUFSIZ];
  char *expected = "header1\n1\n4\n7";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1", "");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_n_columns_selected(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n4,5\n7,8";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_n_columns_arbitrarily_selected(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n4,5\n7,8";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header2,header1", "");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_all_columns_selected(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "", "");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_equal_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2=5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_less_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2<5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_greater_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n7,8";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2>5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_not_equal_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n7,8";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2!=5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_less_equal_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2<=5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_greater_equal_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n4,5\n7,8";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2>=5");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_multiple_filters(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header1>2\nheader2<7");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_multiple_arbitrary_filters(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header2>7\nheader1<2");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_multiple_filters_same_header(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header1=1\nheader1=4");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_filter_not_selected_column(void)
{
  char buf[BUFSIZ];
  char *expected = "header1,header2\n1,2\n4,5";
  freopen(REDIRECT_FILE, "w+", stdout);
  processCsv(TEST_CSV, "header1,header2", "header3<9");
  freopen(REOPEN_PATH, "w", stdout);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_select_header_not_found(void)
{
  char buf[BUFSIZ];
  char *expected = "Header 'header5' not found in CSV file/string";
  freopen(REDIRECT_FILE, "w+", stderr);
  processCsv(TEST_CSV, "header5", "");
  freopen(REOPEN_PATH, "w", stderr);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_filter_header_not_found(void)
{
  char buf[BUFSIZ];
  char *expected = "Header 'header5' not found in CSV file/string";
  freopen(REDIRECT_FILE, "w+", stderr);
  processCsv(TEST_CSV, "", "header5=1");
  freopen(REOPEN_PATH, "w", stderr);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

void test_processCsv_invalid_filter(void)
{
  char buf[BUFSIZ];
  char *expected = "Invalid filter: 'header1#2'";
  freopen(REDIRECT_FILE, "w+", stderr);
  processCsv(TEST_CSV, "", "header1#2");
  freopen(REOPEN_PATH, "w", stderr);
  FILE *file = fopen(REDIRECT_FILE, "r");
  fread(buf, sizeof(char), BUFSIZ, file);
  CU_ASSERT(strncmp(buf, expected, strlen(expected) - 1) == 0);
  fclose(file);
}

int main()
{
  if (CU_initialize_registry() != CUE_SUCCESS)
    errx(EXIT_FAILURE, "can't initialize test registry");

  CU_pSuite processCsvSuite = CU_add_suite("processCsv", NULL, NULL);
  if (CU_get_error() != CUE_SUCCESS)
    errx(EXIT_FAILURE, "%s", CU_get_error_msg());

  CU_add_test(processCsvSuite,
              "processCsv_1_col_selected",
              test_processCsv_1_column_selected);

  CU_add_test(processCsvSuite,
              "processCsv_n_columns_selected",
              test_processCsv_n_columns_selected);

  CU_add_test(processCsvSuite,
              "processCsv_n_columns_arbitrarily_selected",
              test_processCsv_n_columns_arbitrarily_selected);

  CU_add_test(processCsvSuite,
              "processCsv_all_columns_selected",
              test_processCsv_all_columns_selected);

  CU_add_test(processCsvSuite,
              "processCsv_equal_filter",
              test_processCsv_equal_filter);

  CU_add_test(processCsvSuite,
              "processCsv_less_filter",
              test_processCsv_less_filter);

  CU_add_test(processCsvSuite,
              "processCsv_greater_filter",
              test_processCsv_greater_filter);

  CU_add_test(processCsvSuite,
              "processCsv_not_equal_filter",
              test_processCsv_not_equal_filter);

  CU_add_test(processCsvSuite,
              "processCsv_less_equal_filter",
              test_processCsv_equal_filter);

  CU_add_test(processCsvSuite,
              "processCsv_greater_equal_filter",
              test_processCsv_equal_filter);

  CU_add_test(processCsvSuite,
              "processCsv_multiple_filters",
              test_processCsv_multiple_filters);

  CU_add_test(processCsvSuite,
              "processCsv_multiple_arbitrary_filters",
              test_processCsv_multiple_arbitrary_filters);

  CU_add_test(processCsvSuite,
              "processCsv_multiple_filters_same_header",
              test_processCsv_multiple_filters_same_header);

  CU_add_test(processCsvSuite,
              "processCsv_filter_not_selected_column",
              test_processCsv_filter_not_selected_column);

  CU_add_test(processCsvSuite,
              "processCsv_select_header_not_found",
              test_processCsv_select_header_not_found);

  CU_add_test(processCsvSuite,
              "processCsv_filter_header_not_found",
              test_processCsv_filter_header_not_found);

  CU_add_test(processCsvSuite,
              "processCsv_invalid_filter",
              test_processCsv_invalid_filter);

  CU_basic_run_tests();
  CU_cleanup_registry();

  remove(REDIRECT_FILE);

  return EXIT_SUCCESS;
}
