/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_SOURCE_LIB_PARSER_SRC_PARSER_BISON_HPP_INCLUDED
# define YY_YY_SOURCE_LIB_PARSER_SRC_PARSER_BISON_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 46 "source/lib/Parser/parser.y"

    #include <cstdint>
    #include <string>
    #include <cstring>
    #include "parser.hpp"


    struct TimeArr
    {
        TimeIso8601S* array_p;
        size_t last_id;
    };

    struct FloatArr
    {
        float* array_p;
        size_t last_id;
    };


#line 69 "source/lib/Parser/src/parser_bison.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LEX_STRING = 258,
    LEX_DATE_ISO = 259,
    LEX_NUM = 260,
    LEX_L_BRACKET = 261,
    LEX_R_BRACKET = 262,
    LEX_L_CBRACKET = 263,
    LEX_R_CBRACKET = 264,
    LEX_COMMA = 265,
    LEX_SEMICOLON = 266,
    LEX_COLON = 267,
    LEX_ERROR = 268,
    LEX_HOURLY = 269,
    LEX_DAILY = 270,
    LEX_CURRENT = 271
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 68 "source/lib/Parser/parser.y"

    float number;
    std::string* str_p;
    TimeIso8601S* time_p;
    TimeArr* time_arr;
    FloatArr* float_arr;


#line 106 "source/lib/Parser/src/parser_bison.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SOURCE_LIB_PARSER_SRC_PARSER_BISON_HPP_INCLUDED  */
