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
//#include "BTreeIndex.h"
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
	test1.insertAndSplit(14, testid1, test2, firstKey);
	
	test1.printNode();
	cout << endl;
	test2.printNode();
	cout << "firstkey is: " << firstKey << endl;
}

//BTNonLeafNode test begins
void test7()
{
	/*
	pid:990
			0		   1 	 	  2
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

	int eid;
	test1.locate(25, eid);
	cout << "eid should be 2: " << eid << endl;

	int key;
	eid = 1;
	test1.readEntry(eid, tmp_pid, key);
	cout << "pid should be 999: " << tmp_pid << "key should be 20: " << key << endl;

	/*=======================================*/

	BTNonLeafNode test2(12000);
	test2.initializeRoot(990, 40, 1100);
	/*
	pid: 12000
	count pid key  pid key  pid  key  pid
	 ------------------
	| 1 | 990| 40 |1100| 
	 ------------------
	  0	  1    2   3   
	*/
	test2.printNode();


}

void test8()
{
	PageId id;
	int key;

	BTNonLeafNode test1;
	setBuffer(test1.buffer, 2, 0);

	setBuffer(test1.buffer, 1, 1);
	setBuffer(test1.buffer, 2, 2);

	setBuffer(test1.buffer, 3, 3);
	setBuffer(test1.buffer, 4, 4);

	setBuffer(test1.buffer, 5, 5);

	// test1.insert(11, 998);
	// //test1.printNode();
	// test1.readEntry(2, id, key);
	// cout << "key should be 11: " << key << endl;
	// cout << "pid should be 5: " << id << endl;


	test1.insert(1, 997);
	test1.printNode();
	test1.readEntry(0, id, key);
	cout << "key should be 1: " << key << endl;
	cout << "pid should be 1: " << id << endl;
	
	cout << test1.insert(999,999) << endl;
	//test1.printNode();
}

void test9()
{
	PageId id;
	int key;

	BTNonLeafNode test1;
	setBuffer(test1.buffer, 0, 0);
	setBuffer(test1.buffer, 0, 1);
	
	test1.insert(10,10);
	test1.insert(20,20);
	test1.insert(30,30);
	test1.insert(40,40);

	cout << endl;

	BTNonLeafNode test2;
	setBuffer(test2.buffer, 0, 0);
	setBuffer(test2.buffer, 0, 1);

	int midkey;
	test1.insertAndSplit(35, 35, test2, midkey);
	test1.printNode();
	cout << "midkey: " << midkey << endl;
	test2.printNode();
	cout << endl;
}

void test10()
{
	BTreeIndex myIndex;
	myIndex.open("testIndex.idx", 'w');
	//myIndex.open("testIndex.idx", 'r');

	RecordId testid;
	testid.pid = 9;
	testid.sid = 9;

	int key = 20;
	int rc;
	int myKey = 0;

	rc = myIndex.insert(key, testid);
	

	IndexCursor myCursor;
	myCursor.pid = -1;
	myCursor.eid = -1;
	rc = myIndex.locate(20, myCursor);
	cout << myCursor.pid << " " << myCursor.eid << endl;
	//cout << rc << endl;
	myIndex.readForward(myCursor, myKey, testid);
	
	cout << "Key should be 20: " << myKey << endl;
	cout << myCursor.pid << " " << myCursor.eid << endl;
	myIndex.close();
}

void test11()
{
	BTreeIndex myIndex;
	myIndex.open("testIndex.idx", 'w');
	//myIndex.open("testIndex.idx", 'r');

	RecordId testid;
	testid.pid = 9;
	testid.sid = 9;

	int key = 20;
	int rc;
	int myKey = 0;

	rc = myIndex.insert(20, testid);
	//testid.pid = 8;
	//testid.sid = 8;
	rc = myIndex.insert(21, testid);
	rc = myIndex.insert(18, testid);
	rc = myIndex.insert(22, testid);
	rc = myIndex.insert(23, testid);

	rc = myIndex.insert(24, testid);
	rc = myIndex.insert(25, testid);
	rc = myIndex.insert(26, testid);
	
	rc = myIndex.insert(27, testid);
	rc = myIndex.insert(28, testid);
	
	rc = myIndex.insert(29, testid);
	rc = myIndex.insert(30, testid);
	IndexCursor myCursor;
	myCursor.pid = -1;
	myCursor.eid = -1;
	myIndex.locate(24, myCursor);
	cout << myCursor.pid << " " << myCursor.eid << endl;
	myIndex.readForward(myCursor, myKey, testid);
	
	cout << "Key should be 19: " << myKey << endl;
	cout << "pid should be 9: " << testid.pid << " eid should be 9: " << testid.sid << endl;

	myIndex.close();
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
	//test7();
	//test8();
	//test9();
	//test10();
	//test11();
  return 0;
}
