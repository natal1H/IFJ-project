#include "parser.h"

/*	
	TODO:	Gramatika: - Zmenit gramatiku: Zmenit vstupne znaky z ) na epsilon v <arg> <params> atd., pridat <after_id> -> epsilon
			   - <arg> pre print nesedi s gramatikou (v printe musia byt arumenty, nemoze byt epsilon)
			   - pridat pravidlo <stat_list> -> epsilon
			   - vymazat stav <value> (nahradeny <expr>)
			   - pridat volanie funkcie bez zatvoriek

		Kod: - Pri volani funkcie nemusia byt okolo argumentov zatvorky

*/


#define GET_NEXT_TOKEN() if(get_next_token(token) == ERR_SCANNER) return ERR_SCANNER 

int depth_index = 0;
bool in_if_or_while = false;
bool in_def = false;
bool withoutBrackets = false;

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

	int statRetVal = 0; //Uchovava navratovu hodnotu funkcie stat

	// Pravidlo 2: <stat_list> -> <stat> EOL <stat_list>	

	if (token->type == KEYWORD) {
		if (strcmp(token->attribute, "def") == 0  && !in_if_or_while && !in_def) {	//Definícia funkcie sa nemôže nachádzať v zložených príkazoch ani v tele inej funkcie
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {
printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "if") == 0) {
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {
printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "while") == 0) {
			statRetVal = stat(token);
			if (statRetVal == ERR_OK) {
				GET_NEXT_TOKEN();

				if (token->type == EOL) {\
printf("eol\n");
					GET_NEXT_TOKEN();

					return stat_list(token);
				}
			}
		}
		else if (strcmp(token->attribute, "end") == 0) {
			if (in_def && !in_if_or_while) {
				printf("## BOLO V DEFINíCII, teraz prechádza naspäť do main ##\n");

local_table_print(*actual_function_ptr);

				// Nebolo už vo while/if, vychádza z definície funkcie
				// Sémantická akcia - presunúť ukazovať na aktuálnu lokálnu tabuľku naspäť na MAIN
				tGlobalTableNodePtr main_global_node = get_function_node(global_table, MAIN); // Vráti ukazovvateľ na uzol s MAIN v GTS
				actual_function_name = MAIN; // Nastaviť meno aktuálnej funkcie späť na MAIN
				printf("Actual function name: %s\n", actual_function_name);
				actual_function_ptr = (main_global_node->data->function_table); // Aktuálne lokálna tabuľka je nová lokálna tabuľka
				//local_table_print(*actual_function_ptr);
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
		statRetVal = stat(token);
		if (statRetVal == ERR_OK) {
			if (token->type == EOL) {
printf("eol\n");
				GET_NEXT_TOKEN();

				return stat_list(token);
			}
			
		}
	}
	else if (token->type == TYPE_EOF) {
		if (depth_index == 0) {
printf("eof\n");
			return ERR_OK;
		}
		else return ERR_SYNTAX;
	}
	
	return statRetVal;
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
			GET_NEXT_TOKEN();

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
//variable_set_defined(&new_local_table, token->attribute);
				set_function_table(&function_global_node, &new_local_table); // Naviazanie uzla v globálnej na novú lokálnu
				// Bude nasledovať definícia funkcie - preto treba zmeniť ukazovateľ na aktuálnu lokálnu TS z MAIN na tabuľku novej funkcie
				// Nastaviť actual_function_name (ID funkcie, v ktorej sa práve nachádza program) na token->attribute
				actual_function_name = token->attribute;
printf("Actual function name: %s\n", actual_function_name);
				// Nastaviť actual_function_table
				actual_function_ptr = &new_local_table; // Aktuálne lokálna tabuľka je nová lokálna tabuľka
//local_table_print(*actual_function_ptr);
				// Koniec sémantickej kontroly

				GET_NEXT_TOKEN();

				if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
					GET_NEXT_TOKEN();

					// Pravidlo 10: <params> -> epsilon

					if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");
						GET_NEXT_TOKEN();	
					}
					else {
						if (params(token) == ERR_OK) {
							GET_NEXT_TOKEN();
						}
					}

					if (token->type == EOL) {
printf("eol\n");
						GET_NEXT_TOKEN();

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
			GET_NEXT_TOKEN();

			if (CallExpressionParser(token) == ERR_OK) {
				if (strcmp(token->attribute, "then") == 0) {
printf("then ");
					GET_NEXT_TOKEN();

					if (token->type == EOL) {
printf("eol\n");
						GET_NEXT_TOKEN();
						
						depth_index++;
						in_if_or_while = true;

						if (stat_list(token) == ERR_OK) {
							if (strcmp(token->attribute, "else") == 0) {
printf("else ");
								GET_NEXT_TOKEN();

								if (token->type == EOL) {
printf("eol\n");
									GET_NEXT_TOKEN();

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
			GET_NEXT_TOKEN();

			if (CallExpressionParser(token) == ERR_OK) {
				if (strcmp(token->attribute, "do") == 0) {
printf("do ");
					GET_NEXT_TOKEN();

					if (token->type == EOL) {
printf("eol\n");
						GET_NEXT_TOKEN();
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

		// Potrebujem zálohovať ID, lebo budem brať ďalšie tokeny
		id_copy = (char *) realloc(id_copy, sizeof(char) * strlen(token->attribute));
		strcpy(id_copy, token->attribute);
printf("\n##ID COPY: %s\n", id_copy);
		
		GET_NEXT_TOKEN();

		withoutBrackets = false;

		return after_id(token);
	}

	return ERR_SYNTAX;
}






int params (Token *token) {

	// Pravidlo 8: <params> -> ID <params_next>

	if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);
printf("\n##Prvý parameter %s\n", token->attribute);

	// Sémantická akcia - nastaviť počet params funkcie na 1, vložiť param do lok. TS funkcie s typom T_PARAM
	function_set_number_params(global_table, actual_function_name, 1); // params = 1
printf("\nAktuálny počet parametrov funkcie: %d\n", function_get_number_params(global_table, actual_function_name));
	variable_set_defined(actual_function_ptr, token->attribute); // Vloženie param do loc. TS funkcie
printf("\n\n");
//local_table_print(*actual_function_ptr);
printf("\n\n");
	variable_set_type(*actual_function_ptr, token->attribute, T_PARAM);
	// Koniec sémantickej akcie

		GET_NEXT_TOKEN();

		return params_next(token);
	}
	
	return ERR_SYNTAX;
}






int params_next (Token *token) {

	// Pravidlo 10: <params_next> -> , ID <params_next>

	if (token->type == COMMA) {
printf(", ");
		GET_NEXT_TOKEN();

		if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);
printf("\n##Ďalší parameter %s\n", token->attribute);

		// Sémantická akcia - inkrementovať počet params funkcie, vložiť param do lok. TS funkcie s typom T_PARAM
		function_increase_number_params(global_table, actual_function_name); // Inc. počet params
printf("\nAktuálny počet parametrov funkcie: %d\n", function_get_number_params(global_table, actual_function_name));
		variable_set_defined(actual_function_ptr, token->attribute); // Vloženie param do loc. TS funkcie
printf("\n\n");
//local_table_print(*actual_function_ptr);
printf("\n\n");
		variable_set_type(*actual_function_ptr, token->attribute, T_PARAM);
		// Koniec sémantickej akcie

			GET_NEXT_TOKEN();

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

	int retVal = value(token);

	if (retVal != ERR_OK) {
		return retVal;
	}

	retVal = arg_next(token);

	if (retVal != ERR_OK) {
		return retVal;
	}
	
	expected_params--;
printf("\n- decreasing exp param2s\n");

	// Sémantická kontrola
	printf("\n#Expected number of params: %d\n", expected_params);
	// Kontrola počtu načítaných parametrov
	if (expected_params != 0) {
		fprintf(stderr, "Chyba! Nesprávny počet argumentov pri volaní funkcie.\n");
		return ERR_SEM_PARAM; // TODO: Skontrolovať správnosť chybového kódu!!!
	}
	// Koniec sémantickej kontroly

	return ERR_OK;
}






int arg_next (Token *token) {

	// Pravidlo 14: <arg_next> -> , <value> <arg_next>

	if (token->type == COMMA) {
printf(", ");
		GET_NEXT_TOKEN();
		
		if (value(token) == ERR_OK) {
			expected_params--;
printf("\n- decreasing exp params\n");
			return arg_next(token);
		}
	}

	// Pravidlo 15: <arg_next> -> epsilon

	else if (token->type == RIGHT_ROUND_BRACKET && !withoutBrackets) {
printf(") ");

		if (get_next_token(token) == ERR_SYNTAX) {
			return ERR_SYNTAX;
		}

		return ERR_OK;
	}
	else if (token->type == EOL && withoutBrackets) {
		return ERR_OK;
	}
	return ERR_SYNTAX;
}






int after_id (Token *token) {

	// Uložiť niekde id (momentálne v token->attribute)

	int retVal;

	// Pravidlo 16: <after_id> -> ( <arg> )
	//-------------------------------|
	// Volanie funkcie so zatvorkami |
	//-------------------------------|
	//Token obsahuje lavu zatvorku
	if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
		func_id_copy = id_copy;
		id_copy = NULL;
		withoutBrackets = false;

		// Sémantická kontrola
		if (!check_if_function_already_defined(global_table, func_id_copy)) {
			// Funkcia ešte nebola definovaná - chyba
			fprintf(stderr, "Chyba! Funkcia volaná pred jej definíciou.\n");
			return ERR_SEM_UNDEF;
		}
		expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
printf("\nExpected number params: %d\n", expected_params);
		// Koniec sémantickej kontroly

		GET_NEXT_TOKEN();

		// Pravidlo 13: <arg> -> epsilon

		if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");

			// Sémantická kontrola
			if (expected_params != 0) {
			    // Sémantická chyba - nesprávny počet parametrov
			    return ERR_SEM_PARAM;
			}
			// Koniec sémantickej kontroly

			GET_NEXT_TOKEN();

			return ERR_OK;

		}
		else {
			retVal = arg(token);
	
			if (retVal == ERR_OK) {
				GET_NEXT_TOKEN();

				return ERR_OK;
			}
		}

	}
	//--------------------------------------------|
	//Volanie funkcie S argumentami BEZ zatvoriek |
	//--------------------------------------------|
	//Token obsahuje prvy argument funkcie
	else if (token->type == IDENTIFIER || token->type == INTEGER || token->type == FLOAT || token->type == STRING) {	
		func_id_copy = id_copy;
		id_copy = NULL;
		withoutBrackets = true;

     	expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
        printf("\nVolanie bez zátvoriek: Expected number params: %d\n", expected_params);

		
		return arg(token);
	}
	//---------------------------------------------|
	//Volanie funkcie bez argumentov bez zatvoriek |
	//---------------------------------------------|
	//Token obsahuje EOL
	else if (token->type == EOL) {
		func_id_copy = id_copy;
		id_copy = NULL;

		expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
		
		if (expected_params == 0) {
			return ERR_OK;
		}
		else return ERR_SEM_PARAM;
	}


	// Pravidlo 17: <after_id> = <def_value>

	else if (token->type == ASSIGN) {
printf("= ");
printf("\n\tID COPY: %s\n", id_copy);	
		// Sémantická akcia
		// Definovať premennú
		printf("\n\t---PRIDAVAM DO STROMU %s\n", id_copy);
		variable_set_defined(actual_function_ptr, id_copy);
        	// Koniec sémantickej akcie
        	// Generovanie kódu
        	gen_defvar(id_copy, false);
        	printf("\n\t---Teraz by sa definovalo %s\n", id_copy);
        	// Koniec

		GET_NEXT_TOKEN();

		return def_value(token);
	}
	
	return retVal;
}






int def_value (Token *token) {

	int retVal;

	// Pravidlo 19: <def_value> -> <expr>

	if (token->type == INTEGER || token->type == FLOAT || token->type == STRING || token->type == LEFT_ROUND_BRACKET || 
			(token->type == KEYWORD && strcmp(token->attribute, "nil") == 0 )) {
printf("expr ");

		retVal = CallExpressionParser(token);
printf("\n\t\tId copy: %s\n", id_copy);
		if (retVal == ERR_OK) {
			// Sémantická akcia:
			printf("\n\tNASTAVENIE HODNOTY %s na %d\n", id_copy, typeFinal);
			variable_set_type(*actual_function_ptr, id_copy, typeFinal);
printf("\n\tFinalVar: %s\n", finalVar);
			// Koniec sémantickej akcie
			// Generovanie kódu
			// Presun výsledku výrazu do premennej (MOVE id_copy finalVar)
			gen_move_var(id_copy, finalVar, false);
			// Koniec generovania kódu
		}
	}

	else if (token->type == IDENTIFIER) {
printf("%s ", token->attribute);
		// Pravidlo 20: <def_value> -> ID ( <arg> )
        // Kontrola, či sa jedná o definovanu funkciu. Ak nie poslať hneď parseru výrazov

        if (check_if_function_already_defined(global_table, token->attribute)) {
        	expected_params = function_get_number_params(global_table, token->attribute); // Získaj počet params funkcie
printf("\nExpected number params: %d\n", expected_params);
            // Koniec sémantickej kontroly

			// Potrebujem zálohovať ID, lebo budem brať ďalšie tokeny
			func_id_copy = (char *) realloc(id_copy, sizeof(char) * strlen(token->attribute));
			strcpy(func_id_copy, token->attribute);
            
            GET_NEXT_TOKEN();

			//-------------------------------|
			// Volanie funkcie so zatvorkami |
			//-------------------------------|
			//Token obsahuje lavu zatvorku            
			if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
				withoutBrackets = false;

				GET_NEXT_TOKEN();	

				// Pravidlo 13: <arg> -> epsilon

				if (token->type == RIGHT_ROUND_BRACKET) {
printf(") ");

					// Sémantická kontrola
					if (expected_params != 0) {
					    // Sémantická chyba - nesprávny počet parametrov
					    return ERR_SEM_PARAM;
					}
					// Koniec sémantickej kontroly

					GET_NEXT_TOKEN();

					return ERR_OK;
				}
				else {
					retVal = arg(token);
		
					if (retVal == ERR_OK) {
						GET_NEXT_TOKEN();

						return ERR_OK;
					}
				}
			}
			//--------------------------------------------|
			//Volanie funkcie S argumentami BEZ zatvoriek |
			//--------------------------------------------|
			//Token obsahuje prvy argument funkcie
			else if (token->type == IDENTIFIER || token->type == INTEGER || token->type == FLOAT || token->type == STRING) {
				withoutBrackets = true;	

	       		expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
	        	printf("\nVolanie bez zátvoriek: Expected number params: %d\n", expected_params);
		
				return arg(token);
			}
			//---------------------------------------------|
			//Volanie funkcie bez argumentov bez zatvoriek |
			//---------------------------------------------|
			//Token obsahuje EOL
			else if (token->type == EOL) {
			//TODO Volanie funkcie bez parametrov bez zatvoriek
				expected_params = function_get_number_params(global_table, func_id_copy); // Získaj počet params funkcie
			
				if (expected_params == 0) {
					return ERR_OK;
				}
				else return ERR_SEM_PARAM;
			}
	    }
        else {
            // Výraz začínajúci premennou -> zavolať parser výrazov
            retVal = CallExpressionParser(token);
            printf("\n\t\tId copy: %s\n", id_copy);
            // Sémantická akcia:
            printf("Type final: %d\n", typeFinal);
            variable_set_type(*actual_function_ptr, id_copy, typeFinal);
            // Koniec sémantickej akcie
        }
	}
	else if (token->type == LEFT_ROUND_BRACKET) {
printf("expr ");
        retVal = CallExpressionParser(token);
        printf("\n\t\tId copy: %s\n", id_copy);
        // Sémantická akcia:
        variable_set_type(*actual_function_ptr, id_copy, typeFinal);
        // Koniec sémantickej akcie
	}
	
	return retVal;
}


int value (Token *token) {
 	//Pravidlo 21 - 25: <value> -> INTEGER | FLOAT | STRING | NIL | ID
 	if (token->type == KEYWORD && strcmp(token->attribute, "nil") == 0) {
printf("nil ");
		GET_NEXT_TOKEN();
		return ERR_OK;
	}
 	switch (token->type) {
		case INTEGER:
printf("integer ");
			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
		
		case FLOAT:
printf("float ");
			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
 		case STRING:
printf("string ");
			GET_NEXT_TOKEN();
			return ERR_OK;
			break;
 		case IDENTIFIER:
printf("%s ", token->attribute);

			// Sémantická kontrola
			// Premenná musí byť už definovaná
			tLocalTableNodePtr var_node = get_variable_node(*(actual_function_ptr) ,token->attribute);
			if (var_node == NULL) {
				// Neexistuje uzol s premennou -> nebola ešte definovaná - chyba
				fprintf(stderr, "Chyba! Nedefinovaná premenná.\n");
				return ERR_SEM_UNDEF;
			}
			// Koniec sémantickej kontroly

			GET_NEXT_TOKEN();
			
			if (token->type == LEFT_ROUND_BRACKET) {
printf("( ");
				GET_NEXT_TOKEN();
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
