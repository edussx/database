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
		PageId rootPid = 1; //pp: wrong here, rootPid is a data member
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

		return 0;
	}
	//tree is not empty
	else
	{
		//call recursive insert
		//RC rc;
		int m_Sibling_key = -1;
		int m_Sibling_pid = -1;
		bool m_flag = false;//default no overflow

		if(rc = recInsert(key, rid, rootPid, m_flag, m_Sibling_key, m_Sibling_pid, 1))//start from rootPid
			return rc;
		if(m_flag)//new root is needed, treeHeight ++
		{
			//new_root
			BTNonLeafNode new_root;
			//initialize new root with old rootPid and m_Sibling_pid
			new_root.initializeRoot(rootPid, m_Sibling_key, m_Sibling_pid);//
			//update the rootPid
			rootPid = pf.endPid();
			treeHeight ++;//update the tree height
			//finally write the new root back
			if(rc = new_root.write(rootPid, pf))
				return rc;

			return 0;

		}
		else//no need to get new root
		{
			return 0;
		}

	}

    //return rc;
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
	//note the root is height 1, then child of root is height 2
    int cur_height = 1;
    PageId cur_pid;//page id for nonleaf and leaf
    int eid;//entry id in the leaf node
    BTNonLeafNode nonleaf;
    //rc for error code info
    RC rc;
    if(rc = nonleaf.read(rootPid, pf))//if rc is not 0
    	return rc;//return the error code
    //now nonleaf contains the root
    if(rc = nonleaf.locateChildPtr(searchKey, cur_pid))
    	return rc;
    cur_height++;
    while(cur_height < treeHeight)//if it is not the leaf
    {
    	//now nonleaf contains the child of root
    	if(rc = nonleaf.read(cur_pid, pf))
    		return rc;
    	//locate the child ptr of child of root
    	if(rc = nonleaf.locateChildPtr(searchKey, cur_pid))
    		return rc;
    	cur_height++;
    }
    //when exit, it means we reach the leaf node, 
    //and cur_pid contains the leaf pid
    BTLeafNode leaf;
    if(rc = leaf.read(cur_pid, pf))
    	return rc;
    //now leaf contains the leaf node
    //if error code is not 0, it means RC_NO_SUCH_RECORD
    //no record has equal or larger key than the searchKey
    if(rc = leaf.locate(searchKey, eid))
    	return rc;
    //if error code is 0, it means there is a record
    //assign value to the cursor, pid and eid
    cursor.pid = cur_pid;
    cursor.eid = eid;
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
    	cursor.eid = 0;
    	cursor.pid = leaf.getNextNodePtr();
    }
    //else, cursor.pid no change, cursor.eid no change
    return 0;
}


//
/*
 * recursive insert
 * @param key[IN] the key to be inserted
 * @param rid[IN] the RecordId to be inserted
 * @param input_pid[IN] the starting point of pid
 * @param flag[OUT], the flag to indicates whether overflow happens
 * @param sibling_key[OUT], the key to be inserted when overflow happens in child
 * @param sibling_pid[OUT], the pid to be inserted when overflow happens in child
 * @param height[IN] the height 
 * @return error code. 0 if no error
 */
RC BTreeIndex::recInsert(int key, const RecordId& rid, PageId input_pid, bool& flag, int& Sibling_key, PageId& Sibling_pid, int height)
{
	RC rc;
	int m_Sibling_key = -1;
	int m_Sibling_pid = -1;
	bool m_flag = false;//default no overflow
	BTLeafNode leaf;
	BTLeafNode nonleaf;
	PageId child_pid;
	PageId new_pid;
	if(height < treeHeight)//in the nonleaf node level
	{
		if(rc = nonleaf.read(input_pid, pf))
			return rc;
		//now nonleaf contain the starting point
		if(rc = nonleaf.locateChildPtr(key, child_pid))
			return rc;
		//now cur_pid contains the child_pid
		// recursive call insert
		if(rc = recInsert(key, rid, child_pid, m_flag, m_Sibling_key, m_Sibling_pid, height + 1))
			return rc;
		//handle the result from recursive call insert
		//check the flag from the child
		if(m_flag)//if true, then there is overflow
		{
			//we have to use the m_Sibling_key and m_Sibling_pid to insert into non-leaf node
			if(rc = nonleaf.insert(m_Sibling_key, m_Sibling_pid))//if comes here, rc = RC_NODE_FULL, then use insertAndSplit
			{
				BTNonLeafNode Sibling_nonleaf;
				int mid_key;
				//nonleaf is full, use insertAndSplit version
				//and Sibling_nonleaf and nonleaf will redistribute and mid_key shall be return to upper level
				if(rc = nonleaf.insertAndSplit(m_Sibling_key, m_Sibling_pid, Sibling_nonleaf, mid_key))
					return rc;
				m_flag = true;//non leaf has overflow
				m_Sibling_key = mid_key;//set the m_Sibling_key for the upper level node to be inserted
				m_Sibling_pid =  pf.endPid();//get the new page for Sibling and for the upper level node to be inserted

				if(nonleaf.write(input_pid,pf))
					return rc;
				if(Sibling_nonleaf.write(m_Sibling_pid,pf))
					return rc;
				
				return 0;
			}
			else//if comes here, rc = 0, then insert is already down
			{
				//since just insert the sibling_key and Sibling_pid into the nonleaf is done, write back
				if(nonleaf.write(input_pid,pf))
					return rc;
				return 0;//every thing is done
			}

		}
		else//if false then there is no overflow
		{
			//no change to m_flag
			//m_flag is false
			return 0;
		}

	}
	else//in the leaf node level
	{
		if(rc = leaf.read(input_pid, pf))
			return rc;
		//now leaf node contains the leaf
		//try to insert into the key, rid to the leaf node
		if(rc = leaf.insert(key, rid))//if comes here, then rc = RC_NODE_FULL, then use insertAndSplit
		{
			BTLeafNode Sibling_leaf;
			if(rc = leaf.insertAndSplit(key, rid, Sibling_leaf, m_Sibling_key))//note m_Sibling_key is set here
				return rc;
			//now Sibling_leaf and leaf are both updated
			m_flag = true;//set the flag to true
			m_Sibling_pid = pf.endPid();//get the new pid for write
			//update the next node ptr
			Sibling_leaf.setNextNodePtr(leaf.getNextNodePtr());
			leaf.setNextNodePtr(m_Sibling_pid);

			//write back leaf and sibling_leaf
			if(rc = Sibling_leaf.write(m_Sibling_pid, pf))//write back Sibling_leaf
				return rc;
			if(rc = leaf.write(input_pid,pf))//write back leaf
				return rc;
			return 0;//return correct code

		}
		else//if comes here, then no overflow
		{
			//no overflow in the leaf node, write the leaf node back to page
			if(rc = leaf.write(input_pid, pf))
				return rc;
			//comes here, leaf node is updated and written back
			return 0;//return correct code
		}

	}
}
