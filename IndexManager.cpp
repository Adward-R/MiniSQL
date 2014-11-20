#include "IndexManager.h"

int BPlusTree::getPtr(int pos){
    int ptr = 0;
    for(int i = pos; i<pos + POINTERLENGTH ; i++){
        ptr = 10 * ptr + buf.bufferBlock[bufferNum].values[i] - '0';
    }
    return ptr;
}
int BPlusTree::getRecordNum(){
    int recordNum = 0;
    for(int i = 2; i<6; i++){
        if(buf.bufferBlock[bufferNum].values[i] == EMPTY) break;
        recordNum = 10 * recordNum + buf.bufferBlock[bufferNum].values[i] - '0';
    }
    return recordNum;
}

Branch::Branch(int vbufNum, const Index& indexinfor){
    bufferNum = vbufNum;
    isRoot = ( buf.bufferBlock[bufferNum].values[0] == 'R' );
    int recordCount = getRecordNum();
    recordNum = 0;//recordNum will increase when function insert is called, and finally as large as recordCount
    ptrFather = getPtr(6);
    columnLength = indexinfor.columnLength;
    int position = 6 + POINTERLENGTH;
    for(int i = 0; i < recordCount; i++){
        string key = "";
        for(int i = position; i < position + columnLength; i++){
            if(buf.bufferBlock[bufferNum].values[i] == EMPTY) break;
            else key += buf.bufferBlock[bufferNum].values[i];
        }
        position += columnLength;
        int ptrChild = getPtr(position);
        position += POINTERLENGTH;
        IndexBranch node(key, ptrChild);
        insert(node);	
    }
}
Branch::~Branch(){
    //isRoot
    if(isRoot) buf.bufferBlock[bufferNum].values[0] = 'R';
    else buf.bufferBlock[bufferNum].values[0] = '_';
    //is not a Leaf
    buf.bufferBlock[bufferNum].values[1] = '_';
    //recordNum
    char tmpt[5];
    //itoa(recordNum, tmpt, 10);
    sprintf(tmpt,"%d",recordNum);
    string strRecordNum = tmpt;
    while(strRecordNum.length() < 4)
    strRecordNum = '0' + strRecordNum;
    strncpy(buf.bufferBlock[bufferNum].values + 2, strRecordNum.c_str() , 4);
		
    //nodelist
    if(nodelist.size() == 0){
        cout << "Undefined action!" << endl;
        exit(0);
    }

    list<IndexBranch>::iterator i;
    int position = 6 + POINTERLENGTH;	//ǰ��ļ�λ����洢index�������Ϣ��
    for(i = nodelist.begin(); i!=nodelist.end(); i++)
    {
        string key = (*i).key;
        while(key.length() <columnLength)
        key += EMPTY;
        strncpy(buf.bufferBlock[bufferNum].values + position, key.c_str(), columnLength);
        position += columnLength;
			
        char tmpt[5];
        sprintf(tmpt,"%d",(*i).ptrChild);
        //itoa((*i).ptrChild, tmpt, 10);
        string ptrChild = tmpt;
        while(ptrChild.length() < POINTERLENGTH)
        ptrChild = '0' + ptrChild;
        strncpy(buf.bufferBlock[bufferNum].values + position, ptrChild.c_str(), POINTERLENGTH);
        position += POINTERLENGTH;
    }
}
void Branch::insert(IndexBranch node){
    recordNum++;
    list<IndexBranch>::iterator i = nodelist.begin();
    if(nodelist.size() == 0)
    nodelist.insert(i, node);
    else{
        for(i = nodelist.begin(); i!=nodelist.end(); i++)
        if((*i).key > node.key) break;
        nodelist.insert(i, node);
    }
}
	
IndexBranch Branch::pop(){
    recordNum--;
    IndexBranch tmpt = nodelist.back();
    nodelist.pop_back();
    return tmpt;
}
	
IndexBranch Branch::getfront(){
    return nodelist.front();
}

Leaf::Leaf(int vbufNum){	//this kind of leaf is added when old leaf is needed to be splited
    bufferNum = vbufNum;
    recordNum = 0;
    nextSibling = lastSibling = 0;
}
Leaf::Leaf(int vbufNum, const Index& indexinfor){
    bufferNum = vbufNum;
    isRoot = ( buf.bufferBlock[bufferNum].values[0] == 'R' );
    int recordCount = getRecordNum();
    recordNum = 0;
    ptrFather = getPtr(6);
    lastSibling = getPtr(6 + POINTERLENGTH);
    nextSibling = getPtr(6 + POINTERLENGTH*2);
    columnLength = indexinfor.columnLength;	//��������Ժ������Ҫ�õ���

    //cout << "recordCount = "<< recordCount << endl;
    int position = 6 + POINTERLENGTH*3;	//ǰ��ļ�λ����洢index�������Ϣ��
    for(int i = 0; i < recordCount; i++)
    {	
        string key = "";
        for(int i = position; i < position + columnLength; i++){
            if(buf.bufferBlock[bufferNum].values[i] == EMPTY) break;
            else key += buf.bufferBlock[bufferNum].values[i];
        }
        position += columnLength;
        //cout << "get offsetInFile" << endl;
        int offsetInFile = getPtr(position);
        //cout << "get offsetInBlock" << endl;
        position += POINTERLENGTH;
        int offsetInBlock  = getPtr(position);
        position += POINTERLENGTH;
        IndexLeaf node(key, offsetInFile, offsetInBlock);
        insert(node);
    }
}
Leaf::~Leaf(){
    //isRoot
    if(isRoot) buf.bufferBlock[bufferNum].values[0] = 'R';
    else buf.bufferBlock[bufferNum].values[0] = '_';
    //isLeaf
    buf.bufferBlock[bufferNum].values[1] = 'L';
    //recordNum
    char tmpt[5];
    sprintf(tmpt,"%d",recordNum);
    //itoa(recordNum, tmpt, 10);
    string strRecordNum = tmpt;
    while(strRecordNum.length() < 4)
    strRecordNum = '0' + strRecordNum;
    int position = 2;
    strncpy(buf.bufferBlock[bufferNum].values + position, strRecordNum.c_str() , 4);
    position += 4;
    sprintf(tmpt,"%d",ptrFather);
    //	itoa(ptrFather, tmpt, 10);
    string strptrFather = tmpt;
    while(strptrFather.length() < POINTERLENGTH)
    strptrFather = '0' + strptrFather;
    strncpy(buf.bufferBlock[bufferNum].values + position, strptrFather.c_str() , POINTERLENGTH);
    position += POINTERLENGTH;

    sprintf(tmpt,"%d",lastSibling);
    //itoa(lastSibling, tmpt, 10);
    string strLastSibling = tmpt;
    while(strLastSibling.length() < POINTERLENGTH)
    strLastSibling = '0' + strLastSibling;
    strncpy(buf.bufferBlock[bufferNum].values + position, strLastSibling.c_str() , POINTERLENGTH);
    position += POINTERLENGTH;

    sprintf(tmpt,"%d",nextSibling);
    //itoa(nextSibling, tmpt, 10);
    string strNextSibling = tmpt;
    while(strNextSibling.length() < POINTERLENGTH)
    strNextSibling = '0' + strNextSibling;
    strncpy(buf.bufferBlock[bufferNum].values + position, strNextSibling.c_str() , POINTERLENGTH);
    position += POINTERLENGTH;
		
    //nodelist
    if(nodelist.size() == 0){
        cout << "Undefined action!" << endl;
        exit(0);
    }

    list<IndexLeaf>::iterator i;
    for(i = nodelist.begin(); i!=nodelist.end(); i++){
        string key = (*i).key;
        while(key.length() <columnLength)
        key += EMPTY;
        strncpy(buf.bufferBlock[bufferNum].values + position, key.c_str(), columnLength);
        position += columnLength;

        sprintf(tmpt,"%d",(*i).offsetInBlock);
        //itoa((*i).offsetInFile, tmpt, 10);
        string offsetInFile = tmpt;
        while(offsetInFile.length() < POINTERLENGTH)
        offsetInFile = '0' + offsetInFile;
        strncpy(buf.bufferBlock[bufferNum].values + position, offsetInFile.c_str(), POINTERLENGTH);
        position += POINTERLENGTH;
            
        sprintf(tmpt,"%d",(*i).offsetInBlock);
        //itoa((*i).offsetInBlock, tmpt, 10);
        string offsetInBlock = tmpt;
        while(offsetInBlock.length() < POINTERLENGTH)
        offsetInBlock = '0' + offsetInBlock;
        strncpy(buf.bufferBlock[bufferNum].values + position, offsetInBlock.c_str(), POINTERLENGTH);
        position += POINTERLENGTH;
        //cout << key<< "\t" <<offsetInFile<<"\t"<< offsetInFile<< endl;
    }
}

void Leaf::insert(IndexLeaf node){
    recordNum++;
    //cout << "onece" << endl;
    list<IndexLeaf>::iterator i = nodelist.begin();
    if(nodelist.size() == 0){
        nodelist.insert(i, node);
        return;
    }
    else{
        for(i = nodelist.begin(); i!=nodelist.end(); i++)
        if((*i).key > node.key) break;
    }
    nodelist.insert(i, node);
}
IndexLeaf Leaf::pop(){
    recordNum--;
    IndexLeaf tmpt = nodelist.back();
    nodelist.pop_back();
    return tmpt;
}
IndexLeaf Leaf::getfront(){//this is the smallest of all the keys of the list
    return nodelist.front();
}


void IndexManager::createIndex(const Table& tableinfor, Index& indexinfor){
    //create a new file
    string filename = indexinfor.index_name + ".index";
    fstream  fout(filename.c_str(), ios::out);
    fout.close();
    //create a root for the tree
    int blockNum = buf.getEmptyBuffer();
    buf.bufferBlock[blockNum].filename = filename;
    buf.bufferBlock[blockNum].blockOffset = 0;
    buf.bufferBlock[blockNum].isWritten = 1;
    buf.bufferBlock[blockNum].isValid = 1;
    buf.bufferBlock[blockNum].values[0] = 'R';//Root Node Block
    buf.bufferBlock[blockNum].values[1] = 'L';//Leaf Node Block
    memset( buf.bufferBlock[blockNum].values + 2, '0' , 4);
    for (int i = 0; i < 3; i++){
        memset( buf.bufferBlock[blockNum].values+6 + POINTERLENGTH*i, '0' , POINTERLENGTH);
        indexinfor.blockNum++;
    }
    //retrieve datas of the table and form a B+ Tree
    filename = tableinfor.name + ".table";
    string stringrow;
    string key;

    int length = tableinfor.totalLength + 1;
    const int recordNum = BLOCKSIZE / length;

    //read datas from the record and sort it into a B+ Tree and store it
    for(int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++){
        int bufferNum = buf.getIfIsInBuffer(filename, blockOffset);
        if(bufferNum == -1){
            bufferNum = buf.getEmptyBuffer();
            buf.readBlock(filename, blockOffset, bufferNum);
        }
        for(int offset = 0; offset < recordNum; offset ++){
            int position  = offset * length;
            stringrow = buf.bufferBlock[bufferNum].getvalues(position, position + length);
            if(stringrow.c_str()[0] == EMPTY) continue;//inticate that this row of record have been deleted
            stringrow.erase(stringrow.begin());
            key = getColumnValue(tableinfor, indexinfor, stringrow);
            IndexLeaf node(key, blockOffset, offset);
            insertValue(indexinfor, node);
        }
    }
}

IndexBranch IndexManager::insertValue(Index& indexinfor, IndexLeaf node, int blockOffset){
    IndexBranch reBranch;//for return, intial to be empty
    string filename = indexinfor.index_name + ".index";
    int bufferNum = buf.getbufferNum(filename, blockOffset);
    bool isLeaf = ( buf.bufferBlock[bufferNum].values[1] == 'L' );// L for leaf
    if(isLeaf){
        Leaf leaf(bufferNum, indexinfor);
        leaf.insert(node);
        
        const int RecordLength = indexinfor.columnLength + POINTERLENGTH*2;
        const int MaxrecordNum = (BLOCKSIZE-6-POINTERLENGTH*3)/RecordLength;
        if( leaf.recordNum > MaxrecordNum ){//record number is too great, need to split
            if(leaf.isRoot){//this leaf is a root
                int rbufferNum = leaf.bufferNum;	// buffer number for new root
                leaf.bufferNum = buf.addBlockInFile(indexinfor);	//find a new place for old leaf
                int sbufferNum = buf.addBlockInFile(indexinfor);	// buffer number for sibling
                Branch branchRoot(rbufferNum);	//new root, which is branch indeed
                Leaf leafadd(sbufferNum);	//sibling

                //is root
                branchRoot.isRoot = 1;
                leafadd.isRoot = 0;
                leaf.isRoot = 0;

                branchRoot.ptrFather = leafadd.ptrFather = leaf.ptrFather = 0;
                branchRoot.columnLength = leafadd.columnLength = leaf.columnLength;
                //link the newly added leaf block in the link list of leaf
                leafadd.lastSibling = buf.bufferBlock[leaf.bufferNum].blockOffset;
                leaf.nextSibling = buf.bufferBlock[leafadd.bufferNum].blockOffset;
                while(leafadd.nodelist.size() < leaf.nodelist.size()){
                    IndexLeaf tnode = leaf.pop();
                    leafadd.insert(tnode);
                }

                IndexBranch tmptNode;
                tmptNode.key = leafadd.getfront().key;
                tmptNode.ptrChild = buf.bufferBlock[leafadd.bufferNum].blockOffset;
                branchRoot.insert(tmptNode);
                tmptNode.key = leaf.getfront().key;
                tmptNode.ptrChild = buf.bufferBlock[leaf.bufferNum].blockOffset;
                branchRoot.insert(tmptNode);
                return reBranch;
            }
            else{//this leaf is not a root, we have to cascade up
                int bufferNum = buf.addBlockInFile(indexinfor);
                Leaf leafadd(bufferNum);
                leafadd.isRoot = 0;
                leafadd.ptrFather = leaf.ptrFather;
                leafadd.columnLength = leaf.columnLength;
					
                //link the newly added leaf block in the link list of leaf
                leafadd.nextSibling = leaf.nextSibling;
                leafadd.lastSibling = buf.bufferBlock[leaf.bufferNum].blockOffset;
                leaf.nextSibling = buf.bufferBlock[leafadd.bufferNum].blockOffset;
                if(leafadd.nextSibling != 0){
                    int bufferNum = buf.getbufferNum(filename, leafadd.nextSibling);
                    Leaf leafnext(bufferNum, indexinfor);
                    leafnext.lastSibling = buf.bufferBlock[leafadd.bufferNum].blockOffset;
                }
                while(leafadd.nodelist.size() < leaf.nodelist.size()){
                    IndexLeaf tnode = leaf.pop();
                    leafadd.insert(tnode);
                }
                reBranch.key = leafadd.getfront().key;
                reBranch.ptrChild = leaf.nextSibling;
                return reBranch;
            }
        }
        else{//not need to split,just return
            return reBranch;
        }

    }
    else{//not a leaf
        Branch branch(bufferNum, indexinfor);
        list<IndexBranch>::iterator i = branch.nodelist.begin();
        if((*i).key > node.key){
            (*i).key = node.key;
        }
        else{
            for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++)
            if((*i).key > node.key) break;
            i--;
        }
        IndexBranch bnode = insertValue(indexinfor, node, (*i).ptrChild);//go down
			
        if(bnode.key == ""){
            return reBranch;
        }
        else{//bnode.key != "",need to split up
            branch.insert(bnode);
            const int RecordLength = indexinfor.columnLength + POINTERLENGTH;
            const int MaxrecordNum = (BLOCKSIZE-6-POINTERLENGTH) / RecordLength;
            if(branch.recordNum > MaxrecordNum){//need to split up
                if(branch.isRoot){
                    int rbufferNum = branch.bufferNum;	// buffer number for new root
                    branch.bufferNum = buf.addBlockInFile(indexinfor);	//find a new place for old branch
                    int sbufferNum = buf.addBlockInFile(indexinfor);	// buffer number for sibling 
                    Branch branchRoot(rbufferNum);	//new root
                    Branch branchadd(sbufferNum);	//sibling

                    //is root
                    branchRoot.isRoot = 1;
                    branchadd.isRoot = 0;
                    branch.isRoot = 0;

                    branchRoot.ptrFather = branchadd.ptrFather = branch.ptrFather = 0;
                    branchRoot.columnLength = branchadd.columnLength = branch.columnLength;
						
                    while(branchadd.nodelist.size() < branch.nodelist.size()){
                        IndexBranch tnode = branch.pop();
                        branchadd.insert(tnode);
                    }

                    IndexBranch tmptNode;
                    tmptNode.key = branchadd.getfront().key;
                    tmptNode.ptrChild = buf.bufferBlock[branchadd.bufferNum].blockOffset;
                    branchRoot.insert(tmptNode);
                    tmptNode.key = branch.getfront().key;
                    tmptNode.ptrChild = buf.bufferBlock[branch.bufferNum].blockOffset;
                    branchRoot.insert(tmptNode);
                    return reBranch;//here the function must have already returned to the top lay
                }
                else{//branch is not a root
                    int bufferNum = buf.addBlockInFile(indexinfor);
                    Branch branchadd(bufferNum);
                    branchadd.isRoot = 0;
                    branchadd.ptrFather = branch.ptrFather;
                    branchadd.columnLength = branch.columnLength;
						
                    while(branchadd.nodelist.size() < branch.nodelist.size()){
                        IndexBranch tnode = branch.pop();
                        branchadd.insert(tnode);
                    }
                    reBranch.key = branchadd.getfront().key;
                    reBranch.ptrChild = buf.bufferBlock[bufferNum].blockOffset;
                    return reBranch;
                }
            }
            else{//not need to split,just return
                return reBranch;
            }
        }
    }
    return reBranch;//here is just for safe
}
	

Data IndexManager::selectEqual(const Table& tableinfor, const Index& indexinfor, string key, int blockOffset){//start from the root and look down
    Data datas;
    string filename = indexinfor.index_name + ".index";
    int bufferNum = buf.getbufferNum(filename, blockOffset);
    bool isLeaf = ( buf.bufferBlock[bufferNum].values[1] == 'L' );// L for leaf
    if(isLeaf){
        Leaf leaf(bufferNum, indexinfor);
        list<IndexLeaf>::iterator i = leaf.nodelist.begin();
        for(i = leaf.nodelist.begin(); i!= leaf.nodelist.end(); i++)
        if((*i).key == key){
            filename = indexinfor.table_name + ".table";
            int recordBufferNum = buf.getbufferNum(filename, (*i).offsetInFile);
            int position = (tableinfor.totalLength +1)* ((*i).offsetInBlock);
            string stringrow = buf.bufferBlock[recordBufferNum].getvalues(position, position + tableinfor.totalLength);
            if(stringrow.c_str()[0] != EMPTY){
                stringrow.erase(stringrow.begin());
                Row splitedRow = splitRow(tableinfor, stringrow);
                datas.rows.push_back(splitedRow);
                return datas;
            }
        }
    }
    else{	//it is not a leaf
        Branch branch(bufferNum, indexinfor);
        list<IndexBranch>::iterator i = branch.nodelist.begin();
        for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
            if((*i).key > key){
                i--;
                break;
            }
        }
        if(i == branch.nodelist.end()) i--;
        datas = selectEqual(tableinfor, indexinfor, key, (*i).ptrChild);
    }
    return datas;
}	

Data IndexManager::selectBetween(const Table& tableinfor, const Index& indexinfor, string keyFrom, string keyTo, int blockOffset){
    Data datas;
    string filename = indexinfor.index_name + ".index";
    int bufferNum = buf.getbufferNum(filename, blockOffset);
    bool isLeaf = ( buf.bufferBlock[bufferNum].values[1] == 'L' );// L for leaf
    if(isLeaf){
        do{
            Leaf leaf(bufferNum, indexinfor);
            list<IndexLeaf>::iterator i;
            for(i = leaf.nodelist.begin(); i!= leaf.nodelist.end(); i++){
                if((*i).key >= keyFrom){
                    if((*i).key > keyTo){
                        return datas;
                    }
                    filename = indexinfor.table_name + ".table";
                    int recordBufferNum = buf.getbufferNum(filename, (*i).offsetInFile);
                    int position = (tableinfor.totalLength +1)* ((*i).offsetInBlock);
                    string stringrow = buf.bufferBlock[recordBufferNum].getvalues(position, position + tableinfor.totalLength);
                    if(stringrow.c_str()[0] != EMPTY){
                        stringrow.erase(stringrow.begin());
                        Row splitedRow = splitRow(tableinfor, stringrow);
                        datas.rows.push_back(splitedRow);
                    }
                }
            }
            if(leaf.nextSibling != 0){
                filename = indexinfor.index_name + ".index";
                bufferNum = buf.getbufferNum(filename, leaf.nextSibling);
            }
            else return datas;
        }while(1);
    }
    else{//not leaf, go down to the leaf
        Branch branch(bufferNum, indexinfor);
        list<IndexBranch>::iterator i = branch.nodelist.begin();
        if((*i).key > keyFrom){
            datas = selectBetween(tableinfor, indexinfor, keyFrom, keyTo, (*i).ptrChild);
            return datas;
        }
        else{
            for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
                if((*i).key > keyFrom){
                    i--;//�õ�(*i) ��ߵ�ָ���λ��
                    break;
                }
            }
            datas = selectBetween(tableinfor, indexinfor, keyFrom, keyTo, (*i).ptrChild);
            return datas;
        }
    }
    return datas;
}

Row IndexManager::splitRow(Table tableinfor, string row){
    Row splitedRow;
    int s_pos = 0, f_pos = 0;//start position & finish position
    for(int i= 0; i < tableinfor.attriNum; i++){
        s_pos = f_pos;
        f_pos += tableinfor.attributes[i].length;
        string col;
        for(int j = s_pos; j < f_pos; j++){
            if(row[j] == EMPTY) break;
            col += row[j];
        }
        splitedRow.columns.push_back(col);
    }
    return splitedRow;
}

string IndexManager::getColumnValue(const Table& tableinfor, const Index& indexinfor, string row){
    string colValue;
    int s_pos = 0, f_pos = 0;	//start position & finish position
    for(int i= 0; i <= indexinfor.column; i++){
        s_pos = f_pos;
        f_pos += tableinfor.attributes[i].length;
    }
    for(int j = s_pos; j < f_pos && row[j] != EMPTY; j++)	colValue += row[j];
    return colValue;
}
	
void IndexManager::dropIndex(Index& indexinfor){
    string filename = indexinfor.index_name + ".index";
    if( remove(filename.c_str()) != 0 )
    perror( "Error deleting file" );
    else
    buf.setInvalid(filename);
}
