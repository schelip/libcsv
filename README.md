# libcsv

CSV processing library implementation written in C. Allows selecting columns and filtering
rows by lexicographical value. The public functions are defined in the [libcsv](libcsv.h) header file.

## Usage

Compiling and running the unit tests:

```bash
$ gcc libcsv_test.c libcsv.c libcsv_util.c -o libcsv_test -lcunit
$ ./libcsv_test
```

Compiling the library as a shared object:
```bash
$ gcc -shared -o libcsv.so -fPIC libcsv.c libcsv_util.c
```

A docker file is provided to run an alpine linux container with the tests binary and the library shared object.

Usage example:

```C
const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
processCsv(csv, "header1,header3", "header1>1\nheader3<8");

// header1,header3
// 4,6
```

- The CSV to be processed can be a string or provided from a file
- The result will be printed in `stdout`
- Up to 256 comma-separated unique columns with arbitrary length are supported
- Selecting columns will hide every other column from the result
- Columns may be selected in arbitrary order, result will alway follow the original CSV order
- The first row defines the headers, other rows separated by `\n` will be the values
- Rows can be filtered using the `!` `!=` `>` `<` `>=` `<=` operators with column headers
- Filtered headers can appear in any order
- Multiple filters for the same header will behave as `OR`
- Multiple filters for different headers will behave as `AND`
- No headers that don't exist can be used in selection or filtering

## TODO

- Unit tests for `processCsvFile`
- Filtering by comparison between columns
- Ordering feature
