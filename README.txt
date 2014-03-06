Chenxiao Ma
504079332
chenxiao@ucla.edu

Peipei Zhou
204176631
memoryzpp@gmail.com

Part B:
Chenxiao coded BTLeafNode class.
Peipei coded BTNonLeafNode class.

Each BTLeafNode can hold (1024-4)/12 = 85 (RecordId, key) pairs at most.
Each BTNonLeafNode can hold (1024-4)/8 = 172 (key, PageId) pairs at most.

For convenience, we decided to implement the Maximum number N of keys stored in a node to be 60 for both BTLeafNode and BTNonLeafNode. 

Part C:
We used 1(one) grace day.

Chenxiao coded BTreeIndex::open(), BTreeIndex::close(), BTreeIndex::insert()
Peipei coded BTreeIndex::locate(), BTreeIndex::readForward(), BTreeIndex::recInsert()

we slightly modified BTreeNode.cc and believed the change is less than 50%

Part D:
