#include "scanner.h"
#include "error.h"

#include <string.h>


/*
	TODO:	Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			<arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			Zmenit osetrovanie END a ELSE (asi na stack)
*/

typedef enum tdata {
	ELSE, END
} tData;

typedef struct tselem {
	tData data;
	struct tselem *nextPtr;
} tSElem;

typedef struct tstack {
	tSElem *topPtr;
} tStack;

void StackInit (tStack *stack);

void StackPush (tStack *stack, tData data);

void StackPop (tStack *stack);

tData StackTop (tStack *stack);

bool StackEmpty (tStack *stack);

void StackDispose (tStack *stack);

int prog (Token *token, tStack *endElseStack);

int stat_list (Token *token, tStack *endElseStack);

int stat (Token *token, tStack *endElseStack);

int params (Token *token);

int params_next (Token *token);

int arg (Token *token) ;

int arg_next (Token *token);

int after_id (Token *token);

int def_value (Token *token);

int value (Token *token);

// TODO odstrániť keď bude analýza výrazov
int expr (Token *token);