/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include "Bruinbase.h"
#include "RecordFile.h"

using std::string;

//
// helper functions for page manipultation
//

// compute the pointer to the n'th slot in a page
static char* slotPtr(char* page, int n);

// read the record in the n'th slot in the page
static void readSlot(const char* page, int n, int& key, std::string& value);

// write the record to the n'th slot in the page
static void writeSlot(char* page, int n, int key, const std::string& value);

// get # records stored in the page
static int getRecordCount(const char* page);

// update # records stored in the page
static void setRecordCount(char* page, int count);


//
// helper functions for RecordId manipulation
//

// postfix record id iterator
RecordId operator++ (RecordId& rid, int)
{
  RecordId prid(rid);

  // if the end of a page is reached, move to the next page
  if (++rid.sid >= RecordFile::RECORDS_PER_PAGE) {
    rid.pid++;
    rid.sid = 0;
  }

  return prid;
}

// prefix record id iterator
RecordId& operator++ (RecordId& rid)
{
  // if the end of a page is reached, move to the next page
  if (++rid.sid >= RecordFile::RECORDS_PER_PAGE) {
    rid.pid++;
    rid.sid = 0;
  } 

  return rid;
}

// RecordId comparators
bool operator < (const RecordId& r1, const RecordId& r2)
{
  if (r1.pid < r2.pid) return true;
  if (r1.pid > r2.pid) return false;
  return (r1.sid < r2.sid);
}

bool operator > (const RecordId& r1, const RecordId& r2)
{
  if (r1.pid > r2.pid) return true;
  if (r1.pid < r2.pid) return false;
  return (r1.sid > r2.sid);
}

bool operator <= (const RecordId& r1, const RecordId& r2)
{
  if (r1.pid < r2.pid) return true;
  if (r1.pid > r2.pid) return false;
  return (r1.sid <= r2.sid);
}

bool operator >= (const RecordId& r1, const RecordId& r2)
{
  if (r1.pid > r2.pid) return true;
  if (r1.pid < r2.pid) return false;
  return (r1.sid >= r2.sid);
}

bool operator == (const RecordId& r1, const RecordId& r2)
{
  return ((r1.pid == r2.pid) && (r1.sid == r2.sid));
}

bool operator != (const RecordId& r1, const RecordId& r2)
{
  return ((r1.pid != r2.pid) || (r1.sid != r2.sid));
}


RecordFile::RecordFile()
{
  erid.pid = 0;
  erid.sid = 0;
}

RecordFile::RecordFile(const string& filename, char mode)
{
  open(filename, mode);
}

RC RecordFile::open(const string& filename, char mode)
{
  RC   rc;
  char page[PageFile::PAGE_SIZE];

  // open the page file
  if ((rc = pf.open(filename, mode)) < 0) return rc;
  
  //
  // in the rest of this function, we set the end record id
  //

  // get the end pid of the file
  erid.pid = pf.endPid();

  // if the end pid is zero, the file is empty.
  // set the end record id to (0, 0).
  if (erid.pid == 0) {
    erid.sid = 0;
    return 0;
  }

  // obtain # records in the last page to set sid of the end record id.
  // read the last page of the file and get # records in the page.
  // remeber that the id of the last page is endPid()-1 not endPid().
  if ((rc = pf.read(--erid.pid, page)) < 0) {
    // an error occurred during page read
    erid.pid = erid.sid = 0;
    pf.close();
    return rc;
  }

  // get # records in the last page
  erid.sid = getRecordCount(page);
  if (erid.sid >= RECORDS_PER_PAGE) {
    // the last page is full. advance the end record id to the next page.
    erid.pid++;
    erid.sid = 0;
  }
  
  return 0;
}

RC RecordFile::close()
{
  erid.pid = 0;
  erid.sid = 0;

  return pf.close();
}

RC RecordFile::read(const RecordId& rid, int& key, string& value) const
{
  RC   rc;
  char page[PageFile::PAGE_SIZE];
  
  // check whether the rid is in the valid range
  if (rid.pid < 0 || rid.pid > erid.pid) return RC_INVALID_RID;
  if (rid.sid < 0 || rid.sid >= RecordFile::RECORDS_PER_PAGE) return RC_INVALID_RID;
  if (rid >= erid) return RC_INVALID_RID;
  
  // read the page containing the record
  if ((rc = pf.read(rid.pid, page)) < 0) return rc;

  // read the record from the slot in the page
  readSlot(page, rid.sid, key, value);

  return 0;
}

RC RecordFile::append(int key, const std::string& value, RecordId& rid)
{
  RC   rc;
  char page[PageFile::PAGE_SIZE];

  // unless we are writing to the the first slot of an empty page,
  // we have to read the page first
  if (erid.sid > 0) {
    if ((rc = pf.read(erid.pid, page)) < 0) return rc;
  } else {
    // if this is the first slot of an empty page
    // we can simply initialize the page with zeros
    memset(page, 0, PageFile::PAGE_SIZE);
  }
    
  // write the record to the first empty slot 
  writeSlot(page, erid.sid, key, value);

  // the first four bytes in the page stores # records in the page.
  // update this number.
  setRecordCount(page, erid.sid + 1);

  // write the page to the disk
  if ((rc = pf.write(erid.pid, page)) < 0) return rc;
    
  // we need to output the rid of the record slot
  rid = erid;

  // advance the end record id by one to the next empty slot
  ++erid;

  return 0;
}

const RecordId& RecordFile::endRid() const
{
  return erid;
}

static int getRecordCount(const char* page)
{
  int count;

  // the first four bytes of a page contains # records in the page
  memcpy(&count, page, sizeof(int));
  return count;
}

static void setRecordCount(char* page, int count)
{
  // the first four bytes of a page contains # records in the page
  memcpy(page, &count, sizeof(int));
}

static char* slotPtr(char* page, int n) 
{
  // compute the location of the n'th slot in a page.
  // remember that the first four bytes in a page is used to store
  // # records in the page and each slot consists of an integer and
  // a string of length MAX_VALUE_LENGTH
  return (page+sizeof(int)) + (sizeof(int)+RecordFile::MAX_VALUE_LENGTH)*n;
}

static void readSlot(const char* page, int n, int& key, std::string& value)
{
  // compute the location of the record
  char *ptr = slotPtr(const_cast<char*>(page), n);

  // read the key 
  memcpy(&key, ptr, sizeof(int));

  // read the value
  value.assign(ptr + sizeof(int));
}

static void writeSlot(char* page, int n, int key, const std::string& value)
{
  // compute the location of the record
  char *ptr = slotPtr(page, n);

  // store the key
  memcpy(ptr, &key, sizeof(int));

  // store the value. 
  if ((int)value.size() >= RecordFile::MAX_VALUE_LENGTH) {
    // when the string is longer than MAX_VALUE_LENGTH, truncate it.
    memcpy(ptr + sizeof(int), value.c_str(), RecordFile::MAX_VALUE_LENGTH -1);
    *(ptr + sizeof(int) + RecordFile::MAX_VALUE_LENGTH - 1) = 0;
  } else {
    strcpy(ptr + sizeof(int), value.c_str());
  }
}
