#ifndef __buffer_h__
#define __buffer_h__
#include <fstream>
#include <string>
#include <vector> 
#include "Macro.h"
#include "DSL_miniSQL.h"

class BufferManager{
private: 
    BufferBlock bufferblocks[MAXBLOCKNUMBER];     
    int GetBufferID(string filename,int blockoffset);
    void LoadBlock(string filename,int blockoffset,int bufferID);
    void WriteBlock(int bufferID);
    void LRUBlock(int bufferID);
    int GetEmptyBlock();
    int GetEmptyBlockExcept(string filename);
    InsertPosition GetInsertPosition(Table& table);
    int AddBlock(Table& table);
    int AddBlock(Index& index);
    int IfinBuffer(string filename,int blockoffset);
    void LoadTable(Table table);
    void SetUnused(string filename);
    void FlashBack(int bufferID);
public:  
    friend class RecordManager;
    friend class IndexManager;
    friend class Leaf;
    friend class Branch;
    BufferManager(){
        for(int i=0;i<MAXBLOCKNUMBER;bufferblocks[i++].initial());
    }
    ~BufferManager(){
        for(int i=0;i<MAXBLOCKNUMBER;FlashBack(i++));
    }
    char GetBlockContent(int bufferID,int position){
        return bufferblocks[bufferID].getcontent(position);
    }
    string GetBlockContents(int bufferID,int begin,int end){
        return bufferblocks[bufferID].getcontents(begin,end);
    }
    void SetBlockContent(int bufferID,int position,char c){
        bufferblocks[bufferID].setcontent(position,c);
    }
    /*
    static void ShowBuffer(int begin, int end){
	int max = 25;
        if(begin<0 || begin>end || end>=MAXBLOCKNUMBER){
	    cout << "no such buffer blocks" <<endl;
            return;
	}
	if((end-begin) > max){
	    cout << "Too many buffer blocks!" << endl;
	    cout << "At most" << max <<"buffers once." << endl;
            return; 
	}
        for(int i = begin; i <= end; i++){
	    ShowBuffer(i);
	}
    }
    static void ShowBuffer(int bufferID){
	cout << "BlockID: " << bufferID << endl;
	cout << "Written: " << bufferblocks[bufferID].written << endl;
	cout << "Used: " << bufferblocks[bufferID].used << endl;
	cout << "Filename: " << bufferblocks[bufferID].filename << endl;
	cout << "blockoffset: " << bufferblocks[bufferID].blockoffset << endl;
	cout << bufferblocks[bufferID].contents <<endl;
   } */
};

#endif
