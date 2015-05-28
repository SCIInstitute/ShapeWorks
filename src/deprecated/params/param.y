/*
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: param.y,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
   
   This file defines a grammer for the parameter file parser. It serves as an
   input file for the Bison utility, which will generate a function yyparse().

   A successful call to yyparse() will generate a parse tree with a
   parameterFile object at its root.
   
*/
%{

extern "C" {
#include <stdio.h>
}
#include <iostream>
#include "param.h"
#define YYERROR_VERBOSE 1
  
using namespace param;

extern int  line_no, token_head, err_count;
extern int  yylex();
extern char *yytext;
extern FILE *yyin;
extern void yyrestart( FILE * );
File        OPF_file_parse_result;
int         syntax_error_count=0;
int         semantic_error_count=0;

typedef Value<float> DecimalValue;
typedef Value<long int> IntegerValue;
typedef Value<std::string> StringValue;
 
/**
 * Function required by the bison utility.
 */
void reset_yyvalues()
  {
	line_no=0;
	token_head=0;
	syntax_error_count=0;
	semantic_error_count=0;
  }

/**
 * Function required by the bison utility
 */
void yyerror (const char *errstring)
{
  syntax_error_count++;
  std::cerr << "yyparse(): Line " << line_no
            << ", " << "Syntax error before \""
            << yytext << "\"" << std::endl;
}
%}

%union
{
  long int integer;
  float decimal;
  std::string *string;
  Parameter*  Parameter_ptr;
  File*       File_ptr;
  GenericValue * GenericValue_ptr;
  std::vector<GenericValue *> * ValueArray_ptr;
};

%token <integer>NUM
%token <decimal>DEC_NUM
%token <string>IDENT STRING
%token EQUALS LPAREN RPAREN NEWLINE

%type <GenericValue_ptr> Value
%type <Parameter_ptr> Parameter ValueList
%type <File_ptr> File
%type <GenericValue_ptr> IntegerVal DecimalVal StringVal

%%
Start: File
{
  OPF_file_parse_result = *$1;
  delete $1;
}
;

File: Parameter
{
  $$ = new File;
  (*$$)[$1->Key()] = *$1;
  delete $1;
}
| Parameter File
{
  $$ = $2;
  (*$$)[$1->Key()] = *$1;
  delete $1;
}
;

Parameter: LPAREN IDENT RPAREN
{
  $$ = new Parameter();
  $$->Key(*$2);
}
| LPAREN IDENT ValueList RPAREN
{
  $$ = $3;
  $$->Key(*$2);
}
;

ValueList:  Value
{
  $$ = new Parameter();
  $$->PushFront(*$1);
  ValueFactory::Delete( $1 );
}
| Value ValueList
{
  $$ = $2;
  $$->PushFront(*$1);
  ValueFactory::Delete( $1) ;
}
;

Value:  IntegerVal
{
  $$ = dynamic_cast<GenericValue *>($1);
}
| DecimalVal
{
  $$ = dynamic_cast<GenericValue *>($1);
}
| StringVal
{
  $$ = dynamic_cast<GenericValue *>($1);
}
;

IntegerVal:  NUM
{
  $$ = new IntegerValue();
  dynamic_cast<IntegerValue *>($$)->SetValue(yylval.integer);
}
;

DecimalVal    :  DEC_NUM
{
  $$ = new DecimalValue();
  dynamic_cast<DecimalValue *>($$)->SetValue(yylval.decimal);
}
;

StringVal     :  STRING
{
  $$ = new StringValue();
  dynamic_cast<StringValue *>($$)->SetValue(*yylval.string);
  delete yylval.string;
}
;

%%
