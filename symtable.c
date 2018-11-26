#include "symtable.h"

// Funkcie pre globálnu TS
void global_table_init(tGlobalTableNodePtr *rootPtr) {
    (*rootPtr) = NULL;
}

int global_table_insert(tGlobalTableNodePtr *rootPtr, char *id, tDataNodeGlobal *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tGlobalTableNodePtr newPtr = (tGlobalTableNodePtr) malloc(sizeof(struct tGlobalTableNode));
        if (newPtr != NULL) {
            // Podarilo sa alokovať miesto
            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id));
            if (newPtr == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
            if (newPtr->data == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }

            // Uložiť obsah data
            newPtr->data->defined = data->defined;
            newPtr->data->function_table = data->function_table;
            newPtr->data->params = data->params;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            global_table_insert( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            global_table_insert( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->function_table = data->function_table;
            (*rootPtr)->data->params = data->params;
        }
    }

    return ERR_OK;
}

bool global_table_search(tGlobalTableNodePtr rootPtr, char *id, tDataNodeGlobal **data) {
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
                return (global_table_search( rootPtr->lPtr, id, data ));
            }
            else {
                return (global_table_search( rootPtr->rPtr, id, data ));
            }
        }
    }
}

void global_table_replace_by_rightmost(tGlobalTableNodePtr ptrReplaced, tGlobalTableNodePtr *rootPtr) {
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

        tGlobalTableNodePtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        free(temp->data);
        free(temp);
    }
    else {
        global_table_replace_by_rightmost(ptrReplaced, &(*rootPtr)->rPtr);
    }
}

void global_table_delete(tGlobalTableNodePtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        global_table_delete(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        global_table_delete(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tGlobalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tGlobalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            global_table_replace_by_rightmost((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

void global_table_dispose(tGlobalTableNodePtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        global_table_dispose( &(*rootPtr)->lPtr );
        global_table_dispose( &(*rootPtr)->rPtr );

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

void global_table_print_tmp(tGlobalTableNodePtr TempTree, char* sufix, char fromdir) {
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
        global_table_print_tmp(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        global_table_print_tmp(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

void global_table_print(tGlobalTableNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        global_table_print_tmp(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}

// Špeciálne
int function_get_number_params(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        return data->params;
    }
    else {
        return -1;
    }
}

void function_increase_number_params(tGlobalTableNodePtr rootPtr, char *function_id) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        data->params = data->params + 1;
    }
}

void function_set_number_params(tGlobalTableNodePtr rootPtr, char *function_id, int number) {
    tDataNodeGlobal *data;
    bool found = global_table_search(rootPtr, function_id, &data);
    if (found) {
        data->params = number;
    }
}

int function_set_defined(tGlobalTableNodePtr *rootPtr, char *id) {
    tDataNodeGlobal *data;
    if ((*rootPtr) == NULL) {
        // úplne prázdny

        // Vytvor nový uzol v tabulke vlastne
        data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
        if (data == NULL) {
            // chyba
            return ERR_INTERNAL;
        }
        data->defined = true;
        data->function_table = NULL;
        data->params = 0;

        if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
            // Chyba
            return ERR_INTERNAL;
        }

    }
    else {
        bool found = global_table_search((*rootPtr), id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = true;
            data->function_table = NULL;
            data->params = 0;

            if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
                // Chyba
                return ERR_INTERNAL;
            }

        }
        else {
            // Premmenná existuje - nastav defined
            data->defined = true;
        }
    }

    return ERR_OK;
}

int function_set_undefined(tGlobalTableNodePtr *rootPtr, char *id) {
    tDataNodeGlobal *data;
    if ((*rootPtr) == NULL) {
        // úplne prázdny

        // Vytvor nový uzol v tabulke vlastne
        data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
        if (data == NULL) {
            // chyba
            return ERR_INTERNAL;
        }
        data->defined = false;
        data->function_table = NULL;
        data->params = 0;

        if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
            // Chyba
            return ERR_INTERNAL;
        }

    }
    else {
        bool found = global_table_search((*rootPtr), id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNodeGlobal *) malloc(sizeof(tDataNodeGlobal));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = false;
            data->function_table = NULL;
            data->params = 0;

            if (global_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
                // Chyba
                return ERR_INTERNAL;
            }

        }
        else {
            // Premmenná existuje - nastav defined
            data->defined = false;
        }
    }

    return ERR_OK;
}

tGlobalTableNodePtr get_function_node(tGlobalTableNodePtr rootPtr, char *id) {
    if (rootPtr == NULL) {
        return NULL;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            return rootPtr;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (get_function_node( rootPtr->lPtr, id));
            }
            else {
                return (get_function_node( rootPtr->rPtr, id));
            }
        }
    }
}

void set_function_table(tGlobalTableNodePtr *function_node_ptr, tLocalTableNodePtr *local_table_ptr) {
    (*function_node_ptr)->data->function_table = local_table_ptr;
}

// Funkcie pre lokálnu TS
void local_table_init(tLocalTableNodePtr *rootPtr) {
    (*rootPtr) = NULL;
}

int local_table_insert(tLocalTableNodePtr *rootPtr, char *id, tDataNodeLocal *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tLocalTableNodePtr newPtr = (tLocalTableNodePtr) malloc(sizeof(struct tLocalTableNode));
        if (newPtr != NULL) {
            // Podarilo sa alokovať miesto
            // ID
            newPtr->id = (char *) malloc(sizeof(char) * strlen(id));
            if (newPtr == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            strcpy(newPtr->id, id);

            // Alokuj miesto pre data
            newPtr->data = (tDataNodeLocal *) malloc(sizeof(tDataNodeGlobal));
            if (newPtr->data == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }

            // Uložiť obsah data
            newPtr->data->defined = data->defined;
            newPtr->data->type = data->type;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            local_table_insert( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            local_table_insert( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->type = data->type;
        }
    }

    return ERR_OK;
}

bool local_table_search(tLocalTableNodePtr rootPtr, char *id, tDataNodeLocal **data) {
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
                return (local_table_search( rootPtr->lPtr, id, data ));
            }
            else {
                return (local_table_search( rootPtr->rPtr, id, data ));
            }
        }
    }
}

void local_table_replace_by_rightmost(tLocalTableNodePtr ptrReplaced, tLocalTableNodePtr *rootPtr) {
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

        tLocalTableNodePtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        free(temp->data);
        free(temp);
    }
    else {
        local_table_replace_by_rightmost(ptrReplaced, &(*rootPtr)->rPtr);
    }
}

void local_table_delete(tLocalTableNodePtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        local_table_delete(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        local_table_delete(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tLocalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tLocalTableNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            local_table_replace_by_rightmost((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

void local_table_dispose(tLocalTableNodePtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        local_table_dispose( &(*rootPtr)->lPtr );
        local_table_dispose( &(*rootPtr)->rPtr );

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

void local_table_print_tmp(tLocalTableNodePtr TempTree, char* sufix, char fromdir) {
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
        local_table_print_tmp(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        local_table_print_tmp(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

void local_table_print(tLocalTableNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        local_table_print_tmp(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}

// Špeciálne

int variable_set_defined(tLocalTableNodePtr *rootPtr, char *id) {
    tDataNodeLocal *data;
    if ((*rootPtr) == NULL) {
        // úplne prázdny

        // Vytvor nový uzol v tabulke vlastne
        data = (tDataNodeLocal *) malloc(sizeof(tDataNodeLocal));
        if (data == NULL) {
            // chyba
            return ERR_INTERNAL;
        }
        data->defined = true;
        data->type = T_UNDEFINED;

        if (local_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
            // Chyba
            return ERR_INTERNAL;
        }

    }
    else {
        bool found = local_table_search((*rootPtr), id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNodeLocal *) malloc(sizeof(tDataNodeLocal));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = true;
            data->type = T_UNDEFINED;

            if (local_table_insert(&(*rootPtr), id, data) == ERR_INTERNAL) {
                // Chyba
                return ERR_INTERNAL;
            }

        }
        else {
            // Premmenná existuje - nastav defined
            data->defined = true;
        }
    }

    return ERR_OK;
}

void variable_set_type(tLocalTableNodePtr function_table, char *id, tDataType type) {
    tDataNodeLocal *data;
    bool found = local_table_search(function_table, id, &data);
    if (!found) {
        // Nenájdená funkcia
        return ;
    }
    else {
        // funkcia nájdená - vráť ukazovateĺ na ňu
        data->type = type;
    }
}

tLocalTableNodePtr get_variable_node(tLocalTableNodePtr rootPtr, char *id) {
    if (rootPtr == NULL) {
        return NULL;
    }
    else {
        if (strcmp(rootPtr->id,id) == 0) {
            return rootPtr;
        }
        else {
            if (strcmp(rootPtr->id, id) > 0) {
                return (get_variable_node( rootPtr->lPtr, id));
            }
            else {
                return (get_variable_node( rootPtr->rPtr, id));
            }
        }
    }
}

void symbol_table_dispose_all_local(tGlobalTableNodePtr *global_root) {
    if ((*global_root) != NULL) {
        symbol_table_dispose_all_local( &(*global_root)->lPtr );
        symbol_table_dispose_all_local( &(*global_root)->rPtr );

        tLocalTableNodePtr *local_temp = (*global_root)->data->function_table;
        if (local_temp != NULL)
            local_table_dispose(local_temp);
    }
}
