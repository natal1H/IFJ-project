#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int type;
    bool defined;
} tDataLocalNode;

typedef struct tBSTNodeLocal{
    char *id; // Identifikátor
    //int type;
    //bool defined;
    tDataLocalNode *data;

    struct tBSTNodeLocal * lPtr;
    struct tBSTNodeLocal * rPtr;
} *tBSTNodeLocalPtr;

typedef struct {
    int type;
    bool defined;
    tBSTNodeLocalPtr function_local_table;
    // Parametre ?
} tDataGlobalNode;

typedef struct tBSTNodeGlobal{
    char *id; // Identifikátor
    tDataGlobalNode *data;

    struct tBSTNodeGlobal * lPtr;
    struct tBSTNodeGlobal * rPtr;
} *tBSTNodeGlobalPtr;


void BSTInitGlobal (tBSTNodeGlobalPtr *rootPtr);
int BSTInsertGlobal (tBSTNodeGlobalPtr *rootPtr, char *id, tDataGlobalNode *data);
bool BSTSearchGlobal (tBSTNodeGlobalPtr rootPtr, char *id, tDataGlobalNode **data);
void ReplaceByRightmostGlobal (tBSTNodeGlobalPtr ptrReplaced, tBSTNodeGlobalPtr *rootPtr);
void BSTDeleteGlobal (tBSTNodeGlobalPtr *rootPtr, char *id);
void BSTDisposeGlobal (tBSTNodeGlobalPtr *rootPtr);

void BSTInitLocal (tBSTNodeLocalPtr *rootPtr);
int BSTInsertLocal (tBSTNodeLocalPtr *rootPtr, char *id, tDataLocalNode *data);
bool BSTSearchLocal (tBSTNodeLocalPtr rootPtr, char *id, tDataLocalNode **data);
void ReplaceByRightmostLocal (tBSTNodeLocalPtr ptrReplaced, tBSTNodeLocalPtr *rootPtr);
void BSTDeleteLocal (tBSTNodeLocalPtr *rootPtr, char *id);
void BSTDisposeLocal (tBSTNodeLocalPtr *rootPtr);

void Print_tree_global(tBSTNodeGlobalPtr TempTree);
void Print_tree2_global(tBSTNodeGlobalPtr TempTree, char* sufix, char fromdir);

void Print_tree_local(tBSTNodeLocalPtr TempTree);
void Print_tree2_local(tBSTNodeLocalPtr TempTree, char* sufix, char fromdir);
