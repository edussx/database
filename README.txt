Chenxiao Ma
504079332
chenxiao@ucla.edu

Peipei Zhou
204176631
memoryzpp@gmail.com


Chenxiao coded BTLeafNode class.
Peipei coded BTNonLeafNode class.

Each BTLeafNode can hold (1024-4)/12 = 85 (RecordId, key) pairs at most.
Each BTNonLeafNode can hold (1024-4)/8 = 172 (key, PageId) pairs at most.

For convenience, we decided to implement the Maximum number N of keys stored in a node to be 60 for both BTLeafNode and BTNonLeafNode. 