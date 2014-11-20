#include "BufferManager.h"
#include <iostream>

void BufferManager::FlashBack(int bufferID){
    if(!bufferblocks[bufferID].written) return;
    string filename=bufferblocks[bufferID].filename;
    fstream fout(filename.c_str(),ios::in|ios::out);
    fout.seekp(BLOCKSIZE*bufferblocks[bufferID].blockoffset,fout.beg);
    fout.write(bufferblocks[bufferID].getcontents(),BLOCKSIZE);
    bufferblocks[bufferID].initial();
    fout.close();
}

int BufferManager::GetEmptyBlockExcept(string filename){
    int bufferID=-1;
    int min=bufferblocks[0].LRUV;
    for(int i=0;i<MAXBLOCKNUMBER;i++){
        if(!bufferblocks[i].used){
            bufferblocks[i].used=1;
            return i;
        }     
        if(min>=bufferblocks[i].LRUV && bufferblocks[i].filename!=filename){
            min=bufferblocks[i].LRUV;
            bufferID=i;
        }
    }
    if(bufferID==-1){
        cout<<"All the buffer blocks are used.Sorry!"<<endl;
        exit(0);
    }
    FlashBack(bufferID);
    return bufferID;
}

int BufferManager::GetBufferID(string filename,int blockoffset){
    int bufferID=IfinBuffer(filename,blockoffset);
    if(bufferID==-1){
        bufferID=GetEmptyBlockExcept(filename);
        LoadBlock(filename,blockoffset,bufferID);
    }
    return bufferID;
}

void BufferManager::LoadBlock(string filename,int blockoffset,int bufferID){
    bufferblocks[bufferID].used=1;
    bufferblocks[bufferID].written=0;
    bufferblocks[bufferID].filename=filename;
    bufferblocks[bufferID].blockoffset=blockoffset;
    fstream fin(filename.c_str(),ios::in);
    fin.read(bufferblocks[bufferID].getcontents(),BLOCKSIZE);
    fin.close();
}

void BufferManager::WriteBlock(int bufferID){
    bufferblocks[bufferID].used=bufferblocks[bufferID].written=1;
    bufferblocks[bufferID].LRUV++;
}

int BufferManager::GetEmptyBlock(){
    int bufferID=0;
    int min=bufferblocks[0].LRUV;
    for(int i=0;i<MAXBLOCKNUMBER;i++){
        if(!bufferblocks[i].used){
            bufferblocks[i].initial();
            bufferblocks[i].used=1;
            return i;
        }
        if(min>=bufferblocks[i].LRUV){
            min=bufferblocks[i].LRUV;
            bufferID=i;
        }
    }
    FlashBack(bufferID);
    return bufferID;
}



InsertPosition BufferManager::GetInsertPosition(Table& table){
    InsertPosition ip;
    if(!table.blockNum){
        ip.bufferID=AddBlock(table);
        ip.position=0;
        return ip;
    }
    string filename=table.name+".table";
    int length=table.totalLength+1;
    int recordamount=BLOCKSIZE/length;
    int blockoffset=table.blockNum-1;
    int bufferID=IfinBuffer(filename,blockoffset);
    if(bufferID==-1){
        bufferID=GetEmptyBlock();
        LoadBlock(filename,blockoffset,bufferID);
    }
    for(int i=0;i<recordamount;i++){
        int pos=i*length;
        char ifempty=bufferblocks[bufferID].getcontent(pos);
        if(ifempty==EMPTY){
            ip.bufferID=bufferID;
            ip.position=pos;
            return ip;
        }
    }
    ip.bufferID=AddBlock(table);
    ip.position=0;
    return ip;
}
    
int BufferManager::AddBlock(Table& table){
    int bufferID=GetEmptyBlock();
    bufferblocks[bufferID].initial();
    bufferblocks[bufferID].used=bufferblocks[bufferID].written=1;
    bufferblocks[bufferID].filename=table.name+".table";
    bufferblocks[bufferID].blockoffset=table.blockNum++;
    return bufferID;
}
    
int BufferManager::AddBlock(Index& index){ 
    string filename=index.index_name+".index";
    int bufferID=GetEmptyBlockExcept(filename);
    bufferblocks[bufferID].initial();
    bufferblocks[bufferID].used=bufferblocks[bufferID].written=1;
    bufferblocks[bufferID].filename=filename;
    bufferblocks[bufferID].blockoffset=index.blockNum++;
    return bufferID;
}
    
int BufferManager::IfinBuffer(string filename,int blockoffset){
    for(int i=0;i<MAXBLOCKNUMBER;i++)
        if(bufferblocks[i].filename==filename && bufferblocks[i].blockoffset==blockoffset)
            return i;
    return -1;
}

    
void BufferManager::LoadTable(Table table){
    string filename=table.name+".table";
    fstream fin(filename.c_str(),ios::in);
    for(int i=0;i<table.blockNum;i++)
        if(IfinBuffer(filename,i)==-1){
            int bufferID=GetEmptyBlockExcept(filename);
            LoadBlock(filename,i,bufferID);
        }
    fin.close();
}
    
void BufferManager::SetUnused(string filename){
    for(int i=0;i<MAXBLOCKNUMBER;i++)
        if(bufferblocks[i].filename==filename)
            bufferblocks[i].used=bufferblocks[i].written=0;
}
    
