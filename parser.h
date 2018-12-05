#ifndef _PARSER_H
#define _PARSER_H

#include "main.h" // Kvôli global_table
#include "scanner.h"
#include "error.h"
#include "expression_parser.h"
#include "semantic_analysis.h"
#include "code_gen.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool expression;
Token expression_token;


/*
	TODO:	Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			<arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			Zmenit osetrovanie END a ELSE (asi na stack)
*/

int expected_params;

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

#endif