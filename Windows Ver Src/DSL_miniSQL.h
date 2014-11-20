#ifndef __DSL_miniSQL_h__
#define __DSL_miniSQL_h__
#include <string>
#include <vector>
#include "Macro.h"
using namespace std;

class Attribute
{
public:
	string name;
	int type;
	int length;
	bool isPrimeryKey;
	bool isUnique;
	Attribute()
	{
	 isPrimeryKey=false;
	 isUnique=false;
	}
	Attribute(string n, int t, int l, bool isP, bool isU)
		:name(n), type(t), length(l), isPrimeryKey(isP), isUnique(isU){}
};

class Table
{
public:
	string name;   //all the datas is store in file name.table
	int blockNum;	//number of block the datas of the table occupied in the file name.table
	//int recordNum;	//number of record in name.table
	int attriNum;	//the number of attributes in the tables
	int totalLength;	//total length of one record, should be equal to sum(attributes[i].length)
	vector<Attribute> attributes;
	Table(): blockNum(0), attriNum(0), totalLength(0){}
};

class Index
{
public:
	string index_name;	//all the datas is store in file index_name.index
	string table_name;	//the name of the table on which the index is create
	int column;			//on which column the index is created
	int columnLength;
	int blockNum;		//number of block the datas of the index occupied in the file index_name.table
	Index(): column(0), blockNum(0){}
};

class Row
{
public:
	vector<string> columns;
};
class Data
{
public:
	vector<Row> rows;
};

enum Comparison{Lt, Le, Gt, Ge, Eq, Ne};//stands for less than, less equal, greater than, greater equal, equal, not equal respectivly
class Condition{
public:
	Comparison op;
	int columnNum;
	string value;
};

class BufferBlock{
private:
    char contents[BLOCKSIZE+1];
public:
    bool written;
    bool used;
    string filename;
    int blockoffset;
    int LRUV;
    BufferBlock(){
        initial();
    }
    void initial(){
        written=used=blockoffset=LRUV=0;
        filename="NULL";
        for(int i=0;i<BLOCKSIZE;contents[i++]=EMPTY);
        contents[BLOCKSIZE]='\0';
    }	
    string getcontents(int begin,int end){
        string temp="";
        if(begin<0 || begin>end || end>BLOCKSIZE) return temp;
        for(int i=begin;i<end;temp+=contents[i++]);
        return temp;
    }
    char getcontent(int num){
        if(num<0 || num>BLOCKSIZE) return '\0';
        return contents[num]; 
    }
    void setcontent(int num,char c){
         contents[num]=c;
    }
    char* getcontents(){
          return contents;
    }
};

class InsertPosition{
public:
	int bufferID;
	int position;
};

#endif
