#include "instr_list.h"
#include "error.h"

#include <stdio.h>

void listInit(tListOfInstr *L) {
    L->first  = NULL;
    L->last   = NULL;
    L->active = NULL;
}

void listFree(tListOfInstr *L) {
    tListItem *ptr;
    while (L->first != NULL)
    {
        ptr = L->first;
        L->first = L->first->nextItem;

        // Uvolníme inštrukciu v ptr
        tInst_free_instruction(ptr->Instruction);

        // uvolnime celu polozku
        free(ptr);
    }
}

void listInsertPostActive(tListOfInstr *L, tInstr *I) {
    /*
    if (L->active == NULL) {
        // Chyba - nevie, kde pridať
        return ;
    }
    */

    tListItem *newItem;
    newItem = malloc(sizeof (tListItem));

    newItem->Instruction = tInstr_create(I->instType, I->addr1, I->addr2, I->addr3);
    if (newItem->Instruction == NULL) {
        // Chyba
        return ;
    }

    if (L->active == NULL) {
        printf("TU SOM!\n");
        // Prázdny zoznam ešte, tak nech pridá na začiatok
        L->first = newItem;
        L->first->nextItem = NULL;
        L->last = L->first;
        listFirst(L);
    }
    else if (L->active == L->last) {
        // Vložiť za poslednú
        L->last->nextItem=newItem;
        L->last = newItem;
        listNext(L);
    }
    else {
        // Vložiť niekde do stredu zoznamu
        newItem->nextItem = L->active->nextItem;
        L->active->nextItem = newItem;
        listNext(L);
    }
}

void listInsertLast(tListOfInstr *L, tInstr *I) {
    tListItem *newItem;
    newItem = malloc(sizeof (tListItem));

    newItem->Instruction = tInstr_create(I->instType, I->addr1, I->addr2, I->addr3);
    if (newItem->Instruction == NULL) {
        // Chyba
        return ;
    }

    newItem->nextItem = NULL;
    if (L->first == NULL)
        L->first = newItem;
    else
        L->last->nextItem=newItem;

    L->last=newItem;
}

void listFirst(tListOfInstr *L) {
    L->active = L->first;
}

void listNext(tListOfInstr *L) {
    if (L->active != NULL)
        L->active = L->active->nextItem;
}

void listGoto(tListOfInstr *L, void *gotoInstr) {
    L->active = (tListItem*) gotoInstr;
}

void *listGetPointerLast(tListOfInstr *L) {
    return (void*) L->last;
}

tInstr *listGetData(tListOfInstr *L) {
    if (L->active == NULL) {
        return NULL;
    }
    else return (L->active->Instruction);
}

tInstr *tInstr_init() {
    // Alokovanie miesta
    tInstr *i = (tInstr *) malloc(sizeof(tInstr));
    if (i == NULL) {
        return NULL;
    }

    // Nastavenie typu
    i->instType = I_UNDEFINED;

    // Nastavenie počiatočných adries na NULL
    i->addr1 = NULL;
    i->addr2 = NULL;
    i->addr3 = NULL;

    return i;
}

tInstr *tInstr_create(tInstruction_type type, char *addr1, char *addr2, char *addr3) {

    // Alokovanie miesta
    tInstr *i = (tInstr *) malloc(sizeof(tInstr));
    if (i == NULL) {
        return NULL;
    }

    // Nastavenie typu
    i->instType = type;

    // Nastavenie addr1
    if (addr1 != NULL) {
        i->addr1 = malloc(sizeof(char) * strlen(addr1));
        if (i->addr1 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr1, addr1);
    }
    else
        i->addr1 = NULL;

    // Nastavenie addr2
    if (addr2 != NULL) {
        i->addr2 = malloc(sizeof(char) * strlen(addr2));
        if (i->addr2 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr2, addr2);
    }
    else
        i->addr2 = NULL;

    // Nastavenie addr3
    if (addr3 != NULL) {
        i->addr3 = malloc(sizeof(char) * strlen(addr3));
        if (i->addr3 == NULL) {
            // Chyba
            return NULL;
        }
        strcpy(i->addr3, addr3);
    }
    else
        i->addr3 = NULL;

    return i;
}

void tInstr_print_single_instruction(tInstr *I) {
    if (I == NULL) {
        return;
    }

    if (I->instType == I_HEADER) {
        printf(".IFJcode18\n");
        return ;
    }
    else if (I->instType == I_COMENT) {
        printf("%s\n", I->addr1);
        return ;
    }

    // Vypíš opcode
    switch (I->instType) {
        case I_MOVE:
            printf("%s", "MOVE");
            break;
        case I_CREATEFRAME:
            printf("%s", "CREATEFRAME");
            break;
        case I_PUSHFRAME:
            printf("%s", "PUSHFRAME");
            break;
        case I_POPFRAME:
            printf("%s", "POPFRAME");
            break;
        case I_DEFVAR:
            printf("%s", "DEFVAR");
            break;
        case I_CALL:
            printf("%s", "CALL");
            break;
        case I_RETURN:
            printf("%s", "RETURN");
            break;
        case I_PUSHS:
            printf("%s", "PUSHS");
            break;
        case I_POPS:
            printf("%s", "POPS");
            break;
        case I_CLEARS:
            printf("%s", "CLEARS");
            break;
        case I_ADD:
            printf("%s", "ADD");
            break;
        case I_SUB:
            printf("%s", "SUB");
            break;
        case I_MUL:
            printf("%s", "MUL");
            break;
        case I_DIV:
            printf("%s", "DIV");
            break;
        case I_IDIV:
            printf("%s", "IDIV");
            break;
        case I_ADDS:
            printf("%s", "ADDS");
            break;
        case I_SUBS:
            printf("%s", "SUBS");
            break;
        case I_MULS:
            printf("%s", "MULS");
            break;
        case I_DIVS:
            printf("%s", "DIVS");
            break;
        case I_IDIVS:
            printf("%s", "IDIVS");
            break;
        case I_LT:
            printf("%s", "LT");
            break;
        case I_GT:
            printf("%s", "GT");
            break;
        case I_EQ:
            printf("%s", "EQ");
            break;
        case I_LTS:
            printf("%s", "LTS");
            break;
        case I_GTS:
            printf("%s", "GTS");
            break;
        case I_EQS:
            printf("%s", "EQS");
            break;
        case I_AND:
            printf("%s", "AND");
            break;
        case I_OR:
            printf("%s", "OR");
            break;
        case I_NOT:
            printf("%s", "NOT");
            break;
        case I_ANDS:
            printf("%s", "ANDS");
            break;
        case I_ORS:
            printf("%s", "ORS");
            break;
        case I_NOTS:
            printf("%s", "NOTS");
            break;
        case I_INT2FLOAT:
            printf("%s", "INT2FLOAT");
            break;
        case I_FLOAT2INT:
            printf("%s", "FLOAT2INT");
            break;
        case I_INT2CHAR:
            printf("%s", "INT2CHAR");
            break;
        case I_STRI2INT:
            printf("%s", "STRI2INT");
            break;
        case I_INT2FLOATS:
            printf("%s", "INT2FLOATS");
            break;
        case I_FLOAT2INTS:
            printf("%s", "FLOAT2INTS");
            break;
        case I_INT2CHARS:
            printf("%s", "INT2CHARS");
            break;
        case I_STRI2INTS:
            printf("%s", "STRI2INTS");
            break;
        case I_READ:
            printf("%s", "READ");
            break;
        case I_WRITE:
            printf("%s", "WRITE");
            break;
        case I_CONCAT:
            printf("%s", "CONCAT");
            break;
        case I_STRLEN:
            printf("%s", "STRLEN");
            break;
        case I_GETCHAR:
            printf("%s", "GETCHAR");
            break;
        case I_SETCHAR:
            printf("%s", "SETCHAR");
            break;
        case I_TYPE:
            printf("%s", "TYPE");
            break;
        case I_LABEL:
            printf("%s", "LABEL");
            break;
        case I_JUMP:
            printf("%s", "JUMP");
            break;
        case I_JUMPIFEQ:
            printf("%s", "JUMPIFEQ");
            break;
        case I_JUMPIFNEQ:
            printf("%s", "JUMPIFNEQ");
            break;
        case I_JUMPIFEQS:
            printf("%s", "JUMPIFEQS");
            break;
        case I_JUMPIFNEQS:
            printf("%s", "JUMPIFNEQS");
            break;
        case I_BREAK:
            printf("%s", "BREAK");
            break;
        case I_DPRINT:
            printf("%s", "DPRINT");
            break;
    }

    // Vypíš addr1
    if (I->addr1 != NULL) {
        printf(" %s", I->addr1);
    }

    // Vypíš addr2
    if (I->addr2 != NULL) {
        printf(" %s", I->addr2);
    }

    // Vypíš addr3
    if (I->addr3 != NULL) {
        printf(" %s", I->addr3);
    }

    printf("\n");
}

void list_print_instructions(tListOfInstr *L) {
    if (L == NULL) {
        return ;
    }

    tListItem *temp = L->first;
    while (temp != NULL) {
        tInstr_print_single_instruction(temp->Instruction);
        temp = temp->nextItem;
    }
}

void tInst_free_instruction(tInstr *I) {
    if (I != NULL) {
        free(I->addr1);
        free(I->addr2);
        free(I->addr3);
        free(I);
    }
}

int tInstr_set_instruction(tInstr *instr, tInstruction_type type, char *addr1, char *addr2, char *addr3) {
    if (instr == NULL) {
        return -1; // TODO vhodný chybový kód dať
    }

    if (addr1 != NULL)
        printf("SI adr1: %s\n", addr1);
    if (addr2 != NULL)
        printf("SI adr2: %s\n", addr2);
    if (addr3 != NULL)
        printf("SI adr3: %s\n", addr3);

    // Nastavenie nového typu
    instr->instType = type;

    // Nastavenie addr1
    if (addr1 != NULL) {
        // Pozrieť či predtým niečo bolo na addr1
        if (instr->addr1 != NULL) {
            // uvoľniť a znovu alokovať
            // TODO - lepšie použiť realloc?
            free(instr->addr1);
            //instr->addr1 = realloc(instr->addr1, sizeof(char) * strlen(addr1));
        }
        //else {
        instr->addr1 = malloc(sizeof(char) * strlen(addr1));
        if (instr->addr1 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return -1; // TODO vhodný chybový kód dať
        }
        //}
        printf("Kopírovanie -%s-\n", addr1);
        strcpy(instr->addr1, addr1);
        printf("Post copy: -%s- -%s-\n", instr->addr1, addr1);
    }
    else {
        if (instr->addr1 != NULL) {
            free(instr->addr1);
        }
        instr->addr1 = NULL;
    }
printf("SI a adr1: %s\n", instr->addr1);
    // Nastavenie addr2
    if (addr2 != NULL) {
        // Pozrieť či predtým niečo bolo na addr2
        if (instr->addr2 != NULL) {
            // uvoľniť a znovu alokovať
            // TODO - lepšie použiť realloc?
            free(instr->addr2);
            //instr->addr2 = realloc(instr->addr2, sizeof(char) * strlen(addr2));
        }
        //else {
        instr->addr2 = malloc(sizeof(char) * strlen(addr2));
        if (instr->addr2 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return -1; // TODO vhodný chybový kód dať
        }
        //}
        strcpy(instr->addr2, addr2);
    }
    else {
        if (instr->addr2 != NULL) {
            free(instr->addr2);
        }
        instr->addr2 = NULL;
    }
printf("SI b adr1: %s\n", instr->addr1);
    // Nastavenie addr3
    if (addr3 != NULL) {
        // Pozrieť či predtým niečo bolo na addr3
        if (instr->addr3 != NULL) {
            // uvoľniť a znovu alokovať
            // TODO - lepšie použiť realloc?
            free(instr->addr3);
            //instr->addr3 = realloc(instr->addr3, sizeof(char) * strlen(addr3));
        }
        //else {
        instr->addr3 = malloc(sizeof(char) * strlen(addr3));
        if (instr->addr3 == NULL) {
            // Chyba - nepodarilo sa alokovať
            return -1; // TODO vhodný chybový kód dať
        }
        //}
        strcpy(instr->addr3, addr3);
    }
    else {
        if (instr->addr3 != NULL) {
            free(instr->addr3);
        }
        instr->addr3 = NULL;
    }

    if (addr1 != NULL)
        printf("SI END adr1: %s\n", instr->addr1);
    if (addr2 != NULL)
        printf("SI END adr2: %s\n", instr->addr2);
    if (addr3 != NULL)
        printf("SI END adr3: %s\n", instr->addr3);
    printf("SI_END\n");

    return ERR_OK;
}

/*
int main() {
    tInstr *instr1 = tInstr_create(I_MOVE, "var1", "symbol1", NULL);
    tInstr_print_single_instruction(instr1);

    tInstr *instr2 = tInstr_create(I_READ, "var2", "symbol2", "symbol3");
    tInstr_print_single_instruction(instr2);


    tListOfInstr L;
    listInit(&L);
    listInsertLast(&L, instr1);
    listInsertLast(&L, instr2);

    list_print_instructions(&L);

    listFree(&L);
    tInst_free_instruction(instr1);
    tInst_free_instruction(instr2);

    return 0;
}
 */