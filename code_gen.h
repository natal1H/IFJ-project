#ifndef _CODE_GEN_H

    #define _CODE_GEN_H

    #define VAR_PREFIX_LEN 3 // GF@ / LF@ / TF@
    #define INT_PREFIX_LEN 4 // int@

    #include <stdbool.h>
    #include <stdio.h>

    #include "stringlib.h"
    #include "error.h"
    #include "instr_list.h"

    // štruktúra - zoznam inštrukcií
    tListOfInstr instr_list;

    // Pomocná štruktúra - aktuálna inštrukcia
    tInstr *curr_instr;

    // Funkcia na prípravu generovania kódu
    int code_gen_start();

    // Funkcia, ktorá uprace po generovaní kódu
    int code_gen_end();

    // Pomocné funkcie pre generovanie rôznych situácií
    int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global);

#endif