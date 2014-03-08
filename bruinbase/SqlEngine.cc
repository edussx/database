/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  BTreeIndex treeindex;

  vector<SelCond> value_cond;
  vector<SelCond> key_cond;

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;
  bool ifIndex = false;

  for (unsigned i = 0; i < cond.size(); i++)
  {
    if (cond[i].attr == 1)
      key_cond.push_back(cond[i]);
    if (cond[i].attr == 2)
      value_cond.push_back(cond[i]);
  }



  if ((rc = treeindex.open(table + ".idx", 'r')) == 0) ifIndex = true;
  if (cond.size() == 0 ) ifIndex = false;


  //use non-index-based select
  if (!ifIndex)
  {
    // open the table file
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
      fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
      return rc;
    }

    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) {
      // read the tuple
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }

      // check the conditions on the tuple
      for (unsigned i = 0; i < cond.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (cond[i].attr) {
        case 1:
  	diff = key - atoi(cond[i].value);
  	break;
        case 2:
  	diff = strcmp(value.c_str(), cond[i].value);
  	break;
        }

        // skip the tuple if any condition is not met
        switch (cond[i].comp) {
        case SelCond::EQ:
  	if (diff != 0) goto next_tuple;
  	break;
        case SelCond::NE:
  	if (diff == 0) goto next_tuple;
  	break;
        case SelCond::GT:
  	if (diff <= 0) goto next_tuple;
  	break;
        case SelCond::LT:
  	if (diff >= 0) goto next_tuple;
  	break;
        case SelCond::GE:
  	if (diff < 0) goto next_tuple;
  	break;
        case SelCond::LE:
  	if (diff > 0) goto next_tuple;
  	break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
      case 1:  // SELECT key
        fprintf(stdout, "%d\n", key);
        break;
      case 2:  // SELECT value
        fprintf(stdout, "%s\n", value.c_str());
        break;
      case 3:  // SELECT *
        fprintf(stdout, "%d '%s'\n", key, value.c_str());
        break;
      }

      // move to the next tuple
      next_tuple:
      ++rid;
    }

    // print matching tuple count if "select count(*)"
    if (attr == 4) {
      fprintf(stdout, "%d\n", count);
    }
    rc = 0;
  }

  //use index-based select
  else
  {

  }


  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  //file --> parseLoadLine --> load
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  ifstream infile(loadfile.c_str());  //Read file; Must use c_str() since it's C++03
  BTreeIndex treeindex;

  RC     rc;
  int    key;     
  string line, value;

  if (index == true)
  {
    rc = treeindex.open(table + ".idx", 'w');
    //cout << "mark 0" << endl;
    if (rc != 0) {treeindex.close(); return rc;}  
  }

  //Open the table file
  //Open a file in read or write mode. (r/w)
  //When opened in 'w' mode, if the file does not exist, it is created.
  rc = rf.open(table + ".tbl", 'w');
  if (rc !=0) return rc;

    //Get each line from loadfile
  while (getline(infile, line))
  {
    rid = rf.endRid();
    //Get key and value by using parseLoadLine()
    //Get the last record id. Note rid is an instant of class RecordId
    parseLoadLine(line, key, value);
    rc = rf.append(key, value, rid);
    //cout << "mark 1" << endl;
    if (rc != 0) return rc;
    if (index == true)
    {
      //rc = treeindex.open(table + ".idx", 'w');
      //if (rc != 0) return rc;  
      rc = treeindex.insert(key, rid);
      //cout << "mark 2" << endl;
      if (rc != 0) return rc;
      //treeindex.close();
    }
  }

  //Close the table file
  rf.close();
  //Close the index file
  if (index == true) treeindex.close();
  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
