%option noyywrap

%{
#include <cstdio>
#include <string>
#include "ASTNodes.h"
#include "parser.hpp"
#define TOKEN(t) ( yylval.token = t)
%}



%%

[ \t\n]+				{ /* empty */ }
"return"                { return TOKEN(T_RETURN); }
"int"                   { return TOKEN(T_INT); }
"void"                  { return TOKEN(T_VOID); }
"if"                    { return TOKEN(T_IF); }
"else"                  { return TOKEN(T_ELSE); }
"while"                 { return TOKEN(T_WHILE); }
"continue"              { return TOKEN(T_CONTINUE); }
"break"                 { return TOKEN(T_BREAK); }

"="						{ return TOKEN(T_ASIGN); }

"=="					{ return TOKEN(T_CEQ); }
"!="                    { return TOKEN(T_CNE); }
"<"                     { return TOKEN(T_CLT); }
"<="                    { return TOKEN(T_CLE); }
">"                     { return TOKEN(T_CGT); }
">="                    { return TOKEN(T_CGE); }
"+"                     { return TOKEN(T_PLUS); }
"*"                     { return TOKEN(T_MUL); }
"/"                     { return TOKEN(T_DIV); }
"%"                     { return TOKEN(T_MOD); }
"&&"                    { return TOKEN(T_AND); }
"||"                    { return TOKEN(T_OR); }
"&"                     { return TOKEN(T_BITAND); }
"|"                     { return TOKEN(T_BITOR); }
"^"                     { return TOKEN(T_BITXOR); }
">>"                    { return TOKEN(T_RSHIFT); }
"<<"                    { return TOKEN(T_LSHIFT); }

"-"                     { return TOKEN(T_NEG_OR_MINUS); }
"!"                     { return TOKEN(T_NOT); }
"~"                     { return TOKEN(T_BITNOT); }

"("                     { return TOKEN(T_LPAREN); }
")"                     { return TOKEN(T_RPAREN); }
"{"                     { return TOKEN(T_LBRACE); }
"}"                     { return TOKEN(T_RBRACE); }
";"                     { return TOKEN(T_SEMICOLON); }
","                     { return TOKEN(T_COMMA); }

[a-zA-Z_][a-zA-Z0-9_]*	{ yylval.str = new std::string(yytext); return T_IDENTIFIER; }
[0-9]+  				{ yylval.int_const = std::atoi(yytext); return T_INT_CONST; }

.						{ printf("Unknown token: %s\n", yytext); yyterminate(); }

%%