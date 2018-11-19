#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "stringlib.h"
#include "error.h"

typedef enum {
    T_INT,
    T_FLOAT,
    T_STRING,
    T_BOOLEAN,
    T_NIL,
    T_UNDEFINED
} tDataType;

typedef struct {
    tDataType type;
    bool defined;
    struct tBSTNode *function_table;
    int params;
} tDataNode;

typedef struct tBSTNode{
    char *id; // Identifikátor
    tDataNode *data;

    struct tBSTNode * lPtr;
    struct tBSTNode * rPtr;
} *tBSTNodePtr;

void BSTInit (tBSTNodePtr *rootPtr);
int BSTInsert (tBSTNodePtr *rootPtr, char *id, tDataNode *data);
bool BSTSearch (tBSTNodePtr rootPtr, char *id, tDataNode **data);
void ReplaceByRightmost (tBSTNodePtr ptrReplaced, tBSTNodePtr *rootPtr);
void BSTDelete (tBSTNodePtr *rootPtr, char *id);
void BSTDispose (tBSTNodePtr *rootPtr);

int symbol_table_get_params(tBSTNodePtr rootPtr, char *function_id);
int symbol_table_add_param(tBSTNodePtr rootPtr, char *function_id);
int symbol_table_define_variable_or_function(tBSTNodePtr table, char *id);
tBSTNodePtr symbol_table_get_function_table(tBSTNodePtr global_table, char *id);
void symbol_table_set_variable_type(tBSTNodePtr function_table, char *id, tDataType type);
tBSTNodePtr symbol_table_create_local_table(tBSTNodePtr *function_node);

void Print_tree(tBSTNodePtr TempTree);
void Print_tree2(tBSTNodePtr TempTree, char* sufix, char fromdir);