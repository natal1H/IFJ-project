#ifndef  _MAIN_H
    #define _MAIN_H

    #include <stdio.h>
    #include "scanner.h"
    #include "parser.h"
    #include "symtable.h"
    #include "semantic_analysis.h"
    #include "stringlib.h"
    #include "expression_parser.h"

    #define MAIN "main@function"

    tGlobalTableNodePtr global_table; // Globálna tabuľka symbolov, kde sú v jednotlivých uzloch uložené užívateľom definované funkcie podľa ich ID
    //tLocalTableNodePtr actual_function_table; // Tabuľka symbolov aktuálnej funkcie, v ktorej sa program nachádza (buď nejaká užívateľom definovaná funkcia alebo "main")
    tLocalTableNodePtr * actual_function_ptr; // Tabuľka symbolov aktuálnej funkcie, v ktorej sa program nachádza (buď nejaká užívateľom definovaná funkcia alebo "main")
    char *actual_function_name; // ID funkcie, v ktorej sa práve program nachádza
    char *id_copy; // Reťazec na zálohu ID aj po načítaní ďalšich tokenov

#endif