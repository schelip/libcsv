#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "libcsv_util.h"

/**
 * Exit program and print error message for header not found.
 *
 * @param header The header that was not found.
 */
static void headerNotFound(const char header[])
{
  fprintf(stderr, "Header '%s' not found in CSV file/string\n", header);
}

/**
 * Select specific columns from a string with columns, adding a column in the CSV
 * structure for each header with isSelected set.
 *
 * @param csvHeaders String with the columns to select from.
 * @param selectedColumns String with the columns which will be selected.
 * @param csv The CSV structure in which the columns will be allocated.
 * @param success Will be set as true if the operation was successful.
 */
static void addColumns(
    char csvHeaders[],
    const char selectedColumns[],
    Csv *csv,
    bool *success)
{
  if (*selectedColumns)
    for (char *selectedHeader = strtok(strdup(selectedColumns), VALUE_SEPARATOR);
         selectedHeader != NULL;
         selectedHeader = strtok(NULL, VALUE_SEPARATOR))
      if (!strstr(csvHeaders, selectedHeader))
      {
        headerNotFound(selectedHeader);
        *success = false;
        return;
      }

  for (char *header = strtok(csvHeaders, VALUE_SEPARATOR);
       header != NULL;
       header = strtok(NULL, VALUE_SEPARATOR))
    addColumn(csv, header, !*selectedColumns || strstr(selectedColumns, header));

  *success = true;
}

/**
 * Search a CSV structure for a column with a specificied header.
 *
 * @param csv The CSV structure to search in.
 * @param header The header to be searched for.
 * @param success Will be set as true if the operation was successful.
 * @return size_t The column index of the column containing the header.
 */
static size_t getColumn(const Csv *csv, const char header[], bool *success)
{
  for (size_t i = 0; i < csv->colCount; i++)
    if (strcmp(csv->columns[i]->header, header) == 0)
    {
      *success = true;
      return i;
    }

  *success = false;
  headerNotFound(header);
  return 0;
}

/**
 * Create RowFilter structures from a definitions string.
 *
 * @param rowFilterDefinitions A string containing one or more row filter definitions.
 * @param csv The CSV structure containing the columns which will be filtered.
 * @param rowFilters Array which will contain the constructed structures.
 * @param success Will be set as true if the operation was successful.
 * @return size_t How many filters were constructed.
 */
static size_t defineRowFilters(
    const char rowFilterDefinitions[],
    const Csv *csv,
    RowFilter *rowFilters[],
    bool *success)
{
  size_t totalRowFilters = 0;

  for (char *rowFilterDefinition = strtok(strdup(rowFilterDefinitions), LINE_SEPARATOR);
       rowFilterDefinition != NULL;
       rowFilterDefinition = strtok(NULL, LINE_SEPARATOR))
  {
    enum operator op;
    for (size_t i = strlen(rowFilterDefinition) - 1; i > 0; --i)
    {
      switch (rowFilterDefinition[i])
      {
      case '=':
        switch (rowFilterDefinition[i - 1])
        {
        case '!':
          op = NOT_EQUAL;
          break;
        case '<':
          op = LESS_EQUAL;
          break;
        case '>':
          op = GREATER_EQUAL;
          break;
        default:
          op = EQUAL;
        }
        if (op != EQUAL)
          rowFilterDefinition[i - 1] = '\0';
        break;
      case '<':
        op = LESS;
        break;
      case '>':
        op = GREATER;
        break;
      default:
        op = 0;
      }

      if (op)
      {
        rowFilterDefinition[i] = '\0';

        size_t col = getColumn(csv, rowFilterDefinition, success);

        if (!*success)
          return 0;

        rowFilters[totalRowFilters++] = createRowFilter(
            col,
            op,
            &rowFilterDefinition[i + 1]);
        break;
      }
    }

    if (!op)
    {
      fprintf(stderr, "Invalid filter: '%s'\n", rowFilterDefinition);
      *success = false;
      return 0;
    }
  }

  return totalRowFilters;
}

/**
 * Validate whether a given cell respects the filter for its column.
 *
 * If there are multiple filters for the same column, and any of them are true, it
 * is considered a successful validation.
 *
 * Also, if no filters were found for the column, it is considered successful.
 *
 * @param cell The cell with the value to be validated.
 * @param rowFilters Array with the filters to the CSV.
 * @param totalRowFilters How many row filters there are in the array.
 * @param col The column which is being verified
 * @return bool Whether the column is valid for the filters or not.
 */
static bool validateFilters(
    const char cell[],
    RowFilter *rowFilters[],
    size_t totalRowFilters,
    size_t col)
{
  bool hasFilter = false;
  for (size_t i = 0; i < totalRowFilters; i++)
    if (rowFilters[i]->column == col)
    {
      hasFilter = true;
      int comparison = strcmp(cell, rowFilters[i]->value);
      switch (rowFilters[i]->op)
      {
      case EQUAL:
        if (comparison == 0)
          return true;
        break;
      case LESS:
        if (comparison < 0)
          return true;
        break;
      case GREATER:
        if (comparison > 0)
          return true;
        break;
      case NOT_EQUAL:
        if (comparison != 0)
          return true;
        break;
      case LESS_EQUAL:
        if (comparison <= 0)
          return true;
        break;
      case GREATER_EQUAL:
        if (comparison >= 0)
          return true;
        break;
      }
    }
  return !hasFilter;
}

/**
 * Add cells from a row string to a CSV structure, considering row filters.
 *
 * @param csvRow The CSV row string.
 * @param resultCsv The resulting CSV structure.
 * @param rowFilters Array of filters to be validated for each row.
 * @param totalRowFilters How many row filters there are in the array.
 */
static void addFilteredRow(
    char csvRow[],
    Csv *csv,
    RowFilter *rowFilters[],
    size_t totalRowFilters)
{
  addRow(csv);
  size_t col = 0;

  char *cell = csvRow;
  char *cellEnd;
  do
  {
    cellEnd = strchr(cell, *VALUE_SEPARATOR);
    if (cellEnd != NULL)
      *cellEnd = '\0';

    if (!validateFilters(cell, rowFilters, totalRowFilters, col))
    {
      deleteRow(csv, csv->rowCount - 1);
      break;
    }

    if (csv->columns[col]->isSelected)
      setCell(csv, csv->rowCount - 1, col, cell);

    col++;

    if (cellEnd != NULL)
      cell = cellEnd + 1;
  } while (cellEnd != NULL);
}

void processCsv(
    const char csv[],
    const char selectedColumns[],
    const char rowFilterDefinitions[])
{
  Csv *resultCsv = createCsv();
  bool success;

  char *csvHeaders = strtok(strdup(csv), LINE_SEPARATOR);
  addColumns(csvHeaders, selectedColumns, resultCsv, &success);

  if (!success)
  {
    freeCsv(resultCsv);
    return;
  }

  RowFilter *rowFilters[MAX_CSV_COLS] = {NULL};
  size_t totalRowFilters = defineRowFilters(
      rowFilterDefinitions,
      resultCsv,
      rowFilters,
      &success);

  if (!success)
  {
    freeCsv(resultCsv);
    return;
  }

  strtok(strdup(csv), LINE_SEPARATOR); // Ignore header row
  for (char *csvRow = strtok(NULL, LINE_SEPARATOR);
       csvRow != NULL;
       csvRow = strtok(NULL, LINE_SEPARATOR))
    addFilteredRow(csvRow, resultCsv, rowFilters, totalRowFilters);

  printCsv(resultCsv);

  for (size_t i = 0; i < totalRowFilters; i++)
    free(rowFilters[i]);
  freeCsv(resultCsv);
}

void processCsvFile(
    const char csvFilePath[],
    const char selectedColumns[],
    const char rowFilterDefinitions[])
{
  Csv *resultCsv = createCsv();
  bool success;

  FILE *csvFile = fopen(csvFilePath, "r");
  char buffer[BUFSIZ];
  size_t currLen = 0, addLen;

  if (!csvFile)
  {
    freeCsv(resultCsv);
    return;
  }

  char rowPattern[5] = "%s";
  strcat(rowPattern, LINE_SEPARATOR);

  char *csvHeaders = (char *)malloc(0);
  do
  {
    fgets(buffer, BUFSIZ, csvFile);
    addLen = strlen(buffer);
    csvHeaders = (char *)realloc(csvHeaders, currLen + addLen);
    strcpy(&csvHeaders[currLen], buffer);
    currLen += addLen;
  } while (csvHeaders[currLen - 1] != '\n');
  csvHeaders[currLen - 1] = '\0';

  addColumns(csvHeaders, selectedColumns, resultCsv, &success);

  if (!success)
  {
    freeCsv(resultCsv);
    return;
  }

  free(csvHeaders);

  RowFilter *rowFilters[MAX_CSV_COLS] = {NULL};
  size_t totalRowFilters = defineRowFilters(
      rowFilterDefinitions,
      resultCsv,
      rowFilters,
      &success);

  if (!success)
  {
    freeCsv(resultCsv);
    return;
  }

  bool eof = false;
  while (!eof)
  {
    char *csvRow = (char *)malloc(0);
    currLen = 0;
    do
    {
      if ((eof = !fgets(buffer, BUFSIZ, csvFile)))
        break;

      addLen = strlen(buffer);
      csvRow = (char *)realloc(csvRow, currLen + addLen);
      strcpy(&csvRow[currLen], buffer);
      currLen += addLen;
    } while (csvRow[currLen - 1] != '\n');
    csvRow[currLen - 1] = '\0';

    if (!eof)
      addFilteredRow(csvRow, resultCsv, rowFilters, totalRowFilters);

    free(csvRow);
  }

  printCsv(resultCsv);

  for (size_t i = 0; i < totalRowFilters; i++)
    free(rowFilters[i]);
  freeCsv(resultCsv);

  fclose(csvFile);
}
