#ifndef EXPRESSION_PARSER_EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_EXPRESSION_PARSER_H

#include "scanner.h"

#define table_size 25
#define MAXSTACK 50                      /* maximální počet prvků v zásobníku */


typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */
    struct token Token;                                            /* užitečná data */
    struct tDLElem *prev;          /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *next;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr SyntaxChecker;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Symbol;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    struct token stack[MAXSTACK];
    int top;
} tStackP;

typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    long stack[MAXSTACK];
    int top;
} tStackINT;

typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    char* stack[MAXSTACK];
    int top;
} tStackCHAR;


//------PRECEDENCE_TABLE_ENUM--------

typedef enum precedence_table {
    unary_pTable,               //0
    not_pTable,                 //1
    multiplication_pTable,      //2
    nepouzivane_subtraction_pTable,         //3
    division_pTable,            //4
    mod_pTable,                 //5
    and_pTable,                 //6
    addition_pTable,            //7
    substraction_pTable,        //8
    or_pTable,                  //9
    xor_pTable,                 //10
    equals_pTable,              //11
    not_equals_pTable,          //12
    less_pTable,                //13
    less_or_equals_pTable,      //14
    greater_pTable,             //15
    grater_or_equals_pTable,    //16
    in_pTable,                  //17
    left_round_bracket_pTable,  //18
    right_round_bracket_pTable, //19
    id_pTable,                  //20
    function_pTable,            //21
    array_pTable,               //22
    comma_pTable,               //23
    dollar_pTable,              //24
    no_operation_pTable         //25
} pTable;



/**
 * Inicializacia tokenoveho zasobnika
 * @param S Zasobnik
 */
void InitStackTOKEN (tStackP *S);

/**
 * Pridanie tokenu do zasobnika
 * @param S Zasobnik
 * @param ptr Token
 */
void PushStackTOKEN (tStackP *S, struct token ptr);

/**
 * Ziskanie tokenu a uvolnenie tokenu z vrchu zasobnika
 * @param S Zasobnik
 * @return Token zo zasobnika
 */
struct token PopStackTOKEN (tStackP *S);

/**
 * Zisti ci je tokenovy zasobnik prazdny
 * @param S Zasobnik
 * @return Ak je zasobnik prazdny vrati true inak false
 */
bool IsStackEmptyTOKEN (tStackP *S);



/**
 * Inicializacia integeroveho zasobnika
 * @param S Zasobnik
 */
void InitStackINT (tStackINT *S);

/**
 * Prida integerovu hodnotu do zasobnika
 * @param S integerovy zasobnik
 * @param ptr integer
 */
void PushStackINT (tStackINT *S, long ptr);


/**
 * Odstrani integer na vrchu zasobnika a vrati integerovu hodnotu zo zasobnika
 * @param S Integerovi zasobnik
 * @return integer
 */
long PopStackINT (tStackINT *S);


/**
 * Zistenie ci je integerovy zasobnik prazdny
 * @param S Zasobnik
 * @return Ak je zasobnik prazdny vrati true inak false
 */
bool IsStackEmptyINT (tStackINT *S);


/**
 * Inicializuje stringovi zasobnik
 * @param S String zasobnik
 */
void InitStackSTRING (tStackCHAR *S);

/**
 * Prida string do zasobnika
 * @param S Stringovy zasobnik
 * @param ptr String
 */
void PushStackSTRING (tStackCHAR *S, char* ptr);

/**
 * Odstrani string z vrchu zasobnika a vrati ho na vystup funkcie
 * @param S
 * @return
 */
char* PopStackSTRING (tStackCHAR *S);

/**
 * Zisti ci je zasobnik prazdny
 * @param S Zasobnik
 * @return True ak prazdny False ak nie je prazdny
 */
bool IsStackEmptySTRING (tStackINT *S);



/**
 * @param Token Vystup z scannera
 * @param L Buffer/Obojsmerny zoznam
 * @note Ulozi cely kontrolovany vyraz do bufferu
 */
void LoadToBuffer(Token *Token, tDLList *ExprList);

/**
 * Vykona operaciu pri vypocitavani postfixovej notacie
 * @param token_ID1 Prvy operand
 * @param token_OP Operacia
 * @param token_ID2 Druhy operand
 * @return Vrati vysledok danej operacie
 */
//long EvaluateNow(long token_ID1, Token token_OP, long token_ID2 );

/**
 * Vypocita vyraz v postfixovej notacii
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture - TODO momentalne nepotebne
 * @param stackPostfix Zasobnik s ulozenou postfixou notaciou
 * @param stackTemp Pomocny zasobnik pre pretocenie poradia v postfixovom zasobniku
 * @param stackOutputINT Vystupny zasobnik v ktorom je pocitana postfixova notacia - TODO nepouzivane
 * @param stackOutputCHAR Vystupny zasobnik v ktorom je pocitana postfixova notacia v type char!
 * @param last_operation Posledna operacia - momentalne nepotrebne
 * @return Vracia vysledok v datovom type long
 */
char* EvaluateFromPostfix(tDLList *ExprList, tStackP *stackPostfix, tStackP *stackCHAR, tStackINT *stackOutputINT , tStackCHAR *stackOutputCHAR, int last_operation);

/**
 * Spracuje infixovy vyraz do postfixovej notacie pomocou precedencnej tabulky
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stack Pomocny zasobnik kde sa vyhodnocuje priorita operatorov
 * @param stackOutput Vystupny zasobnik s postfixovou notaciou
 * @param last_operation Posledna vykonana operacia - TODO nepotrebne momentane
 * @return Vracia zasobnik s postfixou notaciou
 */
tStackP ParseToPostfix(tDLList *ExprList, tStackP *stack, tStackP *stackOutput, int last_operation);

/**
 * Vykonava syntakticku kontrolu vyrazu
 * @param ExprList Zoznam s vyrazom
 * @param SyntaxError Premenna pre status ci nastal/nenastal error
 * @return Vrati true ak je vyraz syntakticky spravne inak false
 */
bool FindRule(tDLList *ExprList, bool *SyntaxError);

/**
 * Zastresuje syntakticku kontrolu
 * @param ExprList
 * @return Vrati true ak je vyraz syntakticky spravne inak false
 * @note TODO Spojenie s Rules
 */
bool MainSyntaxCheck(tDLList *ExprList);

/**
 * Zastresuje vsetky funkcie vyrazoveho parseru
 * @param token Novy token zo scannera
 * @return Vrati true ak je vyraz syntakticky spravne inak false
 */
int CallExpressionParser(Token *token);

/**
 * Uvolnenie zoznamu v ktorom je nacitany vyraz
 * @param ExprList Zoznam s vyrazom
 */
void FreeBuffer(tDLList *ExprList);

/**
 *  Pretypuje string na integer
 * @param x String na pretypovanie
 * @return Identicka hodnota ako integer
 */
int string_to_integer(char* x);

/**
* Pretypuje integer na string
* @param x Integerova hodnota na pretypovanie
* @return Identicka hodnota v stringu
*/
char* integer_to_string(int x);


#endif //EXPRESSION_PARSER_EXPRESSION_PARSER_H
