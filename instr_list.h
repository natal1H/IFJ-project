#ifndef _INSTR_LIST_H

    #define _INSTR_LIST_H

    #include <stdlib.h>
    #include <string.h>

    typedef enum {
        I_UNDEFINED, // Špeciálny typ - po inicializácii pred nastavením skutočného typu

        I_HEADER, // Hlavička
        I_COMENT, // Komentár
        
        // Práca s rámcami, volanie funkcií
        I_MOVE,
        I_CREATEFRAME,
        I_PUSHFRAME,
        I_POPFRAME,
        I_DEFVAR,
        I_CALL,
        I_RETURN,
        // Práca s datovým zásobníkom
        I_PUSHS,
        I_POPS,
        I_CLEARS,
        // Aritmetické, relačné, booleovské a konverzné inštrukcie
        I_ADD, I_SUB, I_MUL, I_DIV, I_IDIV,
        I_ADDS, I_SUBS, I_MULS, I_DIVS, I_IDIVS,
        I_LT, I_GT, I_EQ,
        I_LTS, I_GTS, I_EQS,
        I_AND, I_OR, I_NOT,
        I_ANDS, I_ORS, I_NOTS,
        I_INT2FLOAT, I_FLOAT2INT, I_INT2CHAR, I_STRI2INT,
        I_INT2FLOATS, I_FLOAT2INTS, I_INT2CHARS, I_STRI2INTS,
        // Vstupno-výstupné inštrukcie
        I_READ,
        I_WRITE,
        I_CONCAT,
        I_STRLEN,
        I_GETCHAR,
        I_SETCHAR,
        // Práca s typy
        I_TYPE,
        // Inštrukcie pre riadenie toku programu
        I_LABEL,
        I_JUMP,
        I_JUMPIFEQ,
        I_JUMPIFNEQ,
        I_JUMPIFEQS, I_JUMPIFNEQS,
        // Ladiace inštrukcie
        I_BREAK,
        I_DPRINT
    } tInstruction_type;

    typedef struct {
        tInstruction_type instType;  // typ instrukce
        char *addr1; // adresa 1
        char *addr2; // adresa 2
        char *addr3; // adresa 3
    } tInstr;

    typedef struct listItem {
        tInstr *Instruction;
        struct listItem *nextItem;
    } tListItem;

    typedef struct {
        struct listItem *first;
        struct listItem *last;
        struct listItem *active;
    } tListOfInstr;

    void listInit(tListOfInstr *L);
    void listFree(tListOfInstr *L);
    void listFirst(tListOfInstr *L);
    void listNext(tListOfInstr *L);
    void listLast(tListOfInstr *L);
    tListItem *listGetActivePtr(tListOfInstr *L);
    tInstr *tInstr_create(tInstruction_type type, char *addr1, char *addr2, char *addr3);
    void tInstr_print_single_instruction(tInstr *I);
    void list_print_instructions(tListOfInstr *L);
    void tInst_free_instruction(tInstr *I);

    tInstr *tInstr_init();
    int tInstr_set_instruction(tInstr *instr, tInstruction_type type, char *addr1, char *addr2, char *addr3);

    void listInsertPostActive(tListOfInstr *L, tInstr *I);

#endif