/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SELECT = 258,
     FROM = 259,
     WHERE = 260,
     LOAD = 261,
     WITH = 262,
     INDEX = 263,
     QUIT = 264,
     COUNT = 265,
     AND = 266,
     OR = 267,
     COMMA = 268,
     STAR = 269,
     LF = 270,
     INTEGER = 271,
     STRING = 272,
     ID = 273,
     EQUAL = 274,
     NEQUAL = 275,
     LESS = 276,
     LESSEQUAL = 277,
     GREATER = 278,
     GREATEREQUAL = 279
   };
#endif
/* Tokens.  */
#define SELECT 258
#define FROM 259
#define WHERE 260
#define LOAD 261
#define WITH 262
#define INDEX 263
#define QUIT 264
#define COUNT 265
#define AND 266
#define OR 267
#define COMMA 268
#define STAR 269
#define LF 270
#define INTEGER 271
#define STRING 272
#define ID 273
#define EQUAL 274
#define NEQUAL 275
#define LESS 276
#define LESSEQUAL 277
#define GREATER 278
#define GREATEREQUAL 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 32 "SqlParser.y"
{
  int integer;
  char* string;
  SelCond* cond;
  std::vector<SelCond>* conds;
}
/* Line 1529 of yacc.c.  */
#line 104 "SqlParser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE sqllval;

