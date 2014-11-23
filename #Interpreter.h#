#ifndef __INTERPRET_H__
#define __INTERPRET_H__

#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

extern CatalogManager catalog; 
extern RecordManager  record;

class Interpreter{
public:
	int m_operation;
	string m_tabname;
	string m_indname;
	string m_colname;
	vector<Attribute> column;
	vector<Condition> condition;
	Row row;
	Table getTableInfo;
	Index getIndexInfo;
	int PrimaryKeyPosition;
	int UniquePostion;
	vector<Condition> UniqueCondition;
	//static CCatalogManager Catalog;
	friend void Execute();
	Interpreter(){
		m_operation =UNKNOW;
		m_tabname = "";
		m_indname = "";
		m_colname="";
		PrimaryKeyPosition=-1;	
		UniquePostion=-1;
	}
	~Interpreter(){}

	void Parse(char* command);
    void initcol();
	void initcond();
	void initvalue();
	void inittable(){
		getTableInfo.name="";
		getTableInfo.attriNum=getTableInfo.blockNum=getTableInfo.totalLength=0;
		getTableInfo.attributes.clear();
	}
	void initindex(){
		getIndexInfo.blockNum=getIndexInfo.column=-1;
		getIndexInfo.index_name="";
		//getIndexInfo.table_name="";
	}
	bool GetWord(string & src, string & des);

protected:
	void MakeInitilate();
	//bool GetWord(string & src, string & des);
	bool GetStr(string & src, string & des);
	short int IsInt(const char *);
	short int IsFloat(char *input);
};

#endif
