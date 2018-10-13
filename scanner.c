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

int get_next_token() {

  tstring_clear_string(read_string); // str := '';

  TState state = START; // Nastavenie počiatočného stavu v DKA
  int c;

  while ((c = getc(stdin)) != EOF) {

    printf("Reading: %c, (%d)\n", c, (int) c);
    printf("State: %d\n", state);
    printf("Je alpha: %d\n", scanner_is_alpha(c));

    switch(state) {
      case START: // Starting point
        printf("START\n");

        if (scanner_is_alpha(c)) {
          // START -> F_ID
          printf("START -> F_ID\n");

          state = F_ID;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (scanner_is_number(c)) {
          // START -> F_INT
          printf("START -> F_INT\n");

          state = F_INT;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }

        break;
      case F_ID:
        printf("F_ID\n");

        if (scanner_is_alpha(c) || scanner_is_number(c)) {
          // F_ID -> F_ID
          printf("F_ID -> F_ID\n");


          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c);
        }
        else {
          printf("ELSE\n");

          // Znak mimo povolených pre identifikátor

          // unget znak

          // zisti, či ten string je medzi kľúčovými slovami

          // if (je_klucove)
          //    TOKEN = get_keyword(str);
          // else
          //    TOKEN = IDENTIFIER
          printf("INDENTIFIKATOR\n");

          break;
        }

        break;
    }

  }

  return 0;
}

// MAIN - only for temp testing
int main(int argc, char** argv) {
  printf("SCANNER\n");

  for (int i = 0; i < argc; i++) {
    printf("Arg no.%d:\t%s\n", i, argv[i]);
  }

  // inicializuj scanner najprv cez scanner_initialize()

  if ( scanner_initialize() != 0 ) {
    // chyba pri inicializácii
    printf("Chyba pri inicializácii scannera");
    return -1; // TODO return actual error code
  }

  get_next_token();


  // po skončení práce uvoľni miesto po read_string
  tstring_free_struct(read_string);

  return 0;
}
