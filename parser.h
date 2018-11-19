#include "scanner.h"
#include "error.h"

#include <string.h>


/*
	TODO:	Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			<arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			Zmenit osetrovanie END a ELSE (asi na stack)
*/

/*
	Počítadlá pre očakávané kľúčové slová END a ELSE
*/
//int 	expecting_end = 0,
//        expecting_else = 0;
int 	expecting_end;
int     expecting_else;

int prog (Token *token);

int stat_list (Token *token);

int stat (Token *token);

int params (Token *token);

int params_next (Token *token);

int arg (Token *token) ;

int arg_next (Token *token);

int after_id (Token *token);

int def_value (Token *token);

int value (Token *token);

// TODO odstrániť keď bude analýza výrazov
int expr (Token *token);