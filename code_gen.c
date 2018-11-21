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

int gen_add_int(char *var_name, char *symbol1, char *symbol2, bool global) {
    if (var_name == NULL || symbol1 == NULL || symbol2 == NULL) {
        // Chyba
        return -1; // TODO nájsť vhodný chybový kód
    }

    // Pred var a pred symboly dať predponu
    // alokovať miesto pre reťazce aj s predponami
    char *var_complete = malloc(sizeof(char) * (strlen(var_name) + VAR_PREFIX_LEN));
    char *symbol1_complete = malloc(sizeof(char) * (strlen(symbol1) + INT_PREFIX_LEN));
    char *symbol2_complete = malloc(sizeof(char) * (strlen(symbol2) + INT_PREFIX_LEN));
    if (var_complete == NULL || symbol1_complete == NULL || symbol2_complete == NULL) {
        // Chyba pri alokácii
        return -1;
    }
    // Pridanie predpôn
    // VAR NAME
    if (global) {
        // Global Frame - GF
        strcpy(var_complete, "GF@");
    }
    else {
        // Local Frame - LF
        strcpy(var_complete, "LF@");
    }
    strcat(var_complete, var_name);
    // SYMBOL 1
    strcpy(symbol1_complete, "int@");
    strcat(symbol1_complete, symbol1);
    // SYMBOL 2
    strcpy(symbol2_complete, "int@");
    strcat(symbol2_complete, symbol2);

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

    gen_add_int("a", "10", "42", true);

    tInstr_print_single_instruction(curr_instr);

    list_print_instructions(&instr_list);

    code_gen_end();

    return 0;
}