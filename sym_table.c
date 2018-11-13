#include "sym_table.h"

void BSTInitGlobal (tBSTNodeGlobalPtr *rootPtr) {
    (*rootPtr) = NULL;
}

void BSTInitLocal (tBSTNodeLocalPtr *rootPtr) {
    (*rootPtr) = NULL;
}

int BSTInsertGlobal (tBSTNodeGlobalPtr *rootPtr, char *id, tDataGlobalNode *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tBSTNodeGlobalPtr newPtr = (tBSTNodeGlobalPtr) malloc(sizeof(struct tBSTNodeGlobal));
        if (newPtr != NULL) {
            // Podarilo sa alokovať miesto

            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id));
            if (newPtr == NULL) {
                // Chyba
                // TODO: vrátiť chybový stav
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataGlobalNode *) malloc(sizeof(tDataGlobalNode));
            if (newPtr->data == NULL) {
                // Chyba
                // TODO: vrátiť chybový stav
            }
            // Uložiť obsah data
            newPtr->data->type = data->type;
            newPtr->data->defined = data->defined;
            newPtr->data->function_local_table = data->function_local_table;
            // TODO: pridať parametre

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            BSTInsertGlobal( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            BSTInsertGlobal( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->type = data->type;
            (*rootPtr)->data->function_local_table = data->function_local_table;
        }
    }


    return 0;
}

int BSTInsertLocal (tBSTNodeLocalPtr *rootPtr, char *id, tDataLocalNode *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tBSTNodeLocalPtr newPtr = (tBSTNodeLocalPtr) malloc(sizeof(struct tBSTNodeLocal));
        if (newPtr != NULL) {
            // Podarilo sa alokovať miesto

            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id));
            if (newPtr == NULL) {
                // Chyba
                // TODO: vrátiť chybový stav
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataLocalNode *) malloc(sizeof(tDataLocalNode));
            if (newPtr->data == NULL) {
                // Chyba
                // TODO: vrátiť chybový stav
            }
            // Uložiť obsah data
            newPtr->data->type = data->type;
            newPtr->data->defined = data->defined;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            BSTInsertLocal( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            BSTInsertLocal( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->type = data->type;
        }
    }

    return 0;
}


bool BSTSearchGlobal (tBSTNodeGlobalPtr rootPtr, char *id, tDataGlobalNode **data) {
    if (rootPtr == NULL) {
        return false;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            *data = rootPtr->data;
            return true;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (BSTSearchGlobal( rootPtr->lPtr, id, data ));
            }
            else {
                return (BSTSearchGlobal( rootPtr->rPtr, id, data ));
            }
        }
    }
}


bool BSTSearchLocal (tBSTNodeLocalPtr rootPtr, char *id, tDataLocalNode **data) {
    if (rootPtr == NULL) {
        return false;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            *data = rootPtr->data;
            return true;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (BSTSearchLocal(rootPtr->lPtr, id, data ));
            }
            else {
                return (BSTSearchLocal(rootPtr->rPtr, id, data ));
            }
        }
    }
}

void ReplaceByRightmostGlobal (tBSTNodeGlobalPtr ptrReplaced, tBSTNodeGlobalPtr *rootPtr) {
    if ((*rootPtr) == NULL) {
        return ;
    }

    if ((*rootPtr)->rPtr == NULL) {
        // Nie je už pravý podstrom - nahraď

        // Skopíruj obsah

        // Skopírovanie id
        ptrReplaced->id = realloc(ptrReplaced->id, sizeof(char) * strlen( (*rootPtr)->id) ); // reallocovaie miesta
        strcpy(ptrReplaced->id, (*rootPtr)->id);

        ptrReplaced->data = (*rootPtr)->data;

        tBSTNodeGlobalPtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        free(temp);
    }
    else {
        ReplaceByRightmostGlobal(ptrReplaced, &(*rootPtr)->rPtr);
    }
}


void ReplaceByRightmostLocal (tBSTNodeLocalPtr ptrReplaced, tBSTNodeLocalPtr *rootPtr) {
    if ((*rootPtr) == NULL) {
        return ;
    }

    if ((*rootPtr)->rPtr == NULL) {
        // Nie je už pravý podstrom - nahraď

        // Skopíruj obsah

        // Skopírovanie id
        ptrReplaced->id = realloc(ptrReplaced->id, sizeof(char) * strlen( (*rootPtr)->id) ); // reallocovaie miesta
        strcpy(ptrReplaced->id, (*rootPtr)->id);

        ptrReplaced->data = (*rootPtr)->data;

        tBSTNodeLocalPtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        free(temp);
    }
    else {
        ReplaceByRightmostLocal(ptrReplaced, &(*rootPtr)->rPtr);
    }
}


void BSTDeleteGlobal (tBSTNodeGlobalPtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        BSTDeleteGlobal(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        BSTDeleteGlobal(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tBSTNodeGlobalPtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tBSTNodeGlobalPtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            ReplaceByRightmostGlobal((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

void BSTDeleteLocal (tBSTNodeLocalPtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        BSTDeleteLocal(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        BSTDeleteLocal(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tBSTNodeLocalPtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tBSTNodeLocalPtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            ReplaceByRightmostLocal((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

void BSTDisposeGlobal (tBSTNodeGlobalPtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        BSTDisposeGlobal( &(*rootPtr)->lPtr );
        BSTDisposeGlobal( &(*rootPtr)->rPtr );

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

void BSTDisposeLocal (tBSTNodeLocalPtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        BSTDisposeLocal( &(*rootPtr)->lPtr );
        BSTDisposeLocal( &(*rootPtr)->rPtr );

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

// Pomocné funkcie - print

void Print_tree2_global(tBSTNodeGlobalPtr TempTree, char* sufix, char fromdir) {
    if (TempTree != NULL)
    {
        char* suf2 = (char*) malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L')
        {
            suf2 = strcat(suf2, "  |");
            printf("%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2_global(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2_global(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

void Print_tree_global(tBSTNodeGlobalPtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        Print_tree2_global(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}


void Print_tree2_local(tBSTNodeLocalPtr TempTree, char* sufix, char fromdir) {
    if (TempTree != NULL)
    {
        char* suf2 = (char*) malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);
        if (fromdir == 'L')
        {
            suf2 = strcat(suf2, "  |");
            printf("%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2_local(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2_local(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

void Print_tree_local(tBSTNodeLocalPtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        Print_tree2_local(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}