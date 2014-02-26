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

	// cout << "next ptr should be 100: " << test1.getNextNodePtr() << endl;
	// cout << "key count should be 1: " << test1.getKeyCount() << endl;

	// test1.setNextNodePtr(5);
	// cout << "new next ptr should be 5: " << test1.getNextNodePtr() << endl;


}

void test2()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 3, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	//cout << test1.getKeyCount() << endl;

	setBuffer(test1.buffer, 5, 5);
	setBuffer(test1.buffer, 6, 6);
	setBuffer(test1.buffer, 7, 7);

	setBuffer(test1.buffer, 8, 8);
	setBuffer(test1.buffer, 9, 9);
	setBuffer(test1.buffer, 10, 10);

	RecordId id;
	int key;

	test1.readEntry(0, key, id);

	cout << "key is: " << key << endl;
	cout << "pid is: "<< id.pid << " sid is: " << id.sid << endl;

	int eid;
	test1.locate(7, eid);

	cout << "it should be 2: " << eid << endl;
}

void test3()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	// setBuffer(test1.buffer, 5, 5);
	// setBuffer(test1.buffer, 6, 6);
	// setBuffer(test1.buffer, 7, 7);

	// setBuffer(test1.buffer, 8, 8);
	// setBuffer(test1.buffer, 9, 9);
	// setBuffer(test1.buffer, 10, 10);

	RecordId testid;
	testid.pid = 6;
	testid.sid = 7;

	test1.insert(8, testid);
	//setBuffer(test1.buffer, 2, 1);
	test1.setKeyCount(2);
	RecordId id;
	int key;

	test1.readEntry(1, key, id);
	cout << "key should be 8: " << key << endl;
	cout << "pid should be 6: " << id.pid << " sid should be 7: " << id.sid << endl;

}

void test4()
{
	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	int eid;
	test1.locate(5, eid);

	cout << "it should be 1: " << eid << endl;
}

void test5()
{
	RecordId id;
	int key;

	BTLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 1, 1);

	setBuffer(test1.buffer, 2, 2);
	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	RecordId testid1;
	testid1.pid = 9;
	testid1.sid = 10;
	test1.insert(11, testid1);
	test1.readEntry(1, key, id);
	cout << "key should be 11: " << key << endl;
	cout << "pid should be 9: " << id.pid << " sid should be 10: " << id.sid << endl;


	RecordId testid2;
	testid2.pid = 6;
	testid2.sid = 7;
	test1.insert(8, testid2);
	test1.readEntry(1, key, id);
	cout << "key should be 8: " << key << endl;
	cout << "pid should be 6: " << id.pid << " sid should be 7: " << id.sid << endl;

	RecordId testid3;
	testid3.pid = 5;
	testid3.sid = 5;
	test1.insert(5, testid3);
	test1.readEntry(1, key, id);
	cout << "key should be 5: " << key << endl;
	cout << "pid should be 5: " << id.pid << " sid should be 5: " << id.sid << endl;
}

void test6()
{
	BTLeafNode test1(999);

	setBuffer(test1.buffer, 1000, 0);
	setBuffer(test1.buffer, 4, 1);

	setBuffer(test1.buffer, 0, 2);
	setBuffer(test1.buffer, 0, 3);
	setBuffer(test1.buffer, 4, 4);

	setBuffer(test1.buffer, 0, 5);
	setBuffer(test1.buffer, 0, 6);
	setBuffer(test1.buffer, 7, 7);

	setBuffer(test1.buffer, 0, 8);
	setBuffer(test1.buffer, 0, 9);
	setBuffer(test1.buffer, 10, 10);

	setBuffer(test1.buffer, 0, 11);
	setBuffer(test1.buffer, 0, 12);
	setBuffer(test1.buffer, 13, 13);

	//test1.printNode();

	BTLeafNode test2(1000);
	RecordId testid1;
	testid1.pid = 0;
	testid1.sid = 0;
	int firstKey;
	test1.insertAndSplit(8, testid1, test2, firstKey);
	test1.printNode();
	cout << endl;
	test2.printNode();
	cout << firstKey << endl;
}

//BTNonLeafNode test begins
void test7()
{
	/*
	pid:990
	count pid key  pid key  pid  key  pid
	 --------------------------------------
	| 3 | 998| 10 | 999| 20 |1000| 30 |1001|
	 --------------------------------------
	  0	  1    2   3   4    5   6    7
	*/
	BTNonLeafNode test1(990);

	setBuffer(test1.buffer, 3, 0);

	setBuffer(test1.buffer, 998, 1);
	setBuffer(test1.buffer, 10, 2);

	setBuffer(test1.buffer, 999, 3);
	setBuffer(test1.buffer, 20, 4);

	setBuffer(test1.buffer, 1000, 5);
	setBuffer(test1.buffer, 30, 6);

	setBuffer(test1.buffer, 1001, 7);

	cout << "keycount should be 3: " << test1.getKeyCount() << endl;
	PageId tmp_pid;
	int searchKey = 25;
	test1.locateChildPtr(25, tmp_pid);
	cout << "pid should be 1000: " << tmp_pid << endl;
	test1.locateChildPtr(35, tmp_pid);
	cout << "pid should be 1001: " << tmp_pid << endl;
	test1.setKeyCount(2);
	cout << "new keycount should be 2: " << test1.getKeyCount() << endl;
	test1.setKeyCount(3);

	BTNonLeafNode test2(12000);
	test2.initializeRoot(990, 40, 1100);
	/*
	pid: 12000
	count pid key  pid key  pid  key  pid
	 ------------------
	| 3 | 990| 40 |1100| 
	 ------------------
	  0	  1    2   3   
	*/
	test2.printNode();


}


int main()
{
  // run the SQL engine taking user commands from standard input (console).
  //SqlEngine::run(stdin);

  	//test();
  	//test2();
  	//test3();
	//test4();
	//test5();
	//test6();
	test7();
  return 0;
}
