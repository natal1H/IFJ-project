#include "code_gen.h"

// Funkcia na prípravu generovania kódu
int code_gen_start() {
    // inicializovať kam sa bude ukladať kód
    /*
    code = tstring_struct_initialize();
    if (code == NULL) {
        // Chyba pri inicializácii
        return -1;
    }
    */

    return 0;
}

int code_gen_end() {

    // uvolniť štruktúru pre kód
    tstring_free_struct(code);

    return 0;
}

// test
int main() {
    printf("CODE GEN TEST\n");

    code_gen_start();

    code_gen_end();

    return 0;
}