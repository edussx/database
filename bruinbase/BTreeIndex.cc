/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
	RC rc;
	char buffer[PAGE_SIZE];

	rc = pf.open(indexname, mode);
	if (rc == 0 && mode = 'r')
	{
		if (pf.endPid() == 0)
		{
			rootPid = -1;
			treeHeight = 0;
		}
		else
		{
			/*
			buffer:
			 -------------------
			|4 bytes|4 bytes|...|
			 -------------------
			rootPid treeHeight
			*/
			rc = pf.read(0, buffer);
			if (rc == 0)
			{
				memcpy(&rootPid, buffer, sizeof(PageId));
				memcpy(&treeHeight, buffer + sizeof(PageId), sizeof(int));
			}
		}
	}

    return rc;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	RC rc;
	char buffer[PAGE_SIZE];

	//write rootPid and treeHeight back to disk
	memcpy(buffer, &rootPid, sizeof(PageId));
	memcpy(buffer + sizeof(PageId), &treeHeight, sizeof(int));

	rc = pf.write(0, buffer);
	if (rc != 0) return rc;
	rc = pf.close();

    return rc;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	RC rc;
	BTNonLeafNode root;
	BTLeafNode leftleaf, rightleaf;
	//Initialize a new tree
	if (rootPid == -1)
	{
		//pid 0 is reserved for rootPid and treeHeight
		PageId rootPid = 1; 
		PageId lpid = 2;
		PageId rpid = 3;

		//write leftnode, which is empty at initialization
		rc = leftleaf.setNextNodePtr(rpid);
		if (rc != 0) return rc;
		rc = leftleaf.write(lpid, pf);
		if (rc != 0) return rc;
		//write rightnode
		rc = rightleaf.insert(key, rid);
		if (rc != 0) return rc;
		rc = rightleaf.write(rpid, pf);
		if (rc != 0) return rc;
		//init root
		rc = root.initializeRoot(lpid, key, rpid);
		if (rc != 0) return rc;
		//write root
		rc = root.write(rootPid, pf);
		if (rc != 0) return rc;
		
		treeHeight = 2;
	}
	//tree is not empty
	else
	{
		
	}

    return rc;
}

/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    
    BTLeafNode leaf;
    //get the cursor pid and eid
    PageId cur_pid = cursor.pid;
    int cur_eid = cursor.eid;
    //rc for this function
    RC rc;
    //read the indicated pid
    if(rc = leaf.read(cur_pid,pf)) //if error code is not 0
    	return rc;//return the error code
    //read the indicated eid
    if(rc = leaf.readEntry(cur_eid, key, rid))//if error code is not 0
    	return rc;//return the error code
    cursor.eid++;//move forward to the next entry
    if(cursor.eid == leaf.getKeyCount())
    //if exceed the valid number of entry
    {
    	cursor.pid = leaf.getNextNodePtr();
    	cursor.eid = 0;
    }
    //else, cursor.pid no change, cursor.eid no change
    return 0;
}
