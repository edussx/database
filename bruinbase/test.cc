#include<iostream>
#include<cstring>
#include"BTreeNode.h"

using namespace std;

int main()
{
	BTLeafNode test1;
	int m_PageId = 0;
	int m_KeyCount = 1;
	memcpy(test1.buffer, (char*)(&m_PageId), 4);
	memcpy(test1.buffer+4, (char*)(&m_KeyCount), 4);	

	char* keycountaddress = test1.buffer + sizeof(PageId);
	int keycount = *((int*)keycountaddress); 
	cout<<keycount<<endl; 
}
