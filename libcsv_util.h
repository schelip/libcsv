#include <stdbool.h>

#define MAX_CSV_COLS 256
#define VALUE_SEPARATOR ","
#define LINE_SEPARATOR "\n"

typedef struct
{
  char *header;
  bool isSelected;
} Column;

typedef struct
{
  Column **columns;
  size_t colCount;
  char ***cells;
  size_t rowCount;
} Csv;

enum operator
{
  EQUAL = 1,
  LESS = 2,
  GREATER = 3,
  NOT_EQUAL = 4,
  LESS_EQUAL = 5,
  GREATER_EQUAL = 6
};

typedef struct
{
  size_t column;
  enum operator op;
  char *value;
} RowFilter;

/**
 * Create and allocate memory to a new CSV data structure.
 *
 * @param colCount How many columns are to be allocated.
 *
 * @return Csv* The created CSV structure.
 */
Csv *createCsv();

/**
 * Allocate memory to a new column in the CSV.
 *
 * @param csv The CSV in which a new column will be allocated.
 * @param header The header of the new column.
 * @param isSelected Wheter the column is selected or not.
 */
void addColumn(Csv *csv, const char header[], const bool isSelected);

/**
 * Allocate memory to a new row in the CSV.
 *
 * @param csv The CSV in which a new row will be allocated.
 */
void addRow(Csv *csv);

/**
 * Set the value of CSV cell.
 *
 * @param csv The CSV containing the cell.
 * @param row The row index of the cell.
 * @param col The column index of the cell.
 * @param value The new value of the cell.
 */
void setCell(Csv *csv, size_t row, size_t col, const char value[]);

/**
 * Get the value of a CSV cell.
 *
 * @param csv The CSV containing the cell.
 * @param row The row index of the cell.
 * @param col The column index of the cell.
 *
 * @return char* The value of the cell.
 */
char *getCell(Csv *csv, size_t row, size_t col);

/**
 * Remove and free a row of a CSV.
 *
 * @param csv The CSV containing the row.
 * @param row The index of the row to be deleted.
 */
void deleteRow(Csv *csv, size_t row);

/**
 * Free the memory allocated for a CSV.
 *
 * @param csv The CSV to be freed.
 */
void freeCsv(Csv *csv);

/**
 * Print a CSV to stdout.
 *
 * @param csv The CSV to be printed.
 */
void printCsv(Csv *csv);

/**
 * Create and allocate memory for a RowFilter structure.
 *
 * @param column The index of the column that will be filtered
 * @param op The operator of the filter
 * @param value The value that the filter is comparing to.
 * @return RowFilter* The created RowFilter structure.
 */
RowFilter *createRowFilter(size_t column, enum operator op, const char *value);
