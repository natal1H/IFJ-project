#include "code_gen.h"

// Funkcia na prípravu generovania kódu
int code_gen_start() {
    // inicializovať kam sa bude ukladať kód
    listInit(&instr_list);

    // Inicializovať štruktúru, do ktorej sa bude zapisovať aktuálna inštrukcia
    curr_instr = tInstr_init();

    // Vložiť prvú inštrukciu - obyčajný komentár
    return ERR_OK;
}

int code_gen_end() {

    // uvolniť štruktúru pre kód
    listFree(&instr_list);

    // uvoľniť štrukturú, v ktorej bola aktuálna inštrukcia
    tInst_free_instruction(curr_instr);

    return ERR_OK;
}

char *get_string_with_prefix(char *str, char *prefix) {
    char *str_complete = malloc(sizeof(char) * (strlen(str) + strlen(prefix)));
    if (str_complete == NULL) {
        // Chyba
        return NULL;
    }

    strcpy(str_complete, prefix);
    strcat(str_complete, str);

    return str_complete;
}

int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global) {
    if (var_name == NULL || symbol1 == NULL || symbol2 == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }

    // Pred var a pred symboly dať predponu
    char *var_complete = get_string_with_prefix(var_name, (global ? "GF@" : "LF@") );
    char *symbol1_complete = get_string_with_prefix(symbol1, "int@" );
    char *symbol2_complete = get_string_with_prefix(symbol2, "int@" );
    if (var_complete == NULL || symbol1_complete == NULL || symbol2_complete == NULL) {
        // Chyba pri alokácii
        return -1;
    }

    // Nastaviť správne aktuálnu inštrukciu
    tInstr_set_instruction(curr_instr, I_ADD, var_complete, symbol1_complete, symbol2_complete);

    // Vložiť inštrukciu do zoznamu
    listInsertPostActive(&instr_list, curr_instr);

    // Uvoľniť miesto, kde boli var_complete, symbol1_complete, symbol2_complete
    free(var_complete);
    free(symbol1_complete);
    free(symbol2_complete);

    return ERR_OK;
}

// test
int main() {
    printf("CODE GEN TEST\n");

    code_gen_start();

    tInstr *first_instr = tInstr_create(I_JUMP, "main", NULL, NULL); // len aby bola nejaká prvá inštrukcia
    listInsertLast(&instr_list, first_instr);
    listFirst(&instr_list);

    //gen_add_int("a", "10", "42", true);

    //tInstr_print_single_instruction(curr_instr);

    list_print_instructions(&instr_list);

    code_gen_end();

    return 0;
}