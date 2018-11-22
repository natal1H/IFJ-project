//--------------INCLUDE--------------
#include "expression_parser.h"
#include "scanner.h"
#include "stringlib.h"
# include <stdio.h>

/*
 * Vyuziva enum token_type zo skenera
 *
 *  Zakladna tabulka:
 *          |            |            |              Nasledujuci token(nad nimi su ID z enum token_type)            |
 *          | Podmienka/ |Moze skoncit|  7     9     8     10    17   18     2     3     4     11   12    13     14    15    16 |
 *          |  Pocitadlo |   vyraz?   |  +     -     *     /     (     )    INT  FLOAT STRING  <    <=     >     >=    ==    != | token   ID
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  +      7
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  -      9
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  *      8
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  /      10
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  (      17
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  )      18
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  1  |  1  |  1  |  1  |  0  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  INT    2
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------  ID
 *          |            |            |  1  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  FLOAT  3
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ?  |  ?  |  ?  |  ?  |  1  |  1  |  STRING 4
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?P |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <      11
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <=     12
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------+-----+------------
 *          |            |            |  ?  |  1P  |  0  |  0 |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >      13
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >=     14
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ==     15
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  !=     16
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *
 *
 *
 */

#define SUCCESSFUL_END 23
#define COUNTER_OF_TOKEN 22


long long syntax_table[22][24]= {
        //    0      1    2      3      4      5      6      7     8     9    10    11    12    13    14    15    16    17    18    19    20      21   22  23
        // NO_TYPE   ID   INT  FLOAT  STRING KWORD  ASSIGN   +     *     -     /     <    =<     >    >=    ==    !=     (     )   COMMA  EOL     EOF   P   S | Nazov tokenu         ID_cislo
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //NO_TYPE,             //0
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    1  , 0 , 1}, //IDENTIFIER,          //1
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1}, //INTEGER,             //2
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1}, //FLOAT,               //3
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1}, //STRING,              //4
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //KEYWORD,             //5
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //ASSIGN,              //6
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //ADDITION,            //7
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //MULTIPLICATION,      //8
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //SUBTRACTION,         //9
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //DIVISION,            //10
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //LESS,                //11
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //LESS_OR_EQUALS,      //12
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //GREATER,             //13
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //GREATER_OR_EQUALS,   //14
        {    0   ,  1  ,  1  ,  1   ,   1  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //EQUALS,              //15
        {    0   ,  1  ,  1  ,  1   ,   1  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //NOT_EQUALS,          //16
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //LEFT_ROUND_BRACKET,  //17
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1}, //RIGHT_ROUND_BRACKET, //18
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0 ,   0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0}, //COMMA,               //19
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    1  , 0 , 1}, //EOL,                 //20
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 1}, //TYPE_EOF,            //21
        //  ^   ^
        //  |   |
        //  |   +Moze skoncit vyraz?
        //  |
        // + Pocet(napr pocitanie zatvoriek)
};

const char precedenceTable[table_size][table_size]= {
        //    0      1    2     3     4     5     6     7     8    9     10    11     12   13    14    15    16    17    18    19    20      21       22    23    24
        //  unary-  not   *     /    div   mod   and    +     -    or    xor    =     <>    <    <=     >    >=    in     (    )     ID   function  array   ,      $
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '#' , '>' }, //unary  0
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // not   1
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // *     2
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // /     3
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // div   4
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // mod   5
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // and   6
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // +     7
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // -     8
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // or    9
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // xor   10
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // =     11
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <>    12
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <     13
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <=    14
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >     15
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >=    16
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // in    17
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // (     18
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // )     19
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // ID    20
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // func  21
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // array 22
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // ,     23
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '#' , '<' ,   '<'   ,  '<' , '#' , '#' }, // $     24
};

int TransformTable[22] = {
        no_operation_pTable,       //0
        id_pTable,                 //1
        id_pTable,                 //2
        id_pTable,                 //3
        no_operation_pTable,       //4
        no_operation_pTable,       //5
        no_operation_pTable,       //6
        addition_pTable,           //7
        multiplication_pTable,     //8
        substraction_pTable,       //9
        division_pTable,           //10
        less_pTable,               //11
        less_or_equals_pTable,     //12
        greater_pTable,            //13
        grater_or_equals_pTable,   //14
        equals_pTable,             //15
        not_equals_pTable,         //16
        left_round_bracket_pTable, //17
        right_round_bracket_pTable,//18
        comma_pTable,              //19
        dollar_pTable,             //20
        no_operation_pTable        //21
};


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
//long EvaluateNow(long token_ID1, Token token_OP, long token_ID2 ){
//
//    switch(token_OP.type){
//
//        case (ADDITION) : {
//            return token_ID1+token_ID2;
//        }
//
//        case (MULTIPLICATION) : {
//            return token_ID1*token_ID2;
//        }
//
//        default:{}
//    }
//}

/**
 * Vypocita vyraz v postfixovej notacii
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture - TODO momentalne nepotebne
 * @param stackPostfix Zasobnik s ulozenou postfixou notaciou
 * @param stackTemp Pomocny zasobnik pre pretocenie poradia v postfixovom zasobniku
 * @param stackOutputINT Vystupny zasobnik v ktorom je pocitana postfixova notacia
 * @param last_operation Posledna operacia - momentalne nepotrebne
 * @return Vracia vysledok v datovom type long
 */
//long EvaluateFromPostfix(tDLList *ExprList, tStackP *stackPostfix, tStackP *stackTemp, tStackINT *stackOutputINT , int last_operation) {
//    printf("tu");
//
//    //Obratenie zasobnika, najskor POPnut potrebujeme operandy ktore su na spodku zasobnika
//    while(stackPostfix->top > 0){
//        PushStack(stackTemp, PopStack(stackPostfix));
//    }
//    stackPostfix = stackTemp;
//
//    while(stackPostfix->top > 0){
//        Token tmp;                    //Pomocny token
//        tmp = PopStack(stackPostfix); //Zisti hodnotu na vrchole zasobni
//
//            if(TransformTable[tmp.type] == id_pTable) { //Ak ide o ID
//                char *ptr;  //Buffer k pretypovaniu
//                PushStackINT(stackOutputINT, strtol(tmp.attribute, &ptr, 10));
//            } else {
//                PushStackINT(stackOutputINT, EvaluateNow(PopStackINT(stackOutputINT), tmp, PopStackINT(stackOutputINT)));
//            }
//    }
//    return PopStackINT(stackOutputINT);
//}

/**
 * Spracuje infixovy vyraz do postfixovej notacie pomocou precedencnej tabulky
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stack Pomocny zasobnik kde sa vyhodnocuje priorita operatorov
 * @param stackOutput Vystupny zasobnik s postfixovou notaciou
 * @param last_operation Posledna vykonana operacia - TODO nepotrebne momentane
 * @return Vracia zasobnik s postfixou notaciou
 */
//tStackP ParseToPostfix(tDLList *ExprList, tStackP *stack, tStackP *stackOutput, int last_operation) {
//
//    bool UvolnovanieZasobnika = false;
//    Token tmp;              //Pomocny token
//    tmp.type = EOL;         //Koniec zasobnika ako EOL
//    PushStack(stack, tmp);  //Informuje o konci zasobnika
//
////    ExprList->Act = ExprList->First;
//
//    while (ExprList->Act != NULL && ExprList->Act->Token.type != EOL) { //Pokym nie je koniec vyrazu(zoznamu) TODO Token.Next == NULL
//
//                tmp = PopStack(stack); //Zisti hodnotu na vrchole zasobnika
//                PushStack(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku
//
//                if(TransformTable[ExprList->Act->Token.type] == id_pTable){ //Ak je to operand
//                    PushStack(stackOutput, ExprList->Act->Token);           //Potom push do vysledneho zasobnika
//                } else {
//
//                    //Pokym je vacsia priorita
//                    while (precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '>') {
//
//                        //Ak sa token na zasobniku nerovna '(' alebo ')'
//                        //Zatvorky nepushujeme do vysledneho vyrazu!
//                        if(TransformTable[tmp.type] != left_round_bracket_pTable && TransformTable[tmp.type] != right_round_bracket_pTable) {
//                            PushStack(stackOutput, PopStack(stack)); //Prehod token do vysledneho zasobnika
//                        } else {
//                            PopStack(stack);    //Len odober token zo zasobnika, len zatvorky!
//                        }
//
//                        tmp = PopStack(stack); //Zisti hodnotu na vrchole zasobnika
//                        PushStack(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku
//                        UvolnovanieZasobnika = true;
//
////                      if(precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '<') {
////                          PushStack(stack, ExprList->Act->Token);
////                      }
//                    }
//                    if(precedence_table[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '=' ){
//                        PopStack(stack);
////                    } else if (1) {
//                    } else {
//                        PushStack(stack, ExprList->Act->Token);
//                    }
//                }
//                UvolnovanieZasobnika = false;
//                ExprList->Act = ExprList->Act->next; //Dalsi Token
//    }
//
//
//    while(stack->top > 1){
////        tmp = PopStack(stack);
////        if (tmp.type != LEFT_ROUND_BRACKET && tmp.type != RIGHT_ROUND_BRACKET) {
//            PushStack(stackOutput, PopStack(stack));
////        }
//    }
////    for(int i = 1; i<=stackOutput->top; i++) {
////        print_token(&(stackOutput)->stack[i]);
////    }
//
//    return *stackOutput;
//}


bool FindRule(tDLList *ExprList, bool *SyntaxError, int *NumberOfLeftBracket, int *NumberOfRightBracket, int *NumberOfID, int *NumberOfOperator, bool FirstToken) {


// TODO kontrola syntaxe
        bool CouldBeTokenTheLastOne = true;
        do{

//   while ( (token->type) != EOL || (token->type) != KEYWORD && (strcmp(token->attribute, "do") != 0 || strcmp(token->attribute, "then") != 0)) {
            //syntax_table[actual token][next token]
            if(ExprList->Act->next == NULL || syntax_table[ExprList->Act->Token.type][ExprList->Act->next->Token.type] == false){
                *SyntaxError = true;
            }

            //Kontrola zatvoriek
            if(ExprList->Act->Token.type == LEFT_ROUND_BRACKET){
                   syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;

            } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET &&
                     (syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] >= syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ) ){
                         syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;

                  } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET){
                         *SyntaxError = true;
                  } else {
                         syntax_table[ExprList->Act->Token.type][COUNTER_OF_TOKEN] += 1;
                  }


            //Vyhodnotenie pre posledny token
            if( syntax_table[ExprList->Act->Token.type][SUCCESSFUL_END]){
                     CouldBeTokenTheLastOne = true;
            } else { CouldBeTokenTheLastOne = false; }

            ExprList->Act = ExprList->Act->next;
        } while (!((ExprList->Act == NULL) || ExprList->Act->Token.type == EOL || ExprList->Act->Token.type == EOF ));

        if( syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] != syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ||
            CouldBeTokenTheLastOne == false ||

            //Ak sa miesaju stringy a int/float
            ((syntax_table[STRING][COUNTER_OF_TOKEN] != 0)
             && (syntax_table[INTEGER][COUNTER_OF_TOKEN] +
              syntax_table[FLOAT][COUNTER_OF_TOKEN]) != 0) ||

            //Ak je vo vyraze string a pocet operandov je parny
            //TODO Doplnit podmienku ze identifier je string!
            ((syntax_table[STRING][COUNTER_OF_TOKEN] > 0)
             && ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN]) == 0)
             &&  (syntax_table[STRING][COUNTER_OF_TOKEN]%2) == 0) ||

            //Maximalne jeden porovnavaci token
            (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) != 0) &&

              (syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN] != 1)) ||

            //Pocet operacii je neparny
            //Sucet operatov musi byt parny ak neexistuje porovnavanie
            !(

                     //Ak nie je pritomne porovnanie
                    (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 0)

                            && //musi byt

                                      //Potom je pocet operacii neparny
                                     ((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +
                                     syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1)&&
                                     ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 0))

                     || //alebo

                    //Ak je pritomne porovnanie
                   ((((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                   syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                   syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 1))

                            && //musi byt

                                   //Sucet operacii je neparny a sucet operandov je neparny
                                   ((((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                 syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1) &&
                                   ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 1))

                                   || //alebo

                                    //Sucet operacii je parny a sucet operandov je parny
                                   (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                  syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 0) &&
                                   ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 0))))
            )


        ){
            *SyntaxError = true;
        }

////    fprintf(stderr,"Syntakticka chyba, nenajdene pravidlo");
//    *SyntaxError = true;
    return *SyntaxError;
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
            bool SyntaxError = false;
            bool *SyntaxErroPtr = &SyntaxError;

            bool FirstToken = true;
            bool *FirstTokenPtr = &FirstToken;

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

            //Kontroluje syntakticke pravidla
            return FindRule(ExprList, SyntaxErroPtr, NumberOfLeftBracketsPtr, NumberOfRightBracketsPtr, NumberOfIDPtr, NumberOfOperatorPtr, FirstTokenPtr);
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



        //TODO semanticka priprava
        //Skontroluje ci sa nachadza vo vyraze iba jeden symbol
        //Symbol moze byt == != > >= < <=
        //Jeho miesto sa ulozi do strukturu -> rozdelenie do dvoch vyrazov

        // symbol = 0 -> Ziadny symbol
        // symbol = 1 -> Najdene '=='
        // symbol = 2 -> Najdene '!='
        // symbol = 3 -> Najdene '<'
        // symbol = 4 -> Najdene '<='
        // symbol = 5 -> Najdene '>'
        // symbol = 6 -> Najdene '>='
//        int symbol = 0;
        //Pri spravnom vyraze musi byt vzdy maximalne 1!
//        int symbolCounter = 0;

//        tmp->SyntaxChecker = tmp->First;
//        while ( !((tmp->SyntaxChecker == NULL) || tmp->SyntaxChecker->Token.type == EOL))
//        {
//            switch (tmp->SyntaxChecker->Token.type){
//                case(EQUALS) : {
////                    symbol = 1;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                case(NOT_EQUALS) : {
////                    symbol = 2;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                case(LESS) : {
////                    symbol = 3;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                case(LESS_OR_EQUALS) : {
////                    symbol = 4;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                case(GREATER) : {
////                    symbol = 5;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                case(GREATER_OR_EQUALS) : {
////                    symbol = 6;
//                    symbolCounter++;
//                    tmp->Symbol = tmp->SyntaxChecker;
//                    break;
//                }
//                default: {
//                }
//            }//End switch
//            tmp->SyntaxChecker = tmp->SyntaxChecker->next;
//        }

//        if(symbolCounter > 1){
//            fprintf(stderr, "Error");
//            return false; //syntaxStatus
//        }



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
//        *stack = ParseToPostfix(ExprList, stack, stackOutput, last_operation);
        //Re-Inicializacia(Vycistenie) zasobnika
        InitStack(stackOutput);

        //Vysledok daneho vyrazu TODO Zmenit
//        long vysledok = 0;
//        vysledok = EvaluateFromPostfix(ExprList, stack, stackOutput, stackOutputINT , last_operation);
//        printf("%ld", vysledok);


//        *stack = UseLanguageRules(ExprList, stack, last_operation);
        return syntaxStatus;
    }
}





//bool CallExpressionParser(Token *token) {
//
//    printf("EXPRS PARSER TEST\n");
////    freopen("input.txt","r",stdin);
//
////    // inicializuj scanner najprv cez scanner_initialize()
////    if ( scanner_initialize() != 0 ) {
////        // chyba pri inicializácii
////        printf("Chyba pri inicializácii scannera");
////        return -1; // TODO return actual error code
////    }
//
//
////    Token *token =  token_initialize();
////    expressionPtr Expr = InitExpression();
//    tDLList *ExprArray = malloc(sizeof(tDLList));
//
//    int ret = 0;
//    //TODO ak vrati error scanner potom vratit ten error
////    while ( ( ret = get_next_token((token = token_initialize()))) != EOF) {
//
//     while ( (token->type) != EOF) {
//
////                print_token(token);
//
//
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

bool CallExpressionParser(Token *token) {

    printf("EXPRS PARSER TEST\n");
//    freopen("input.txt","r",stdin);

//    // inicializuj scanner najprv cez scanner_initialize()
//    if ( scanner_initialize() != 0 ) {
//        // chyba pri inicializácii
//        printf("Chyba pri inicializácii scannera");
//        return -1; // TODO return actual error code
//    }


//    Token *token =  token_initialize();
//    expressionPtr Expr = InitExpression();
    tDLList *ExprArray = malloc(sizeof(tDLList));

    int ret = 0;
    //TODO ak vrati error scanner potom vratit ten error
//    while ( ( ret = get_next_token((token = token_initialize()))) != EOF) {

    while ( (token->type) != EOL || (token->type) != KEYWORD && (strcmp(token->attribute, "do") != 0 || strcmp(token->attribute, "then") != 0)) {

//                print_token(token);


        LoadToBuffer(token, ExprArray);
        //        token_free(token);
    }
    bool MainSyntaxStatus = false;
    MainSyntaxStatus = MainSyntaxCheck(ExprArray);

    if(MainSyntaxStatus == 1){
//        printf ("\nMainSyntaxStatus %d \n", ERR_SYNTAX);
        return ERR_SYNTAX;
    } else{
//        printf ("\nMainSyntaxStatus %d \n", ERR_OK);
        return ERR_OK;
    }
    
    //        printf("RET: %d\n", ret);
//        print_token(token);
//        if (token->type == INTEGER) {
//            printf("integer\n");
//        }
//        token_free(token);
//        first_negative_number = false; //Po vypisani vzdy nasleduje druhy znak, navzdy false
//    }

//    token_free(token);

    // po skončení práce uvoľni miesto po read_string
//    tstring_free_struct(read_string);
}


////Parameter vysledneho bude get_next_token
//int main(int argc, char *argv[]) {
//
////    printf("EXPRS PARSER TEST\n");
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
//
//    MainSyntaxStatus = MainSyntaxCheck(ExprArray);
//
//    if(MainSyntaxStatus == 1){
////        printf ("\nMainSyntaxStatus %d \n", ERR_SYNTAX);
//        return ERR_SYNTAX;
//    } else{
////        printf ("\nMainSyntaxStatus %d \n", ERR_OK);
//        return ERR_OK;
//    }
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
//}
