#include "symtable.h"


void BSTInit (tBSTNodePtr *rootPtr) {
    (*rootPtr) = NULL;
}


int BSTInsert (tBSTNodePtr *rootPtr, char *id, tDataNode *data) {
    if ((*rootPtr) == NULL) {
        // Vytvorenie nového uzlu
        tBSTNodePtr newPtr = (tBSTNodePtr) malloc(sizeof(struct tBSTNode));
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
            newPtr->data = (tDataNode *) malloc(sizeof(tDataNode));
            if (newPtr->data == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            // Uložiť obsah data
            newPtr->data->type = data->type;
            newPtr->data->defined = data->defined;
            newPtr->data->function_table = data->function_table;

            // Alokovať a uložiť value
            newPtr->data->value = tstring_struct_initialize();
            if (newPtr->data->value == NULL) {
                // Chyba
                return ERR_INTERNAL;
            }
            tstring_add_line(newPtr->data->value, data->value->string);

            // Parametre
            // Inicializovať dynamický string TString

            // Nakopírovať obsah
            newPtr->data->params = data->params;

            (*rootPtr) = newPtr;
        }
    }

    else {
        if (strcmp(id, (*rootPtr)->id) < 0 ) { // id < (*rootPtr)->id
            // Ideme vľavo
            BSTInsert( &(*rootPtr)->lPtr, id, data );
        }
        else if (strcmp(id, (*rootPtr)->id) > 0 ) { // id > (*rootPtr)->id
            // Ideme vpravo
            BSTInsert( &(*rootPtr)->rPtr, id, data );
        }
        else {
            // Prepíšeme staré dáta novými
            (*rootPtr)->data->defined = data->defined;
            (*rootPtr)->data->type = data->type;
            (*rootPtr)->data->function_table = data->function_table;
            (*rootPtr)->data->params = data->params;

            // Prepísanie value
            tstring_clear_string((*rootPtr)->data->value);
            tstring_add_line((*rootPtr)->data->value, data->value->string);
        }
    }


    return 0;
}


bool BSTSearch (tBSTNodePtr rootPtr, char *id, tDataNode **data) {
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
                return (BSTSearch( rootPtr->lPtr, id, data ));
            }
            else {
                return (BSTSearch( rootPtr->rPtr, id, data ));
            }
        }
    }
}


void ReplaceByRightmost (tBSTNodePtr ptrReplaced, tBSTNodePtr *rootPtr) {
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

        tBSTNodePtr temp = (*rootPtr);
        (*rootPtr) = (*rootPtr)->lPtr;

        tstring_free_struct(temp->data->value);
        free(temp->data);
        free(temp);
    }
    else {
        ReplaceByRightmost(ptrReplaced, &(*rootPtr)->rPtr);
    }
}

void BSTDelete (tBSTNodePtr *rootPtr, char *id) {
    if ((*rootPtr) == NULL) {
        // Nie je čo vymazať
        return;
    }

    if (strcmp(id, (*rootPtr)->id) < 0) {
        // Treba ís doľava
        BSTDelete(&(*rootPtr)->lPtr, id);
    }
    else if (strcmp(id, (*rootPtr)->id) > 0) {
        // Treba ísť doprava
        BSTDelete(&(*rootPtr)->rPtr, id);
    }
    else {
        // Treba vymazať tento uzol

        // Skontrolovať potomkov

        if ((*rootPtr)->rPtr == NULL) {
            // Nemá pravý podstrom
            tBSTNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->lPtr;

            // Vymazanie uzlu
            tstring_free_struct(temp->data->value);

            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else if ((*rootPtr)->lPtr == NULL) {
            // Nemá ľavý podstrom
            tBSTNodePtr temp = (*rootPtr);
            (*rootPtr) = (*rootPtr)->rPtr;

            // Vymazanie uzlu
            tstring_free_struct(temp->data->value);

            free(temp->data);
            free(temp->id);
            free(temp);
        }
        else {
            // Má dvoch potomkov
            ReplaceByRightmost((*rootPtr), &(*rootPtr)->lPtr);
        }
    }
}

void BSTDispose(tBSTNodePtr *rootPtr) {
    if ((*rootPtr) != NULL) {
        BSTDispose( &(*rootPtr)->lPtr );
        BSTDispose( &(*rootPtr)->rPtr );

        // Uvoľni value
        tstring_free_struct((*rootPtr)->data->value);

        // Uvoľni dáta
        free((*rootPtr)->data);

        // Uvoľni id
        free((*rootPtr)->id);

        free(*rootPtr);
        (*rootPtr) = NULL;
    }
}

// Pomocné funkcie - print

void Print_tree2(tBSTNodePtr TempTree, char* sufix, char fromdir) {
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
        Print_tree2(TempTree->rPtr, suf2, 'R');
        printf("%s  +-[%s]\n", sufix, TempTree->id);
        strcpy(suf2, sufix);
        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");
        Print_tree2(TempTree->lPtr, suf2, 'L');
        if (fromdir == 'R') printf("%s\n", suf2);
        free(suf2);
    }
}

void Print_tree(tBSTNodePtr TempTree) {
    printf("Struktura binarniho stromu:\n");
    printf("\n");
    if (TempTree != NULL)
        Print_tree2(TempTree, "", 'X');
    else
        printf("strom je prazdny\n");
    printf("\n");
    printf("=================================================\n");
}

int symbol_table_get_params(tBSTNodePtr rootPtr, char *function_id) {

    tDataNode *data;
    bool found = BSTSearch(rootPtr, function_id, &data);
    if (found) {
        return data->params;
    }
    else {
        return -1;
    }
}

int symbol_table_add_param(tBSTNodePtr rootPtr, char *function_id) {
    tDataNode *data;
    bool found = BSTSearch(rootPtr, function_id, &data);
    if (!found) {
        // Chyba - neexistuje tá funkcia
        return -1;
    }
    else {
        data->params = data->params + 1;
    }
}

int symbol_table_define_variable_or_function(tBSTNodePtr table, char *id) {
    if (table == NULL) {
        return -1;
    }
    else {
        tDataNode *data;
        bool found = BSTSearch(table, id, &data);
        if (!found) {
            // Premenná neexistuje zatiaľ

            // Vytvor nový uzol v tabulke vlastne
            data = (tDataNode *) malloc(sizeof(tDataNode));
            if (data == NULL) {
                // chyba
                return ERR_INTERNAL;
            }
            data->defined = true;
            data->type = T_UNDEFINED;
            data->function_table = NULL;
            data->params = -1;

            if (BSTInsert(&table, id, data) == ERR_INTERNAL) {
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

tBSTNodePtr symbol_table_get_function_table(tBSTNodePtr global_table, char *id) {

    tDataNode *data;
    bool found = BSTSearch(global_table, id, &data);
    if (!found) {
        // Nenájdená funkcia
        return NULL;
    }
    else {
        // funkcia nájdená - vráť ukazovateĺ na ňu
        return data->function_table;
    }
}

void symbol_table_set_variable_type(tBSTNodePtr function_table, char *id, tDataType type) {
    tDataNode *data;
    bool found = BSTSearch(function_table, id, &data);
    if (!found) {
        // Nenájdená funkcia
        return ;
    }
    else {
        // funkcia nájdená - vráť ukazovateĺ na ňu
        data->type = type;
    }
}

tBSTNodePtr symbol_table_create_local_table(tBSTNodePtr *function_node) {
    if (function_node == NULL) {
        return NULL;
    }

    tBSTNodePtr local_table;
    BSTInit (&local_table);

    return local_table;
}

char *symbol_table_get_variable_value(tBSTNodePtr local_table, char *id) {
    tDataNode *data;
    bool found = BSTSearch(local_table, id, &data);
    if (!found) {
        // Nenájdená premená
        return NULL;
    }
    else {
        // nájdená premenná - vráť hodnotu
        return data->value->string;
    }
}

int symbol_table_set_variable_value(tBSTNodePtr *rootPtr, char *id, char *value) {
    tDataNode *data;
    bool found = BSTSearch((*rootPtr), id, &data);
    if (!found) {
        // Nenájdená premenná
        return -1;
    }
    else {
        // Nájdená premenná - nastav hodnotu
        tstring_clear_string(data->value);
        tstring_add_line(data->value, value);
        return ERR_OK;

    }
}

/*
int main() {

    tBSTNodePtr myTree;
    BSTInit(&myTree);

    tDataNode data;
    data.function_table = NULL;
    data.type = 42;
    data.defined = true;
    TString *str = tstring_struct_initialize();
    tstring_add_line(str, "text");
    data.value = str;


    data.params = 2;

    BSTInsert(&myTree, "hello", &data);

    tDataNode *myData;
    bool found = BSTSearch(myTree, "hello", &myData);
    printf("%d\n", found);


    Print_tree(myTree);

    int par = symbol_table_get_params(myTree, "hello");
    printf("%d\n", par);
    symbol_table_add_param(myTree, "hello");
    par = symbol_table_get_params(myTree, "hello");
    printf("%d\n", par);

    char *text = symbol_table_get_variable_value(myTree, "hello");
    printf("%s\n", text);

    BSTDispose(&myTree);

    tstring_free_struct(str);

    return 0;
}
 */