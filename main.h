#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"

#define MAIN "main@function"

tBSTNodePtr global_table;
tBSTNodePtr actual_function_table;