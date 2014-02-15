/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#ifndef SQLENGINE_H
#define SQLENGINE_H

#include <vector>
#include "Bruinbase.h"
#include "RecordFile.h"

/**
 * data structure to represent a condition in the WHERE clause
 */
struct SelCond {
  int attr;     // attribute: 1 - key column,  2 - value column
  enum Comparator { EQ, NE, LT, GT, LE, GE } comp;
  char* value;  // the value to compare
};

/**
 * the class that takes, parses, and executes the user commands.
 */
class SqlEngine {
 public:
    
  /**
   * takes the user commands from commandline and executes them.
   * when user issues SELECT or LOAD from commandline, this function
   * calls SqlEngine::select() or SqlEngine::load() functions.
   * @param commandline[IN] the input stream to get user commands
   * @return error code. 0 if no error
   */
  static RC run(FILE* commandline);

  /**
   * executes a SELECT statement.
   * all conditions in conds must be ANDed together.
   * the result of the SELECT is printed on screen.
   * @param attr[IN] attribute in the SELECT clause
   * (1: key, 2: value, 3: *, 4: count(*))
   * @param table[IN] the table name in the FROM clause
   * @param conds[IN] list of conditions in the WHERE clause
   * @return error code. 0 if no error
   */
  static RC select(int attr, const std::string& table, const std::vector<SelCond>& conds);

  /**
   * load a table from a load file.
   * @param table[IN] the table name in the LOAD command
   * @param loadfile[IN] the file name of the load file
   * @param index[IN] true if "WITH INDEX" option was specified
   * @return error code. 0 if no error
   */
  static RC load(const std::string& table, const std::string& loadfile, bool index);

  /**
   * parse a line from the load file into the (key, value) pair.
   * @param line[IN] a line from a load file
   * @param key[OUT] the key field of the tuple in the line
   * @param value[OUT] the value field of the tuple in the line
   * @return error code. 0 if no error
   */
  static RC parseLoadLine(const std::string& line, int& key, std::string& value);
};

#endif /* SQLENGINE_H */
