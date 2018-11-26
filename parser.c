#include "parser.h"

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
printf("eol\n");
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
printf("eol\n");
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

				if (token->type == EOL) {\
printf("eol\n");
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "end") == 0) {
			if (in_def && !in_if_or_while) {
				printf("## BOLO V DEFINíCII, teraz prechádza naspäť do main ##\n");
				// Nebolo už vo while/if, vychádza z definície funkcie
				// Sémantická akcia - presunúť ukazovať na aktuálnu lokálnu tabuľku naspäť na MAIN
				tGlobalTableNodePtr main_global_node = get_function_node(global_table, MAIN); // Vráti ukazovvateľ na uzol s MAIN v GTS
				actual_function_name = MAIN; // Nastaviť meno aktuálnej funkcie späť na MAIN
				printf("Actual function name: %s\n", actual_function_name);
				actual_function_ptr = (main_global_node->data->function_table); // Aktuálne lokálna tabuľka je nová lokálna tabuľka
				local_table_print(*actual_function_ptr);
				// Koniec sémantickej akcie
			}
			if (in_if_or_while || in_def) {
printf("end ");
				return ERR_OK;
			}
		}
		else if (strcmp(token->attribute, "else") == 0) {
			if (in_if_or_while) {
printf("else ");
				return ERR_OK;
			}
		}
	}
	else if (token->type == IDENTIFIER) {
		if (stat(token) == ERR_OK) {
printf("%s",  token->attribute);
			if (token->type == EOL) {
printf("eol\n");
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				return stat_list(token);
			}
		}
	}
	else if (token->type == TYPE_EOF) {
		if (depth_index == 0) {
printf("eof\n");
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
printf("def ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);

				// Sémantická kontrola - nebola už definovaná funkcia s takýmto názvom?
				if (function_definition(&global_table, token->attribute) != ERR_OK) {
					// Funkcia bola predtým definovaná - došlo k redefinícii - sémantická chyba
					return ERR_SEM_ELSE; // TODO - správny chybový kód?
				}
				// Vytvoriť lokálnu tabuľku symbolov
				tLocalTableNodePtr new_local_table;
				local_table_init(&new_local_table); // Inicializácia novej lokálnej tabuľky
				tGlobalTableNodePtr function_global_node = get_function_node(global_table, token->attribute); // Vráti ukazovvateľ na uzol s token->attribute v GTS
variable_set_defined(&new_local_table, token->attribute);
				set_function_table(&function_global_node, &new_local_table); // Naviazanie uzla v globálnej na novú lokálnu
				// Bude nasledovať definícia funkcie - preto treba zmeniť ukazovateľ na aktuálnu lokálnu TS z MAIN na tabuľku novej funkcie
				// Nastaviť actual_function_name (ID funkcie, v ktorej sa práve nachádza program) na token->attribute
				actual_function_name = token->attribute;
printf("Actual function name: %s\n", actual_function_name);
				// Nastaviť actual_function_table
				actual_function_ptr = &new_local_table; // Aktuálne lokálna tabuľka je nová lokálna tabuľka
local_table_print(*actual_function_ptr);
				// Koniec sémantickej kontroly

				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}

				if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					// Pravidlo 10: <params> -> epsilon

					if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
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
printf("eol\n");
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
printf("if ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (CallExpressionParser(token) == ERR_OK) {
				if (strcmp(token->attribute, "then") == 0) {
printf("then ");
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
printf("eol\n");
						if (get_next_token(token) == ERR_SCANNER) {
							return ERR_SCANNER;
						}
						
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "else") == 0) {
printf("else ");
								if (get_next_token(token) == ERR_SCANNER) {
									return ERR_SCANNER;
								}

								if (token->type == EOL) {
printf("eol\n");
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
printf("while ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (CallExpressionParser(token) == ERR_OK) {
				if (strcmp(token->attribute, "do") == 0) {
printf("do ");
					if (get_next_token(token) == ERR_SCANNER) {
						return ERR_SCANNER;
					}

					if (token->type == EOL) {
printf("eol\n");
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
printf("%s ", token->attribute);
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
printf("%s ", token->attribute);
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
printf(", ");
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			return params_next(token);
		}
	}

	// Pravidlo 11: <params_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
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
printf(", ");
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}
		
		if (value(token) == ERR_OK) {
			return arg_next(token);
		}
	}

	// Pravidlo 15: <arg_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
		return ERR_OK;
	}

	return ERR_SYNTAX;
}


int after_id (Token *token) {

	// Pravidlo 16: <after_id> -> ( <arg> )

	if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		// Pravidlo 13: <arg> -> epsilon

		if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			return ERR_OK;

		}
		else if (arg(token) == ERR_OK) {
			if (get_next_token(token) == ERR_SYNTAX) {
				return ERR_SYNTAX;
			}

			return ERR_OK;
		}
	}

	// Pravidlo 17: <after_id> = <def_value>

	else if (token->type == ASSIGN) {
printf("= ");
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
printf("expr ");
		return CallExpressionParser(token);
	}

	// Pravidlo 20: <def_value> -> ID ( <arg> )

	else if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}

		if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}

			if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
				return ERR_OK;
			}
			else return arg(token);
		}
	}
	else if (token->type == LEFT_ROUND_BRACKET) {
printf("expr ");
		return CallExpressionParser(token);
	}
	
	return ERR_SYNTAX;
}


int value (Token *token) {
 	//Pravidlo 21 - 25: <value> -> INTEGER | FLOAT | STRING | NIL | ID
 	if (token->type == KEYWORD && strcmp(token->attribute, "nil") == 0) {
printf("nil ");
		if (get_next_token(token) == ERR_SCANNER) {
			return ERR_SCANNER;
		}
		return ERR_OK;
	}
 	switch (token->type) {
		case INTEGER:
printf("integer ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}
			return ERR_OK;
			break;
		
		case FLOAT:
printf("float ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}
			return ERR_OK;
			break;
 		case STRING:
printf("string ");
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}
			return ERR_OK;
			break;
 		case IDENTIFIER:
printf("%s ", token->attribute);
			if (get_next_token(token) == ERR_SCANNER) {
				return ERR_SCANNER;
			}
			
			if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
				if (get_next_token(token) == ERR_SCANNER) {
					return ERR_SCANNER;
				}
 				if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
					return ERR_OK;
				}
				else return arg(token);
			}
			else return ERR_OK;
			break;
 		default:
			return ERR_SYNTAX;
	}
}
