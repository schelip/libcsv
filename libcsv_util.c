#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "libcsv_util.h"

Csv *createCsv()
{
  Csv *csv = (Csv *)malloc(sizeof(Csv));
  csv->cells = NULL;
  csv->columns = NULL;
  csv->rowCount = 0;
  csv->colCount = 0;

  return csv;
}

void addColumn(Csv *csv, const char header[], const bool isSelected)
{
  csv->columns = (Column **)realloc(csv->columns, (csv->colCount + 1) * sizeof(Column *));
  Column *column = (Column *)malloc(sizeof(Column));
  column->header = strdup(header);
  column->isSelected = isSelected;
  csv->columns[csv->colCount++] = column;
}

void addRow(Csv *csv)
{
  csv->cells = (char ***)realloc(csv->cells, (csv->rowCount + 1) * sizeof(char **));

  csv->cells[csv->rowCount] = (char **)malloc(csv->colCount * sizeof(char *));
  char **row = csv->cells[csv->rowCount];

  for (size_t i = 0; i < csv->colCount; i++)
    row[i] = NULL;

  csv->rowCount++;
}

void setCell(Csv *csv, size_t row, size_t col, const char value[])
{
  if (row >= csv->rowCount || col >= csv->colCount)
    return;

  csv->cells[row][col] = strdup(value);
}

char *getCell(Csv *csv, size_t row, size_t col)
{
  if (row >= csv->rowCount || col >= csv->colCount)
    return NULL;

  return csv->cells[row][col];
}

void deleteRow(Csv *csv, size_t row)
{
  if (row >= csv->rowCount)
    return;

  char **rowData = csv->cells[row];

  for (size_t i = 0; i < csv->colCount; i++)
    if (rowData[i] != NULL)
      free(rowData[i]);

  free(rowData);
  csv->rowCount--;

  for (size_t i = row + 1; i < csv->rowCount; i++)
    csv->cells[i] = csv->cells[i + 1];

  csv->cells = (char ***)realloc(csv->cells, csv->rowCount * sizeof(char **));
}

void freeCsv(Csv *csv)
{
  for (size_t i = 0; i < csv->colCount; i++)
    free(csv->columns[i]);

  free(csv->columns);

  for (size_t i = 0; i < csv->rowCount; i++)
  {
    for (size_t j = 0; j < csv->colCount; j++)
      free(csv->cells[i][j]);

    free(csv->cells[i]);
  }

  free(csv->cells);
  free(csv);
}

void printCsv(Csv *csv)
{
  for (size_t i = 0; i < csv->colCount; i++)
  {
    if (!csv->columns[i]->isSelected)
      continue;

    if (i)
      printf(",");
    printf("%s", csv->columns[i]->header);
  }
  printf("\n");

  for (size_t i = 0; i < csv->rowCount; i++)
  {
    for (size_t j = 0; j < csv->colCount; j++)
    {
      if (!csv->columns[j]->isSelected)
        continue;

      if (j)
        printf(",");
      printf("%s", csv->cells[i][j]);
    }
    printf("\n");
  }
}

RowFilter *createRowFilter(size_t column, enum operator op, const char *value)
{
  RowFilter *rowFilter = (RowFilter *)malloc(sizeof(RowFilter));
  rowFilter->column = column;
  rowFilter->op = op;
  rowFilter->value = strdup(value);
  return rowFilter;
}
