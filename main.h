#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "semantic_analysis.h"
#include "stringlib.h"

#define MAIN "main@function"

tGlobalTableNodePtr global_table; // Globálna tabuľka symbolov, kde sú v jednotlivých uzloch uložené užívateľom definované funkcie podľa ich ID
tLocalTableNodePtr actual_function_table; // Tabuľka symbolov aktuálnej funkcie, v ktorej sa program nachádza (buď nejaká užívateľom definovaná funkcia alebo "main")
tLocalTableNodePtr * actual_function_ptr; // Tabuľka symbolov aktuálnej funkcie, v ktorej sa program nachádza (buď nejaká užívateľom definovaná funkcia alebo "main")
char *actual_function_name; // ID funkcie, v ktorej sa práve program nachádza