#include "parser.h"
#include "scanner.h"
#include <stdio.h>
#include <stdbool.h>

/*	
	TODO:	Gramatika: - Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			   - <arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			   - pridat pravidlo <stat_list> -> epsilon
			   - vymazat stav <value> (nahradeny <expr>)

		Kod: - Pri volani funkcie nemusia byt okolo argumentov zatvorky
		     - Doriesit priradovanie do premennej:
			(semanticka anlyza pozrie ci je identifikator 
			 v lokalnej tabulke symbolov(premenna) alebo
			 v globalnej tabulke symbolov(funkcia).
			 Ak je v lokalnej, vola sa expression parser.)

*/


int depth_index = 0;
bool in_if_or_while = false;
bool in_def = false;

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
		if (strcmp(token->attribute, "def") == 0) {
			return stat_list(token);
		}
		else if (strcmp(token->attribute, "if") == 0) {
			return stat_list(token);
		}
		else if (strcmp(token->attribute, "while") == 0) {
			return stat_list(token);
		}
		else {
			return ERR_SYNTAX;
		}

	}
	else if (token->type == IDENTIFIER) {
		return stat_list(token);
	}
	else if (token->type == TYPE_EOF) {
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
int stat_list (Token *token) {

	// Pravidlo 2: <stat_list> -> <stat> EOL <stat_list>

	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0  && !in_if_or_while && !in_def) {	//Definícia funkcie sa nemôže nachádzať v zložených príkazoch ani v tele inej funkcie
			if (stat(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}
				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "if") == 0) {
			if (stat(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "while") == 0) {
			if (stat(token) == ERR_OK) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == EOL) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "end") == 0) {
			if (in_if_or_while || in_def) {
				return ERR_OK;
			}
		}
		else if (strcmp(token->attribute, "else") == 0) {
			if (in_if_or_while) {
				return ERR_OK;
			}
		}
	}
	else if (token->type == IDENTIFIER) {
		if (stat(token) == ERR_OK) {
			if (token->type == EOL) {
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				return stat_list(token);
			}
		}
	}
	else if (token->type == TYPE_EOF) {
		if (depth_index == 0) {

			return ERR_OK;
		}
	}
	
	return ERR_SYNTAX;
}


/*
	Funkcia pre stav <stat>.
	
	Statement musí začínať kľúčovým slovom alebo identifikátorom.
*/
int stat (Token *token) {

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
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}

						depth_index++;
						in_def = true;

						if (stat_list(token) == ERR_OK) {
							if (token->type == KEYWORD && strcmp(token->attribute, "end") == 0){
								depth_index--;
								in_def = false;

								return ERR_OK;
							}
						}
					}
				}
			}
		}	

		// Pravidlo 4: IF <expr> THEN EOL <stat_list> ELSE EOL <stat_list> END

		else if (strcmp(token->attribute, "if") == 0) {
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (CallExpressionParser(token) == ERR_OK {
				if (strcmp(token->attribute, "then") == 0) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "else") == 0) {
								if (get_next_token(token) == ERR_SCANNER) {
									return ERR_SCANNER;
								}

								if (token->type == EOL) {
									if (get_next_token(token) == ERR_SCANNER) {
										return ERR_SCANNER;
									}

									if (stat_list(token) == ERR_OK) {
										if (strcmp(token->attribute, "end") == 0) {
											depth_index--;
											if (depth_index == 0 || (in_def && depth_index == 1)) in_if_or_while = false;

											return ERR_OK;
										}
									}
								}
							}	
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

			if (CallExpressionParser(token) == ERR_OK) {
				if (strcmp(token->attribute, "do") == 0) {
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "end") == 0) {
								depth_index--;
								if (depth_index == 0 || (in_def && depth_index == 1)) in_if_or_while = false;

								return ERR_OK;
							}
						}
					}
				}
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

	return ERR_SYNTAX;
}


int params (Token *token) {

	// Pravidlo 8: <params> -> ID <params_next>

	if (token->type == IDENTIFIER) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		return params_next(token);
	}
	
	return ERR_SYNTAX;
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
	}

	// Pravidlo 11: <params_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
		return ERR_OK;
	}

	return ERR_SYNTAX;
}


int arg (Token *token) {

	// Pravidlo 12: <arg> -> <value> <arg_next>

	if (value(token) == ERR_OK && arg_next(token) == ERR_OK) {
		return ERR_OK;
	}
	else return ERR_SYNTAX;
}


int arg_next (Token *token) {

	// Pravidlo 14: <arg_next> -> , <value> <arg_next>

	if (token->type == COMMA) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}
		
		if (value(token) == ERR_OK) {
			return arg_next(token);
		}
	}

	// Pravidlo 15: <arg_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
		return ERR_OK;
	}

	return ERR_SYNTAX;
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
		else if (arg(token) == ERR_OK) {
			return ERR_OK;
		}
	}

	// Pravidlo 17: <after_id> = <def_value>

	else if (token->type == ASSIGN) {
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		return def_value(token);
	}
	
	return ERR_SYNTAX;
}


int def_value (Token *token) {
	//TODO: vid zaciatok suboru
	// Pravidlo 19: <def_value> -> <expr>

	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING ||
			(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 )) {
		return CallExpressionParser(token);
	}

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
		return CallExpressionParser(token);
	}
	
	return ERR_SYNTAX;
}
