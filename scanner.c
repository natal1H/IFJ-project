#include "scanner.h"

#define scanner_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define scanner_is_number(c) (c >= '0' && c <= '9')


int scanner_initialize() {
  // Inicializuj read_string, do ktorého sa budú pri načítaní tokenov ukladať znaky
  read_string = tstring_struct_initialize();

  if (read_string == NULL) {
    return -1; // TODO: return actual error code
  }
  else {
    return 0;
  }
}

Token *token_initialize() {
  // Alokuj miesto pre štruktúru token
  Token *token = (Token *) malloc(sizeof(Token));
  if (token == NULL) {
    return NULL;
  }

  token->attribute = NULL;

  return token;
}

void token_free(Token *token) {
  // TODO: Why? Prečo to vyhadzuje seg fault
 // printf("=1\n");
  //if (token->attribute == NULL)
  //  printf(" je null\n");
  //else
  //  printf(" nie je null\n");
  //free(token->attribute);
 // printf("=2\n");
  free(token);
}

int token_set_type_attribute(Token *token, Token_Type type, char *attribute) {
  // Token nesmie byť neinicializovaný
  if (token == NULL) {
    return -1; // TODO: return actual error code
  }
  // Nastav token type
  token->type = type;

  // Nastav attribute - treba alokovať miesto pre string
  token->attribute = (char *) malloc(sizeof(char) * strlen(attribute));
  if (token->attribute == NULL) {
    return -1; // TODO: return actual error code
  }

  // Skopírovať string do token->attribute
  strcpy(token->attribute, attribute);

  return 0;
}

void print_token(Token *token) {
  printf("Token type: %d\n", token->type);
  printf("Token attribute: %s\n", token->attribute);
}

int get_next_token(Token *token) {

  tstring_clear_string(read_string); // str := '';

  TState state = START; // Nastavenie počiatočného stavu v DKA
  int c = getc(stdin);

  do {

    switch(state) {
      case START: // Starting point

        if (scanner_is_alpha(c) || c == (int) '_') {
          // START -> F_ID

          state = F_ID;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (scanner_is_number(c)) {
          // START -> F_INT

          state = F_INT;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '=') {
          // START -> F_ASSIGN
          state = F_ASSIGN;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '+') {
          // START -> F_ADDITION

          printf("ADDITION\n");

          token = token_initialize();
          token_set_type_attribute(token, ADDITION, "");

          print_token(token);

          return 0;
        }
        else if (c == (int) '*') {
          // START -> F_MULTIPLICATION

          printf("MULTIPLICATION\n");

          token = token_initialize();
          token_set_type_attribute(token, MULTIPLICATION, "");

          print_token(token);

          return 0;
        }
        else if (c == (int) '-') {
          // START -> F_SUBSTRACTION

          printf("SUBTRACTION\n");

          token = token_initialize();
          token_set_type_attribute(token, SUBTRACTION, "");

          print_token(token);

          return 0;
        }
        else if (c == (int) '/') {
          // START -> F_DIVISION

          printf("DIVISION\n");

          token = token_initialize();
          token_set_type_attribute(token, DIVISION, "");

          print_token(token);

          return 0;
        }
        else if (c == (int) '<') {
          // START -> F_LESS
          state = F_LESS;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '>') {
          // START -> F_GREATER
          state = F_GREATER;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }

        break;
      case F_ID:

        if (scanner_is_alpha(c) || scanner_is_number(c) || c == (int) '_') {
          // F_ID -> F_ID

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c);
        }
        else {
          // Znak mimo povolených pre identifikátor

          // unget znak
          ungetc(c, stdin);

          // zisti, či ten string je medzi kľúčovými slovami

          // if (je_klucove)
          //    TOKEN = get_keyword(str);
          // else
          //    TOKEN = IDENTIFIER
          printf("IDENTIFIKATOR\n");

          token = token_initialize();
          token_set_type_attribute(token, IDENTIFIER, read_string->string);

          print_token(token);

          return 0;
        }

        break;
      case F_INT:
        if (scanner_is_number(c)) {
          // F_INT -> F_INT
        }
        else {
          ungetc(c, stdin);

          printf("INT\n");

          token = token_initialize();
          token_set_type_attribute(token, INTEGER, read_string->string);

          print_token(token);

          return 0;
        }
        break;
    }


  //}
  } while((c = getc(stdin)) != EOF);


  // TODO: pridať tu ošetrenie, že c sa dostalo až na EOF
  // Nech pozrie na koncový stav, ak je v F_nieco, nech vytvorí token s príslušným typom a do attribute nahrá zostatok v read_string

  return 0;
}

// MAIN - only for temp testing
int main(int argc, char** argv) {
  printf("SCANNER TEST\n");


  // inicializuj scanner najprv cez scanner_initialize()


  if ( scanner_initialize() != 0 ) {
    // chyba pri inicializácii
    printf("Chyba pri inicializácii scannera");
    return -1; // TODO return actual error code
  }

  Token *token;
  get_next_token(token);

  token_free(token);
  get_next_token(token);


  token_free(token);
  get_next_token(token);

  // po skončení práce uvoľni miesto po read_string
  tstring_free_struct(read_string);

  return 0;
}

// TODO: Na konci musi byť \n aby zobralo posledny token - fix it later!!!!