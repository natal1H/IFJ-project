#include "scanner.h"
#include "error.h"


/*	
	TODO:	Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			<arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			Zmenit osetrovanie END a ELSE (asi na stack)
*/

/*
	Počítadlá pre očakávané kľúčové slová END a ELSE
*/
global int 	expecting_end = 0,
			expecting_else = 0;
/*
	Funkcia pre stav <prog>.
	
	Program musí začínať kľúčovým slovom alebo musí byť prázdny (EOF).
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút, 
	prechádza sa do stavu <stat_list>.
	V prípade, že prijatý token je EOF, preklad sa končí s návratovou hodnotou ERR_OK.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.
*/
int prog (Token *token) {

	// Pravidlo 1: <prog> -> <stat_list> EOF

	if (token->type == KEYWORD) {
		switch (token->attribute) {
			case KEYWORD_DEF:
				return stat_list(token);
				break;

			case KEYWORD_IF:
				return stat_list(token);
				break;

			case KEYWORD_WHILE:
				return stat_list(token);
				break;

			case KEYWORD_PRINT:
				return stat_list(token);
				break;

			default: 
				return ERR_SYNTAX;
		}
	}
	else if (token->type == IDENTIFIER) {
		return stat_list(token);
	}
	else if (token->type == EOF) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}

/*
	Funkcia pre stav <stat_list>.
	
	Statement list musí začínať kľúčovým slovom alebo musí byť prázdny.
	V prípade, že prijatý token je typu KEYWORD a má vhodný atribút, 
	prechádza sa do stavu <stat>.
	V ostatných prípadoch nastane syntax error a funkcia vracia hodnotu ERR_SYNTAX.
*/
int stat_list (Token *token) {

	// Pravidlo 2: <stat_list> -> <stat> EOL <stat_list>

	if (token->type == KEYWORD) {
		switch (token->attribute) {
			case KEYWORD_DEF:
				if (stat(token) == ERR_OK) {
					get_next_token(token);

					if (token->type == EOL) {
						get_next_token(token);

						return stat_list(token);
					}
				}
				break;

			case KEYWORD_IF:
				if (stat(token) == ERR_OK) {
					get_next_token(token);

					if (token->type == EOL) {
						get_next_token(token);

						return stat_list(token);
					}
				}
				break;

			case KEYWORD_WHILE:
				if (stat(token) == ERR_OK) {
					get_next_token(token);

					if (token->type == EOL) {
						get_next_token(token);

						return stat_list(token);
					}
				}
				break;

			case KEYWORD_PRINT:
				if (stat(token) == ERR_OK) {
					get_next_token(token);

					if (token->type == EOL) {
						get_next_token(token);

						return stat_list(token);
					}
				}
				break;

			case KEYWORD_END:
				if (expecting_end > 0) {	//Kontrola, či sa v kóde očakáva END
					expecting_end--;		//Ak áno, počítadlo očakávaných END-ov sa dekrementuje
											//a vracia sa návratová hodnota ERR_OK
					return ERR_OK;			//Ak by táto kontrola nebola vykonaná, program by akceptoval
				}							//END a ELSE aj mimo If-ov atď.
				else return ERR_SYNTAX;

				break;

			case KEYWORD_ELSE:				//To isté, ako pre END
				if (expecting_else > 0) {
					expecting_else--;

					return ERR_OK;
				}
				else return ERR_SYNTAX;

				break;

			default: 
				return ERR_SYNTAX;
		}
	}
	else if (token->type == IDENTIFIER) {
		if (stat(token) == ERR_OK) {
			get_next_token(token);

			if (token->type == EOL) {
				get_next_token(token);

				return stat_list(token);
			}
		}
	}
	else if (token->type == EOF) {
		if (!expecting_end && !expecting_else) {
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
int stat (Token *token) {

	// Pravidlo 3: <stat> -> DEF ID ( <params> ) EOL <stat_list> END 

	if (token->attribute == KEYWORD_DEF) {
		get_next_token(token);

		if (token->type == IDENTIFIER) {
			get_next_token(token);

			if (token->type == LEFT_ROUND_BRACKET) {
				get_next_token(token);

				// Pravidlo 10: <params> -> epsilon

				if (token->type == RIGHT_ROUND_BRACKET) {
					get_next_token(token);	
				}
				else {
					if (params(token) == ERR_OK) {
						get_next_token(token);
					}
					else return ERR_SYNTAX;
				}

				if (token->type == EOL) {
					get_next_token(token);
					expecting_end++;

					return stat_list(token);
				}
			}
		}
	}
	

	// Pravidlo 4: IF <expr> THEN EOL <stat_list> ELSE EOL <stat_list> END

	else if (token->attribute == KEYWORD_IF) {
		get_next_token(token);

		if (expr(token) == ERR_OK) {
			get_next_token(token);

			if (token->attribute == KEYWORD_THEN) {
				get_next_token(token);

				if (token->type == EOL) {
					get_next_token(token);
					if (expecting_end > 0) {
						expecting_end = -expecting_end;
					}
					expecting_else++;

					if (stat_list(token) == ERR_OK) {	//ELSE a END sa kontrolujú vo funkcii stat_list.
						get_next_token(token);

						return stat_list(token);		
					}
				}
			}
		}
	}


	// Pravidlo 5: WHILE <expr> DO EOL <stat_list> END

	else if (token->attribute == KEYWORD_WHILE) {
		get_next_token(token);

		if (expr(token) == ERR_OK) {
			get_next_token(token);

			if (token->attribute == KEYWORD_DO) {
				get_next_token(token);

				if (token->type == EOL) {
					get_next_token(token);
					expecting_end++;
					if (expecting_else > 0){
						expecting_else = -expecting_else;
					}

					return stat_list(token);
				}
			}
		}
	}

	// Pravidlo 6: PRINT ( <arg> )

	else if (token->type == IDENTIFIER && token->attribute == PRINT) {
		get_next_token(token);

		if (token->type == LEFT_ROUND_BRACKET) {
			get_next_token(token);

			if (arg(token) == ERR_OK) {
				get_next_token(token);

				if (token->type == RIGHT_ROUND_BRACKET) {
					return ERR_OK;
				}
			}
		}
	}

	// Pravidlo 7: ID <after_id>

	else if (token->type == IDENTIFIER) {
		get_next_token(token);

		return after_id(token);
	}
	else return ERR_SYNTAX;
}


int params (Token *token) {

	// Pravidlo 8: <params> -> ID <params_next>

	if (token->type == IDENTIFIER) {
		get_next_token(token);

		return params_next(token);
	}
	else return ERR_SYNTAX;
}


int params_next (Token *token) {

	// Pravidlo 10: <params_next> -> , ID <params_next>

	if (token->type == COMMA) {
		get_next_token(token);

		if (token->type == IDENTIFIER) {
			get_next_token(token);

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
		token->type == NIL || token->type == IDENTIFIER) {
		return (value(token) && arg_next(token));
	}
	else return ERR_SYNTAX;
}


int arg_next (Token *token) {

	// Pravidlo 14: <arg_next> -> , <value> <arg_next>

	get_next_token(token);

	if (token->type == COMMA) {
		get_next_token(token);

		if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
		token->type == NIL || token->type == IDENTIFIER) {

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
		get_next_token(token);

		// Pravidlo 13: <arg> -> epsilon

		if (token->type == RIGHT_ROUND_BRACKET) {
			return ERR_OK;

		}
		else {
			if (arg(token) == ERR_OK) {
				get_next_token(token);

				return ERR_OK;
			}
			else return ERR_SYNTAX;
		}
	}

	// Pravidlo 17: <after_id> = <def_value>

	else if (token->type == ASSIGN) {
		return def_value;
	}
	else return ERR_SYNTAX;
}


int def_value (Token *token) {

	// Pravidlo 18: <def_value> -> <value>

	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
		token->type == NIL || token->type == IDENTIFIER) {
		return value(token);
	}

	// Pravidlo 19: <def_value> -> <expr>
	//	VYRAZY NIE SU OSETRENE, mozno spolu s <value>: return expr(token)

	// Pravidlo 20: <def_value> -> ID ( <arg> )

	else if (token->type == IDENTIFIER) {
		get_next_token(token);

		if (token->type == LEFT_ROUND_BRACKET) {
			get_next_token(token);

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

		case NIL:
			return ERR_OK;
			break;

		case IDENTIFIER:
			get_next_token(token);
			
			if (token->type == LEFT_ROUND_BRACKET) {
				get_next_token(token);

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