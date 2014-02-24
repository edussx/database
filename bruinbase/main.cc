/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include <cstring>
#include <iostream>
using namespace std;

void setBuffer(char* buff, int value, int pos)
{
	char* position = buff + sizeof(int)*pos;
	memcpy(position, (char*) &value, 4);
}

void test()
{
	BTLeafNode test1;

	int m_PageId = 10;
	int m_KeyCount = 1;
	//memcpy(test1.buffer, (char*)(&m_PageId), 4);
	memcpy(test1.buffer+4, (char*)(&m_KeyCount), 4);	

	setBuffer(test1.buffer, 100 ,0);

	cout << "next ptr should be 100: " << test1.getNextNodePtr() << endl;
	cout << "key count should be 1: " << test1.getKeyCount() << endl;

	test1.setNextNodePtr(5);
	cout << "new next ptr should be 5: " << test1.getNextNodePtr() << endl;

}

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);

  test();

  return 0;
}
