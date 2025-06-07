%{
#include <cstdio>

#include "ASTNodes.h"
#include "global.h"

IdentifierTable Node::identifierTable; // 标识符表定义
std::stack<int> Node::labelStack; // 标签栈，用于continue和break
int Node::labelNo = 0;

std::shared_ptr<NBlock> programBlock;
extern int yylex();
extern int yylineno;
void yyerror(const char* s)
{
    printf("Error: %s at line %d\n", s, yylineno);
}
%}

%union
{
	int int_const;
	std::string* str;
	int token;

	NType* type;
	NOperator* op;
	
	NExpression* expression;
	NStatement* statement;
	NIdentifier* identifier;

	NVariableDeclaration* variable_declaration;
	NFunctionDefine* function_define;
	
	NBlock* block;

	std::vector<std::shared_ptr<NExpression>>* expression_list;
}

%token <str> T_IDENTIFIER
%token <int_const> T_INT_CONST
%token <token> T_NEG T_NOT T_BITNOT
%token <token> T_ASIGN
%token <token> T_CEQ T_CNE T_CLT T_CLE T_CGT T_CGE
%token <token> T_LPAREN T_RPAREN T_LBRACE T_RBRACE 
%token <token> T_COMMA T_SEMICOLON
%token <token> T_PLUS T_MINUS T_MUL T_DIV T_MOD 
%token <token> T_AND T_OR T_BITAND T_BITOR T_BITXOR
%token <token> T_LSHIFT T_RSHIFT
%token <token> T_RETURN T_INT T_VOID T_IF T_ELSE T_WHILE T_CONTINUE T_BREAK
%token <token> T_NEG_OR_MINUS

%left T_ASIGN
%left T_OR
%left T_AND
%left T_BITOR
%left T_BITXOR
%left T_BITAND
%left T_CEQ T_CNE
%left T_CGE T_CGT T_CLE T_CLT
%left T_LSHIFT T_RSHIFT
%left T_PLUS T_MINUS T_NEG_OR_MINUS
%left T_MUL T_DIV T_MOD
%right T_NOT T_BITNOT T_NEG

%type <block> block c_program statements
%type <statement> statement
%type <function_define> func_define

%type <type> typename

%type <expression> expression number cal_expression
%type <identifier> identifier
%type <variable_declaration> func_define_args var_decl var_decl_inner
%type <expression_list> call_args


%start c_program

%%
c_program : 	statements { programBlock = std::shared_ptr<NBlock>($1); }
				;

statements : 	statement { $$ = new NBlock(); $$->statements->push_back(std::shared_ptr<NStatement>($<statement>1)); }
				| statements statement { $1->statements->push_back(std::shared_ptr<NStatement>($<statement>2)); }
				;

statement : 	var_decl T_SEMICOLON { $$ = $1; }
				| func_define { $$ = $1; }
				| expression T_SEMICOLON { $$ = new NExpressionStatement(std::shared_ptr<NExpression>($1)); }
				| T_RETURN expression T_SEMICOLON { $$ = new NReturnStatement(std::shared_ptr<NExpression>($2)); }
				| T_IF T_LPAREN expression T_RPAREN block { $$ = new NIfStatement(std::shared_ptr<NExpression>($3), std::shared_ptr<NBlock>($5)); }
				| T_IF T_LPAREN expression T_RPAREN block T_ELSE block { $$ = new NIfStatement(std::shared_ptr<NExpression>($3), std::shared_ptr<NBlock>($5), std::shared_ptr<NBlock>($7)); }
				| T_WHILE T_LPAREN expression T_RPAREN block { $$ = new NWhileStatement(std::shared_ptr<NExpression>($3), std::shared_ptr<NBlock>($5)); }
				| T_CONTINUE T_SEMICOLON { $$ = new NContinueStatement(); }
				| T_BREAK T_SEMICOLON { $$ = new NBreakStatement(); }
				;

block : 		T_LBRACE statements T_RBRACE { $$ = $2; }
				| T_LBRACE T_RBRACE { $$ = new NBlock(); }
				;

typename : 		T_INT { $$ = new NType(CType::INT); }
				| T_VOID { $$ = new NType(CType::VOID); }
				;

var_decl :		typename var_decl_inner { $$ = $2; }
				;

var_decl_inner:	identifier
					{
						$$ = new NVariableDeclaration();
						$$->addItem(std::shared_ptr<NIdentifier>($1));
					}
				| identifier T_ASIGN expression
					{ 
						$$ = new NVariableDeclaration();
						$$->addItem(std::shared_ptr<NIdentifier>($1), nullptr, std::shared_ptr<NExpression>($3));
					}
				| var_decl_inner T_COMMA identifier
					{
						$1->addItem(std::shared_ptr<NIdentifier>($3));
					}
				| var_decl_inner T_COMMA identifier T_ASIGN expression
					{
						$1->addItem(std::shared_ptr<NIdentifier>($3), nullptr, std::shared_ptr<NExpression>($5));
					}
				;

func_define : 	typename identifier T_LPAREN func_define_args T_RPAREN block 
					{ $$ = new NFunctionDefine(std::shared_ptr<NType>($1), std::shared_ptr<NIdentifier>($2), std::shared_ptr<NVariableDeclaration>($4), std::shared_ptr<NBlock>($6)); }
				;

func_define_args : 	/* blank */ { $$ = new NVariableDeclaration(); }
					| typename identifier 
						{ 	
							$$ = new NVariableDeclaration();
							$$->addItem(std::shared_ptr<NIdentifier>($2), std::shared_ptr<NType>($1));
						}
					| func_define_args T_COMMA typename identifier 
						{ 
							$$->addItem(std::shared_ptr<NIdentifier>($4), std::shared_ptr<NType>($3));
						}
					;

call_args : 	/* blank */ { $$ = new ExpressionList(); }
				| expression { $$ = new ExpressionList(); $$->push_back(std::shared_ptr<NExpression>($1)); }
				| call_args T_COMMA expression { $1->push_back(std::shared_ptr<NExpression>($3)); }
				;

identifier : 	T_IDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
				;

number : 		T_INT_CONST { $$ = new NInteger($1); }
				;

expression : 	identifier T_ASIGN expression { $$ = new NAssignment(std::shared_ptr<NIdentifier>($1), std::shared_ptr<NExpression>($3)); }
				| identifier T_LPAREN call_args T_RPAREN { $$ = new NMethodCall(std::shared_ptr<NIdentifier>($1), std::shared_ptr<ExpressionList>($3)); }
				| identifier { $$ = $1; }
				| number { $$ = $1; }
				| T_LPAREN expression T_RPAREN { $$ = $2; }
				| cal_expression { $$ = $1; }
				;

cal_expression:   T_NEG_OR_MINUS 			expression %prec T_NEG		{ auto op = new NOperator(COperator::NEG); 		$$ = new NUnaryOperatorExpression(std::shared_ptr<NExpression>($2), std::shared_ptr<NOperator>(op)); } 
				| T_NOT 					expression 					{ auto op = new NOperator(COperator::NOT); 		$$ = new NUnaryOperatorExpression(std::shared_ptr<NExpression>($2), std::shared_ptr<NOperator>(op)); }
				| T_BITNOT 					expression					{ auto op = new NOperator(COperator::BITNOT); 	$$ = new NUnaryOperatorExpression(std::shared_ptr<NExpression>($2), std::shared_ptr<NOperator>(op)); }
			 	| expression T_CEQ 			expression 					{ auto op = new NOperator(COperator::CEQ);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_CNE 			expression 					{ auto op = new NOperator(COperator::CNE);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_CLT 			expression 					{ auto op = new NOperator(COperator::CLT);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_CLE 			expression 					{ auto op = new NOperator(COperator::CLE);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_CGT 			expression 					{ auto op = new NOperator(COperator::CGT);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_CGE			expression 					{ auto op = new NOperator(COperator::CGE);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_PLUS 		expression 					{ auto op = new NOperator(COperator::PLUS);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_NEG_OR_MINUS expression %prec T_MINUS	{ auto op = new NOperator(COperator::MINUS); 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_MUL 			expression 					{ auto op = new NOperator(COperator::MUL);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_DIV 			expression 					{ auto op = new NOperator(COperator::DIV);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_MOD			expression 					{ auto op = new NOperator(COperator::MOD);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_AND 			expression 					{ auto op = new NOperator(COperator::AND);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_OR			expression 					{ auto op = new NOperator(COperator::OR);	 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_BITAND 		expression 					{ auto op = new NOperator(COperator::BITAND);	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_BITOR 		expression 					{ auto op = new NOperator(COperator::BITOR); 	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_BITXOR 		expression 					{ auto op = new NOperator(COperator::BITXOR);	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_RSHIFT 		expression 					{ auto op = new NOperator(COperator::RSHIFT);	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				| expression T_LSHIFT		expression 					{ auto op = new NOperator(COperator::LSHIFT);	$$ = new NBinaryOperatorExpression(std::shared_ptr<NExpression>($1), std::shared_ptr<NOperator>(op), std::shared_ptr<NExpression>($3)); }
				;

%%