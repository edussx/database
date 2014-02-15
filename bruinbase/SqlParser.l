%{
#include <cstring>
#include "SqlEngine.h"
#include "SqlParser.tab.h"

char* strlower(char* s)
{
	char* i = s;
	char  c;
	while ((c = *i)) {
		*i++ = tolower(c);
        }
	return s;
}
%}

%%

SELECT|select   return SELECT;
FROM|from       return FROM;
WHERE|where     return WHERE;
LOAD|load       return LOAD;
WITH|with	return WITH;
INDEX|index	return INDEX;
QUIT|quit	return QUIT;
EXIT|exit	return QUIT;
COUNT\(\*\)|count\(\*\) return COUNT;

AND|and         return AND;
OR|or           return OR;
"="		return EQUAL;
"<>"		return NEQUAL;
">"		return GREATER;
"<"		return LESS;
">="		return GREATEREQUAL;
"<="  		return LESSEQUAL;

\-?[0-9]+                   sqllval.string = strdup(sqltext); return INTEGER;
'[^']*'                  sqllval.string = strdup(sqltext+1); sqllval.string[sqlleng-2] = 0; return STRING;
[A-Za-z][A-Za-z0-9\-_]*  sqllval.string = strlower(strdup(sqltext)); return ID;
,                        return COMMA;
\*                       return STAR;
\r?\n			 return LF;
\;			/* ignore semicolon */
[ \t]+			/* ignore white space */

%%
