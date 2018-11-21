#include "parser.h"

/*	
	TODO:	Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			<arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			pridat pravidlo <stat_list> -> epsilon
*/

/* ---------------------------------------
 * Funkcie pre zásobník hodnôt typu tData.
 * ---------------------------------------
 * Zásobník slúži na počítanie nedokončených zložených príkazov, zisťuje sa tak,
 * či program očakáva kľúčové slová END (alebo ELSE v prípade príkazu IF-ELSE).
 * Dodatočne, sa pomocou zásobníku kontroluje či sa môže vykonať definícia funkcie,
 * ktorá môže byť vykonaná iba mimo zložených príkazov (t.j. z8sobník musí byť prázdny).
 */

void StackInit (tStack *stack) {
	stack->topPtr = NULL;
}

void StackPush (tStack *stack, tData data) {
	tSElem *newElemPtr = (tSElem *) malloc(sizeof(tSElem));

	if (newElemPtr != NULL) {
		newElemPtr->data = data;
		newElemPtr->nextPtr = stack->topPtr;
		stack->topPtr = newElemPtr;
	}
	else {
		fprintf (stderr, "Chyba pri alokácii pamäte.");
		exit (ERR_INTERNAL);
	}
}

void StackPop (tStack *stack) {
	tSElem *elemPtr;

	if (stack->topPtr != NULL) {
		elemPtr = stack->topPtr;
		stack->topPtr = stack->topPtr->nextPtr;
		free (elemPtr);
	}
	else {
		fprintf (stderr, "Chyba: Podtečenie zásobníku.");
		exit (ERR_INTERNAL);
	}
}

tData StackTop (tStack *stack) {
	if (stack->topPtr != NULL) {
		return (stack->topPtr->data);
	}
	else {
		fprintf (stderr, "Chyba: Získavanie hodnoty prvku z prázndeho zásobíku");
		exit (ERR_INTERNAL);
	}
}

bool StackEmpty (tStack *stack) {
	return (stack->topPtr == NULL);
}

void StackDispose (tStack *stack) {
	tSElem *elemPtr;

	while (stack->topPtr != NULL) {
		elemPtr = stack->topPtr;
		stack->topPtr = stack->topPtr->nextPtr;
		free (elemPtr);
	}
}

/* -------------------------------------------------------------------------------- */

/*
	Funkcia pre stav <prog>.
	
	Program musí začínať kľúčovým slovom alebo musí byť prázdny (EOF).
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút, 
	prechádza sa do stavu <stat_list>.
	V prípade, že prijatý token je EOF, preklad sa končí s návratovou hodnotou ERR_OK.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.
*/
int prog (Token *token, tStack *endElseStack) {

	// Pravidlo 1: <prog> -> <stat_list> EOF

	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0) {
			return stat_list(token, endElseStack);
		}
		else if (strcmp(token->attribute, "if") == 0) {
			return stat_list(token, endElseStack);
		}
		else if (strcmp(token->attribute, "while") == 0) {
			return stat_list(token, endElseStack);
		}
		else if (strcmp(token->attribute, "print") == 0) {
			return stat_list(token, endElseStack);
		}
		else {
			return ERR_SYNTAX;
		}

	}
	else if (token->type == IDENTIFIER) {
		return stat_list(token, endElseStack);
	}
	else if (token->type == EOF) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}

/*
	Funkcia pre stav <stat_list>.
	
	Statement list musí začínať kľúčovým slovom alebo musí byť prázdny.
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút alebo je typu IDENTIFIER
	prechádza sa do stavu <stat>.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.
*/
int stat_list (Token *token, tStack *endElseStack) {

	// Pravidlo 2: <stat_list> -> <stat> EOL <stat_list>

	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0  &&  StackEmpty (endElseStack)) {	//Definícia funkcie sa nemôže nachádzať v zložených príkazoch ani v tele inej funkcie
			if (stat(token, endElseStack) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token, endElseStack);
				}
			}
		}
		else if (strcmp(token->attribute, "if") == 0) {
			if (stat(token, endElseStack) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token, endElseStack);
				}
			}
		}
		else if (strcmp(token->attribute, "while") == 0) {
			if (stat(token, endElseStack) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token, endElseStack);
				}
			}
		}
		else if (strcmp(token->attribute, "print") == 0) {
			if (stat(token, endElseStack) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token, endElseStack);
				}
			}
		}
		else if (strcmp(token->attribute, "end") == 0  &&  StackTop (endElseStack) == END) {
			StackPop (endElseStack);

			return ERR_OK;
		}
		else if (strcmp(token->attribute, "else") == 0  &&  StackTop (endElseStack) == ELSE) {
			StackPop (endElseStack);

			return ERR_OK;
		}
		else return ERR_SYNTAX;

	}
	else if (token->type == IDENTIFIER) {
		if (stat(token, endElseStack) == ERR_OK) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == EOL) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				return stat_list(token, endElseStack);
			}
		}
	}
	else if (token->type == EOF) {
		if (StackEmpty(endElseStack)) {
			return ERR_OK;
		}
		else return ERR_SYNTAX;
	}
	else return ERR_SYNTAX;
}


/*
	Funkcia pre stav <stat>.
	
	Statement musí začínať kľúčovým slovom alebo identifikátorom.
*/
int stat (Token *token, tStack *endElseStack) {

	// Pravidlo 3: <stat> -> DEF ID ( <params> ) EOL <stat_list> END 
	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == IDENTIFIER) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == LEFT_ROUND_BRACKET) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					// Pravidlo 10: <params> -> epsilon

					if (token->type == RIGHT_ROUND_BRACKET) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}	
					}
					else {
						if (params(token) == ERR_OK) {
							if (get_next_token(token) == ERR_SCANNER) {
								return ERR_SCANNER;
							}
						}
						else return ERR_SYNTAX;
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						StackPush (endElseStack, END);

						return stat_list(token, endElseStack);
					}
				}
			}
		}	
	

		// Pravidlo 4: IF <expr> THEN EOL <stat_list> ELSE EOL <stat_list> END

		else if (strcmp(token->attribute, "if") == 0) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (expr(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (strcmp(token->attribute, "then") == 0) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						StackPush (endElseStack, ELSE);

						if (stat_list(token, endElseStack) == ERR_OK) {	//ELSE a END sa kontrolujú vo funkcii stat_list.
							if (get_next_token(token) == ERR_SCANNER) {
								return ERR_SCANNER;
							}
							StackPush (endElseStack, END);

							return stat_list(token, endElseStack);		
						}
					}
				}
			}
		}


		// Pravidlo 5: WHILE <expr> DO EOL <stat_list> END

		else if (strcmp(token->attribute, "while") == 0) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (expr(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (strcmp(token->attribute, "do") == 0) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						StackPush (endElseStack, END);

						return stat_list(token, endElseStack);
					}
				}
			}
		}
	
		// Pravidlo 6: PRINT ( <arg> ) ;   Pravá zátvorka je kontrolovaná vo funkcii arg_next

		else if (strcmp(token->attribute, "print") == 0) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == LEFT_ROUND_BRACKET) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				return arg(token);
			}
		}
	}

	// Pravidlo 7: ID <after_id>

	else if (token->type == IDENTIFIER) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		return after_id(token);
	}
	else return ERR_SYNTAX;
}


int params (Token *token) {

	// Pravidlo 8: <params> -> ID <params_next>

	if (token->type == IDENTIFIER) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		return params_next(token);
	}
	else return ERR_SYNTAX;
}


int params_next (Token *token) {

	// Pravidlo 10: <params_next> -> , ID <params_next>

	if (token->type == COMMA) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		if (token->type == IDENTIFIER) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			return params_next(token);
		}
		else return ERR_SYNTAX;
	}

	// Pravidlo 11: <params_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}


int arg (Token *token) {

	// Pravidlo 12: <arg> -> <value> <arg_next>

	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
			(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 ) || token->type == IDENTIFIER) {
		return (value(token) && arg_next(token));
	}
	else return ERR_SYNTAX;
}


int arg_next (Token *token) {

	// Pravidlo 14: <arg_next> -> , <value> <arg_next>

	if (get_next_token(token) == ERR_SCANNER) {
		return ERR_SCANNER;
	}

	if (token->type == COMMA) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
				(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 ) || token->type == IDENTIFIER) {

			return arg_next(token);
		}
		else return ERR_SYNTAX;
	}

	// Pravidlo 15: <arg_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}


int after_id (Token *token) {

	// Pravidlo 16: <after_id> -> ( <arg> )

	if (token->type == LEFT_ROUND_BRACKET) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		// Pravidlo 13: <arg> -> epsilon

		if (token->type == RIGHT_ROUND_BRACKET) {
			return ERR_OK;

		}
		else {
			if (arg(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				return ERR_OK;
			}
			else return ERR_SYNTAX;
		}
	}

	// Pravidlo 17: <after_id> = <def_value>

	else if (token->type == ASSIGN) {
		return def_value(token);
	}
	else return ERR_SYNTAX;
}


int def_value (Token *token) {

	// Pravidlo 18: <def_value> -> <value>

	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
			(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 ) || token->type == IDENTIFIER) {
		return value(token);
	}

	// Pravidlo 19: <def_value> -> <expr>
	//	VYRAZY NIE SU OSETRENE, mozno spolu s <value>: return expr(token)

	// Pravidlo 20: <def_value> -> ID ( <arg> )

	else if (token->type == IDENTIFIER) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		if (token->type == LEFT_ROUND_BRACKET) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == RIGHT_ROUND_BRACKET) {
				return ERR_OK;
			}
			else return arg(token);
		}
	}
	else if (token->type == LEFT_ROUND_BRACKET) {
		return expr(token);
	}
	else return ERR_SYNTAX;
}


int value (Token *token) {

	//Pravidlo 21 - 25: <value> -> INTEGER | FLOAT | STRING | NIL | ID

	if (token->type == KEYWORD && strcmp(token->attribute, "nil") == 0) {
		return ERR_OK;
	}

	switch (token->type) {
		case INTEGER:
			return ERR_OK;
			break;
		
		case FLOAT:
			return ERR_OK;
			break;

		case STRING:
			return ERR_OK;
			break;

		case IDENTIFIER:
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}
			
			if (token->type == LEFT_ROUND_BRACKET) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == RIGHT_ROUND_BRACKET) {
					return ERR_OK;
				}
				else return arg(token);
			}
				return ERR_OK;
				break;

		default:
			return ERR_SYNTAX;
	}
}

// TODO odstrániť keď bude analýza výrazov
int expr (Token *token) {
	return ERR_OK;
}