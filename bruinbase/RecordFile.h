/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#ifndef RECORDFILE_H
#define RECORDFILE_H

#include <string>
#include "PageFile.h"

/**
 * The data structure for pointing to a particular record in a RecordFile.
 * A record id consists of pid (PageId) and sid (the slot number in the page)
 */
typedef struct {
  PageId  pid;  // page number. the first page is 0
  int     sid;  // slot number. the first slot is 0
} RecordId;

//
// helper functions for RecordId
// 

// RecordId iterators
RecordId& operator++ (RecordId& rid);
RecordId  operator++ (RecordId& rid, int);

// RecordId comparators
bool operator> (const RecordId& r1, const RecordId& r2);
bool operator< (const RecordId& r1, const RecordId& r2);
bool operator>= (const RecordId& r1, const RecordId& r2);
bool operator<= (const RecordId& r1, const RecordId& r2);
bool operator== (const RecordId& r1, const RecordId& r2);
bool operator!= (const RecordId& r1, const RecordId& r2);

/**
 * read/write a record to a file
 */
class RecordFile {
 public:

  // maximum length of the value field
  static const int MAX_VALUE_LENGTH = 100;  

  // number of record slots per page
  static const int RECORDS_PER_PAGE = (PageFile::PAGE_SIZE - sizeof(int))/ (sizeof(int) + MAX_VALUE_LENGTH);  
    // Note that we subtract sizeof(int) from PAGE_SIZE because the first
    // four bytes in the page is used to store # records in the page.

  RecordFile();
  RecordFile(const std::string& filename, char mode);
  
  /**
   * open a file in read or write mode.
   * when opened in 'w' mode, if the file does not exist, it is created.
   * @param filename[IN] the name of the file to open
   * @param mode[IN] 'r' for read, 'w' for write
   * @return error code. 0 if no error
   */
  RC open(const std::string& filename, char mode);

  /**
   * close the file.
   * @return error code. 0 if no error
   */
  RC close();

  /**
   * read a record from the file. note that every record is a (key, value) pair.
   * @param rid[IN] the id of the record to read
   * @param key[OUT] the record key
   * @param value[OUT] the record valu
   * @return error code. 0 if no error
   */
  RC read(const RecordId& rid, int& key, std::string& value) const;

  /**
   * append a new record at the end of the file.
   * note that RecordFile does not have write() function.
   * append is the only way to write a record to a RecordFile.
   * @param key[IN] the record key
   * @param value[IN] the record value
   * @param rid[OUT] the location of the stored record
   * @return error code. 0 if no error
   */
  RC append(int key, const std::string& value, RecordId& rid);

  /**
   * note the +1 part. The rid of the last record is endRid()-1.
   * @return (last record id + 1) of the RecordFile
   */
  const RecordId& endRid() const;

 private:
  PageFile pf;     // the PageFile used to store the records
  RecordId erid;   // the last record id of the file + 1
};

#endif // RECORDFILE_H
