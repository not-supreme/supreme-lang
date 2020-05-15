#ifndef SUPREMELANG_PRECEDENCE_H
#define SUPREMELANG_PRECEDENCE_H

typedef enum _e_precedence
{
    PRECEDENCE_NONE,
    PRECEDENCE_ASSIGNMENT,  //  =        
    PRECEDENCE_OR,          //  ||       
    PRECEDENCE_AND,         //  &&
    PRECEDENCE_EQUALITY,    //  == !=
    PRECEDENCE_COMPARISON,  //  < > <= >=
    PRECEDENCE_TERM,        //  + -
    PRECEDENCE_FACTOR,      //  * /
    PRECEDENCE_UNARY,       //  ! -
    PRECEDENCE_CALL,        //  . ()
    PRECEDENCE_PRIMARY
} e_precedence;

#endif // SUPREMELANG_PRECEDENCE_H
