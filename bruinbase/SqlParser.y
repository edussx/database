%{
#include <cstdio>
#include <cstring>
#include <sys/times.h>
#include <climits>
#include <string>
#include "Bruinbase.h"
#include "SqlEngine.h" 
#include "PageFile.h"

int  sqllex(void);  
void sqlerror(const char *str) { fprintf(stderr, "Error: %s\n", str); }
extern "C" { int  sqlwrap() { return 1; } }

static void runSelect(int attr, const char* table, const std::vector<SelCond>& conds)
{
  struct tms tmsbuf;
  clock_t btime, etime;
  int     bpagecnt, epagecnt;

  btime = times(&tmsbuf);
  bpagecnt = PageFile::getPageReadCount();
  SqlEngine::select(attr, table, conds);
  etime = times(&tmsbuf);
  epagecnt = PageFile::getPageReadCount();

  fprintf(stderr, "  -- %.3f seconds to run the select command. Read %d pages\n", ((float)(etime - btime))/sysconf(_SC_CLK_TCK), epagecnt - bpagecnt);
}

%}

%union {
  int integer;
  char* string;
  SelCond* cond;
  std::vector<SelCond>* conds;
}

%token SELECT FROM WHERE LOAD WITH INDEX QUIT COUNT AND OR 
%token COMMA STAR LF
%token <string> INTEGER STRING ID
%token EQUAL NEQUAL LESS LESSEQUAL GREATER GREATEREQUAL 

%type <integer> attributes attribute comparator
%type <string> table value
%type <cond> condition
%type <conds> conditions
%%

commands:
	commands command
	|
	;

command:
        load_command { fprintf(stdout, "Bruinbase> "); }
	| select_command { fprintf(stdout, "Bruinbase> "); }
	| quit_command
	| error LF { fprintf(stdout, "Bruinbase> "); }
	| LF { fprintf(stdout, "Bruinbase> "); }
	;

quit_command:
	QUIT { return 0; }
	;

load_command:
	LOAD table FROM STRING LF { 
	  SqlEngine::load(std::string($2), std::string($4), false); 
	  free($2);
	  free($4);
	}
	| LOAD table FROM STRING WITH INDEX LF { 
	  SqlEngine::load(std::string($2), std::string($4), true); 
	  free($2);
	  free($4);
	}
	;

select_command:
	SELECT attributes FROM table LF {
   	        std::vector<SelCond> conds;
		runSelect($2, $4, conds);
		free($4);
	}
	| SELECT attributes FROM table WHERE conditions LF {
	        runSelect($2, $4, *$6);
	  	free($4);
	  	for (unsigned i = 0; i < $6->size(); i++) {
		    free((*$6)[i].value);
		}
	  	delete $6;
	}
	;

conditions:
	condition {
	  std::vector<SelCond>* v = new std::vector<SelCond>;
	  v->push_back(*$1);
	  $$ = v;
          delete $1;
	}
	| conditions AND condition {
	  $1->push_back(*$3);
	  $$ = $1;
          delete $3;
	}
	;

condition:
	attribute comparator value { 
	  SelCond* c = new SelCond;
	  c->attr = $1;
	  c->comp = static_cast<SelCond::Comparator>($2);
	  c->value = $3;
	  $$ = c;
        }
	;

attributes:
	attribute { $$ = $1; }
	| STAR  { $$ = 3; }
	| COUNT { $$ = 4; }
	;

attribute:
	ID { 
		if (strcasecmp($1, "key") == 0) $$=1;
		else if (strcasecmp($1, "value") == 0) $$=2;
		else sqlerror("wrong attribute name. neither key or value");
		free($1);
	}

value:
	INTEGER  { $$ = $1; }
        | STRING { $$ = $1; }
	;

table:
	ID { $$ = $1; }
	;

comparator:
	EQUAL          { $$ = SelCond::EQ; }
	| NEQUAL       { $$ = SelCond::NE; }
	| LESS         { $$ = SelCond::LT; }
	| GREATER      { $$ = SelCond::GT; }
	| LESSEQUAL    { $$ = SelCond::LE; }
	| GREATEREQUAL { $$ = SelCond::GE; }
	;
