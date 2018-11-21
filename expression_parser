//--------------INCLUDE--------------
#include "expression_parser.h"
#include "scanner.h"
#include "stringlib.h"
# include <stdio.h>

/* -------------------------------------------------------------------------- */
void InitStack (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

//void PushStack (tStackP *S, char ptr)
void PushStack (tStackP *S, struct token ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

//char PopStack (tStackP *S)
struct token PopStack (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }
}

bool IsStackEmpty (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */

void InitStackINT (tStackINT *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

//void PushStack (tStackP *S, char ptr)
void PushStackINT (tStackINT *S, long ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

//char PopStack (tStackP *S)
long PopStackINT (tStackINT *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }
}

bool IsStackEmptyINT (tStackINT *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */



/**
 * @param Token Vystup z scannera
 * @param L Buffer/Obojsmerny zoznam
 * @note Ulozi cely kontrolovany vyraz do bufferu
 */
void LoadToBuffer(Token *Token, tDLList *ExprList) {

    tDLElemPtr Expr = malloc(sizeof(struct tDLElem)); //Alokovanie prvku
    if (Expr == NULL) { //Ak sa nepodarila alokacia
        fprintf(stderr, "Memory is full");
        return;
    } else {

        if (ExprList->First == NULL) {
            (*Expr).Token = *Token; //inicializacia noveho prvku
            ExprList->First = Expr;        //prvy v zozname ak je prvy prvok
            ExprList->Last = Expr;         //zaroven ako posledny
            ExprList->Act = Expr;
        } else {
            (*Expr).Token = *Token;         //inicializacia noveho prvku
            ExprList->Last->next = Expr;           //pridanie do zoznamu na koniec
            ExprList->Last->next->prev = ExprList->Last;  //obojsmerne previazanie
            ExprList->Last = Expr;                 //nastavenie posledneho prvku
        }
    }
}

/**
 * Vykona operaciu pri vypocitavani postfixovej notacie
 * @param token_ID1 Prvy operand
 * @param token_OP Operacia
 * @param token_ID2 Druhy operand
 * @return Vrati vysledok danej operacie
 */
long EvaluateNow(long token_ID1, Token token_OP, long token_ID2 ){

    switch(token_OP.type){

        case (ADDITION) : {
            return token_ID1+token_ID2;
        }

        case (MULTIPLICATION) : {
            return token_ID1*token_ID2;
        }

        default:{}
    }
}

/**
 * Vypocita vyraz v postfixovej notacii
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture - TODO momentalne nepotebne
 * @param stackPostfix Zasobnik s ulozenou postfixou notaciou
 * @param stackTemp Pomocny zasobnik pre pretocenie poradia v postfixovom zasobniku
 * @param stackOutputINT Vystupny zasobnik v ktorom je pocitana postfixova notacia
 * @param last_operation Posledna operacia - momentalne nepotrebne
 * @return Vracia vysledok v datovom type long
 */
long EvaluateFromPostfix(tDLList *ExprList, tStackP *stackPostfix, tStackP *stackTemp, tStackINT *stackOutputINT , int last_operation) {
    printf("tu");

    //Obratenie zasobnika, najskor POPnut potrebujeme operandy ktore su na spodku zasobnika
    while(stackPostfix->top > 0){
        PushStack(stackTemp, PopStack(stackPostfix));
    }
    stackPostfix = stackTemp;

    while(stackPostfix->top > 0){
        Token tmp;                    //Pomocny token
        tmp = PopStack(stackPostfix); //Zisti hodnotu na vrchole zasobni

            if(TransformTable[tmp.type] == id_pTable) { //Ak ide o ID
                char *ptr;  //Buffer k pretypovaniu
                PushStackINT(stackOutputINT, strtol(tmp.attribute, &ptr, 10));
            } else {
                PushStackINT(stackOutputINT, EvaluateNow(PopStackINT(stackOutputINT), tmp, PopStackINT(stackOutputINT)));
            }
    }
    return PopStackINT(stackOutputINT);
}

/**
 * Spracuje infixovy vyraz do postfixovej notacie pomocou precedencnej tabulky
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stack Pomocny zasobnik kde sa vyhodnocuje priorita operatorov
 * @param stackOutput Vystupny zasobnik s postfixovou notaciou
 * @param last_operation Posledna vykonana operacia - TODO nepotrebne momentane
 * @return Vracia zasobnik s postfixou notaciou
 */
tStackP ParseToPostfix(tDLList *ExprList, tStackP *stack, tStackP *stackOutput, int last_operation) {

    bool UvolnovanieZasobnika = false;
    Token tmp;              //Pomocny token
    tmp.type = EOL;         //Koniec zasobnika ako EOL
    PushStack(stack, tmp);  //Informuje o konci zasobnika

//    ExprList->Act = ExprList->First;

    while (ExprList->Act != NULL && ExprList->Act->Token.type != EOL) { //Pokym nie je koniec vyrazu(zoznamu) TODO Token.Next == NULL

                tmp = PopStack(stack); //Zisti hodnotu na vrchole zasobnika
                PushStack(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku

                if(TransformTable[ExprList->Act->Token.type] == id_pTable){ //Ak je to operand
                    PushStack(stackOutput, ExprList->Act->Token);           //Potom push do vysledneho zasobnika
                } else {

                    //Pokym je vacsia priorita
                    while (precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '>') {

                        //Ak sa token na zasobniku nerovna '(' alebo ')'
                        //Zatvorky nepushujeme do vysledneho vyrazu!
                        if(TransformTable[tmp.type] != left_round_bracket_pTable && TransformTable[tmp.type] != right_round_bracket_pTable) {
                            PushStack(stackOutput, PopStack(stack)); //Prehod token do vysledneho zasobnika
                        } else {
                            PopStack(stack);    //Len odober token zo zasobnika, len zatvorky!
                        }

                        tmp = PopStack(stack); //Zisti hodnotu na vrchole zasobnika
                        PushStack(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku
                        UvolnovanieZasobnika = true;

//                      if(precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '<') {
//                          PushStack(stack, ExprList->Act->Token);
//                      }
                    }
                    if(precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '=' ){
                        PopStack(stack);
//                    } else if (1) {
                    } else {
                        PushStack(stack, ExprList->Act->Token);
                    }
                }
                UvolnovanieZasobnika = false;
                ExprList->Act = ExprList->Act->next; //Dalsi Token
    }


    while(stack->top > 1){
//        tmp = PopStack(stack);
//        if (tmp.type != LEFT_ROUND_BRACKET && tmp.type != RIGHT_ROUND_BRACKET) {
            PushStack(stackOutput, PopStack(stack));
//        }
    }
    for(int i = 1; i<=stackOutput->top; i++) {
        print_token(&(stackOutput)->stack[i]);
    }

    return *stackOutput;
}


struct tDLElem FindRule(tDLList *ExprList, bool *SyntaxError, int *NumberOfLeftBracket, int *NumberOfRightBracket, int *NumberOfID, int *NumberOfOperator) {


    //TODO Moze existovat tvar 'ID ) <' ?


//----------------------------------------------------------------------------------------------

    //E ->   ID  OPERACIA  ID  -> Priklad: A+B
    if ( ((    TransformTable[ExprList->Act->Token.type]             == id_pTable)           &&
             ((TransformTable[ExprList->Act->next->Token.type]   == addition_pTable)         ||
              (TransformTable[ExprList->Act->next->Token.type]   == substraction_pTable)     ||
              (TransformTable[ExprList->Act->next->Token.type]   == multiplication_pTable)   ||
              (TransformTable[ExprList->Act->next->Token.type]   == division_pTable)         ||
              (TransformTable[ExprList->Act->next->Token.type]   == equals_pTable)           ||
              (TransformTable[ExprList->Act->next->Token.type]   == not_equals_pTable)       ||
              (TransformTable[ExprList->Act->next->Token.type]   == less_pTable)             ||
              (TransformTable[ExprList->Act->next->Token.type]   == less_or_equals_pTable)   ||
              (TransformTable[ExprList->Act->next->Token.type]   == greater_pTable)          ||
              (TransformTable[ExprList->Act->next->Token.type]   == grater_or_equals_pTable) ||
              (TransformTable[ExprList->Act->next->Token.type]   == not_equals_pTable))      &&
               TransformTable[ExprList->Act->next->next->Token.type] == id_pTable)) {

                    if(ExprList->Act->next->next->next != NULL) { //Cize posledne ID
                        *NumberOfID += 1;
                    } else {
                        *NumberOfID += 2;
                    }
                    *NumberOfOperator += 1;

                    ExprList->Act = ExprList->Act->next->next;
                    return *ExprList->Act; //Najdene
    }


    //E ->   R_ZATZATVORKA  OPERACIA  L_ZATVOVORKA  -> Priklad: (ID)
    if ( TransformTable[ExprList->Act->Token.type]             == right_round_bracket_pTable   &&
         TransformTable[ExprList->Act->next->Token.type]       == id_pTable                    &&
         TransformTable[ExprList->Act->next->next->Token.type] == left_round_bracket_pTable){

                    *NumberOfLeftBracket += 1;
                    *NumberOfRightBracket += 1;
                    *NumberOfID += 1;

                    ExprList->Act = ExprList->Act->next->next;
                    return *ExprList->Act; //Najdene
    }

    //E ->   ID  R_ZATVORKA  OPERACIA  -> Priklad: ID)+
    if (   TransformTable[ExprList->Act->Token.type]             == id_pTable                  &&
           TransformTable[ExprList->Act->next->Token.type]       == right_round_bracket_pTable &&
         (
          (TransformTable[ExprList->Act->next->next->Token.type]   == addition_pTable)         ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == substraction_pTable)     ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == multiplication_pTable)   ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == division_pTable)         ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == equals_pTable)           ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == not_equals_pTable)       ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == less_pTable)             ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == less_or_equals_pTable)   ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == greater_pTable)          ||
          (TransformTable[ExprList->Act->next->next->Token.type]   == grater_or_equals_pTable)
         )){

                    *NumberOfID += 1;
                    *NumberOfRightBracket += 1;
                    *NumberOfOperator += 1;

                    ExprList->Act = ExprList->Act->next->next;
                    return *ExprList->Act; //Najdene
    }

    //E ->   OPERACIA  L_ZATVORKA  ID -> Priklad: +(ID
    if ((
                 (TransformTable[ExprList->Act->Token.type]   == addition_pTable)               ||
                 (TransformTable[ExprList->Act->Token.type]   == substraction_pTable)           ||
                 (TransformTable[ExprList->Act->Token.type]   == multiplication_pTable)         ||
                 (TransformTable[ExprList->Act->Token.type]   == division_pTable)               ||
                 (TransformTable[ExprList->Act->Token.type]   == equals_pTable)                 ||
                 (TransformTable[ExprList->Act->Token.type]   == not_equals_pTable)             ||
                 (TransformTable[ExprList->Act->Token.type]   == less_pTable)                   ||
                 (TransformTable[ExprList->Act->Token.type]   == less_or_equals_pTable)         ||
                 (TransformTable[ExprList->Act->Token.type]   == greater_pTable)                ||
                 (TransformTable[ExprList->Act->Token.type]   == grater_or_equals_pTable))      &&
                  TransformTable[ExprList->Act->next->Token.type]  == left_round_bracket_pTable &&
                  TransformTable[ExprList->Act->next->next->Token.type]    == id_pTable
         ){

                    *NumberOfOperator += 1;
                    *NumberOfLeftBracket +=1;
                    *NumberOfID +=1;

        ExprList->Act = ExprList->Act->next->next;
        return *ExprList->Act; //Najdene
    }

    //E ->   ID  OPERACIA  L_ZATVORKA -> Priklad: ID+(
    if ((
        TransformTable[ExprList->Act->Token.type]          == id_pTable                 &&
       ((TransformTable[ExprList->Act->next->Token.type]  == addition_pTable)           ||
        (TransformTable[ExprList->Act->next->Token.type]   == substraction_pTable)      ||
        (TransformTable[ExprList->Act->next->Token.type]   == multiplication_pTable)    ||
        (TransformTable[ExprList->Act->next->Token.type]   == division_pTable)          ||
        (TransformTable[ExprList->Act->next->Token.type]   == equals_pTable)            ||
        (TransformTable[ExprList->Act->next->Token.type]   == not_equals_pTable)        ||
        (TransformTable[ExprList->Act->next->Token.type]   == less_pTable)              ||
        (TransformTable[ExprList->Act->next->Token.type]   == less_or_equals_pTable)    ||
        (TransformTable[ExprList->Act->next->Token.type]   == greater_pTable)           ||
        (TransformTable[ExprList->Act->next->Token.type]   == grater_or_equals_pTable)) &&
         TransformTable[ExprList->Act->next->next->Token.type]         == left_round_bracket_pTable
            )){

                *NumberOfID +=1;
                *NumberOfOperator +=1;
                *NumberOfLeftBracket +=1;

        ExprList->Act = ExprList->Act->next->next;
        return *ExprList->Act; //Najdene
    }

    //E ->   L_ZATVORKA  ID  OPERAND -> Priklad: (ID+
    if ((
                  TransformTable[ExprList->Act->Token.type]          == left_round_bracket_pTable &&
                  TransformTable[ExprList->Act->next->Token.type]         == id_pTable &&
                ((TransformTable[ExprList->Act->next->next->Token.type]  == addition_pTable)           ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == substraction_pTable)      ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == multiplication_pTable)    ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == division_pTable)          ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == equals_pTable)            ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == not_equals_pTable)        ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == less_pTable)              ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == less_or_equals_pTable)    ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == greater_pTable)           ||
                 (TransformTable[ExprList->Act->next->next->Token.type]   == grater_or_equals_pTable))

    )){
                *NumberOfLeftBracket +=1;
                *NumberOfID +=1;
                *NumberOfOperator +=1;

        ExprList->Act = ExprList->Act->next->next;
        return *ExprList->Act; //Najdene
    }

    //E ->   OPERANT ID R_ZATVORKA -> Priklad: +ID)
    if ((
            ((TransformTable[ExprList->Act->Token.type]  == addition_pTable)           ||
             (TransformTable[ExprList->Act->Token.type]   == substraction_pTable)      ||
             (TransformTable[ExprList->Act->Token.type]   == multiplication_pTable)    ||
             (TransformTable[ExprList->Act->Token.type]   == division_pTable)          ||
             (TransformTable[ExprList->Act->Token.type]   == equals_pTable)            ||
             (TransformTable[ExprList->Act->Token.type]   == not_equals_pTable)        ||
             (TransformTable[ExprList->Act->Token.type]   == less_pTable)              ||
             (TransformTable[ExprList->Act->Token.type]   == less_or_equals_pTable)    ||
             (TransformTable[ExprList->Act->Token.type]   == greater_pTable)           ||
             (TransformTable[ExprList->Act->Token.type]   == grater_or_equals_pTable)) &&
              TransformTable[ExprList->Act->next->Token.type]          == id_pTable &&
              TransformTable[ExprList->Act->next->next->Token.type]    == right_round_bracket_pTable

    )){

                *NumberOfOperator +=1;
                *NumberOfID +=1;
                *NumberOfRightBracket +=1;

        ExprList->Act = ExprList->Act->next->next;
        return *ExprList->Act; //Najdene
    }


    //E ->   R_ZATVORKA OP ID -> Priklad: )+ID
    if ((
              TransformTable[ExprList->Act->Token.type]          == right_round_bracket_pTable &&
                      ((TransformTable[ExprList->Act->next->Token.type]  == addition_pTable)           ||
                       (TransformTable[ExprList->Act->next->Token.type]   == substraction_pTable)      ||
                       (TransformTable[ExprList->Act->next->Token.type]   == multiplication_pTable)    ||
                       (TransformTable[ExprList->Act->next->Token.type]   == division_pTable)          ||
                       (TransformTable[ExprList->Act->next->Token.type]   == equals_pTable)            ||
                       (TransformTable[ExprList->Act->next->Token.type]   == not_equals_pTable)        ||
                       (TransformTable[ExprList->Act->next->Token.type]   == less_pTable)              ||
                       (TransformTable[ExprList->Act->next->Token.type]   == less_or_equals_pTable)    ||
                       (TransformTable[ExprList->Act->next->Token.type]   == greater_pTable)           ||
                       (TransformTable[ExprList->Act->next->Token.type]   == grater_or_equals_pTable)) &&

                      TransformTable[ExprList->Act->next->next->Token.type]    == id_pTable

    )){

        *NumberOfRightBracket +=1;
        *NumberOfOperator +=1;
        *NumberOfID +=1;

        ExprList->Act = ExprList->Act->next->next;
        return *ExprList->Act; //Najdene
    }

    fprintf(stderr,"Syntakticka chyba, nenajdene pravidlo");
    *SyntaxError = true;
    return *ExprList->Act;
}


bool Rules(tDLList *ExprList) {

    if(ExprList == NULL || ExprList->First == NULL){
        fprintf(stderr,"Chyba");
    }
    else {
        if (ExprList->First->next == NULL) { //Len jedina const/ID
            //TODO

        } else {                             //Vacsi vyraz
            //Inicializacia temporary
            struct tDLElem tmp;

            bool SyntaxError = false;
            bool *SyntaxErroPtr = &SyntaxError;

            int NumberOfLeftBrackets = 0;
            int *NumberOfLeftBracketsPtr = &NumberOfLeftBrackets;

            int NumberOfRightBrackets = 0;
            int *NumberOfRightBracketsPtr = &NumberOfRightBrackets;

            int NumberOfID = 0;
            int *NumberOfIDPtr = &NumberOfID;

            int NumberOfOperator = 0;
            int *NumberOfOperatorPtr = &NumberOfOperator;

            //Kontrolujeme syntax od zaciatku zoznamu
            ExprList->Act = ExprList->First;

            //Kontrola vyrazov podla dostupnych pravidiel
            do {
                tmp = FindRule(ExprList, SyntaxErroPtr, NumberOfLeftBracketsPtr, NumberOfRightBracketsPtr, NumberOfIDPtr, NumberOfOperatorPtr); //Hlada pravidlo

                //Kym nie je na konci riadku alebo vyrazu
            } while (!((tmp.next == NULL) || tmp.next->Token.type == EOL) && SyntaxError == false);

            //Bola najdena syntakticka chyba?
            if(SyntaxError  == false //&&
//               NumberOfID%2 == 0 &&
//               ((NumberOfOperator%2)+1) == 1 &&
//               NumberOfLeftBrackets == NumberOfRightBrackets
               ) {
                return true; //Bez chyby
            } else {
                return false; //Najdena syntakticka chyba
            }
        }
    }
}


bool MainSyntaxCheck(tDLList *ExprList) {
    if (ExprList == NULL) {
        return false; //Ziadny vyraz TODO osetrit specialny stav
    } else {

        //Kontrola syntaxe vyrazu
        bool syntaxStatus = false;

        tDLList *tmp;
        tmp = ExprList;
        tmp->Act = tmp->First;


        //Skontroluje ci sa nachadza vo vyraze iba jeden symbol
        //Symbol moze byt == != > >= < <=
        //Jeho miesto sa ulozi do strukturu -> rozdelenie do dvoch vyrazov
        //TODO Urobit z tohoto funkciu

        // symbol = 0 -> Ziadny symbol
        // symbol = 1 -> Najdene '=='
        // symbol = 2 -> Najdene '!='
        // symbol = 3 -> Najdene '<'
        // symbol = 4 -> Najdene '<='
        // symbol = 5 -> Najdene '>'
        // symbol = 6 -> Najdene '>='
//        int symbol = 0;
        //Pri spravnom vyraze musi byt vzdy maximalne 1!
        int symbolCounter = 0;

        tmp->SyntaxChecker = tmp->First;
        while ( !((tmp->SyntaxChecker == NULL) || tmp->SyntaxChecker->Token.type == EOL))
        {
            switch (tmp->SyntaxChecker->Token.type){
                case(EQUALS) : {
//                    symbol = 1;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                case(NOT_EQUALS) : {
//                    symbol = 2;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                case(LESS) : {
//                    symbol = 3;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                case(LESS_OR_EQUALS) : {
//                    symbol = 4;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                case(GREATER) : {
//                    symbol = 5;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                case(GREATER_OR_EQUALS) : {
//                    symbol = 6;
                    symbolCounter++;
                    tmp->Symbol = tmp->SyntaxChecker;
                    break;
                }
                default: {
                }
            }//End switch
            tmp->SyntaxChecker = tmp->SyntaxChecker->next;
        }

        if(symbolCounter > 1){
            fprintf(stderr, "Error");
            return false; //syntaxStatus
        }



        syntaxStatus =  Rules(ExprList);
//        printf("%d", syntaxStatus);



        int static last_operation = no_operation_pTable; //TODO Nepouzivane

        //Alokacia zasobnika
        tStackP *stack = malloc(sizeof(tStackP));
        tStackP *stackOutput = malloc(sizeof(tStackP));
        tStackINT *stackOutputINT = malloc(sizeof(tStackINT));

        //Inicializacia zasobnika
        InitStack(stack);
        InitStack(stackOutput);
        InitStackINT(stackOutputINT);


        //Rozlozenie vyrazu do postfixovej notacie
        ExprList->Act = ExprList->First;
        *stack = ParseToPostfix(ExprList, stack, stackOutput, last_operation);
        //Re-Inicializacia(Vycistenie) zasobnika
        InitStack(stackOutput);

        //Vysledok daneho vyrazu TODO Zmenit
        long vysledok = 0;
        vysledok = EvaluateFromPostfix(ExprList, stack, stackOutput, stackOutputINT , last_operation);
        printf("%ld", vysledok);


//        *stack = UseLanguageRules(ExprList, stack, last_operation);
        return syntaxStatus;
    }
}




//
////Parameter vysledneho bude get_next_token
//int main(int argc, char *argv[]) {
//
//    printf("EXPRS PARSER TEST\n");
//    freopen("input.txt","r",stdin);
//
//    // inicializuj scanner najprv cez scanner_initialize()
//    if ( scanner_initialize() != 0 ) {
//        // chyba pri inicializácii
//        printf("Chyba pri inicializácii scannera");
//        return -1; // TODO return actual error code
//    }
//
//
//    Token *token =  token_initialize();
////    expressionPtr Expr = InitExpression();
//    tDLList *ExprArray = malloc(sizeof(tDLList));
//
//    int ret = 0;
//    //TODO ak vrati error scanner potom vratit ten error
//    while ( ( ret = get_next_token((token = token_initialize()))) != EOF) {
////                print_token(token);
//        LoadToBuffer(token, ExprArray);
//        //        token_free(token);
//    }
//    bool MainSyntaxStatus = false;
//    MainSyntaxStatus = MainSyntaxCheck(ExprArray);
//
//    //        printf("RET: %d\n", ret);
////        print_token(token);
////        if (token->type == INTEGER) {
////            printf("integer\n");
////        }
////        token_free(token);
////        first_negative_number = false; //Po vypisani vzdy nasleduje druhy znak, navzdy false
////    }
//
////    token_free(token);
//
//    // po skončení práce uvoľni miesto po read_string
////    tstring_free_struct(read_string);
//
//    return MainSyntaxStatus;
//}
