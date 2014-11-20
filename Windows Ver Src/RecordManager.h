#ifndef _RECORD_H_
#define _RECORD_H_
#include "BufferManager.h"
#include "DSL_miniSQL.h"
#include "Macro.h"
#include <iostream>
#include <vector>

extern BufferManager buf;
class RecordManager{
private:
	Row SplitRow(Table table, string row);

	string ConnectRow(Table table, Row splitedrow);

public:
	bool Comparator(Table table, Row row, vector<Condition> conditions);

	Data Select(Table& table);
	
	Data Select(Table table, vector<Condition> conditions);
	
	void InsertValue(Table& table, Row& splitedrow);
	
	int DeleteValue(Table table);
	
	int DeleteValue(Table table, vector<Condition> conditions);
	
	void DropTable(Table table);
	
	void CreateTable(Table table);
	
	void ShowDatas(const Data& datas) const{
            if(!datas.rows.size()) {
		cout<<"No Data"<<endl;
		return;
	    }
	    for(int i=0;i<datas.rows.size();i++){
		cout<<(i+1)<<" ";//ÐòºÅ 
		for(int j=0;j<datas.rows[i].columns.size();j++){
		    for(int k=0;k<datas.rows[i].columns[j].length();k++)
		   	if(datas.rows[i].columns[j].c_str()[k]==EMPTY) break;
			else cout<<datas.rows[i].columns[j].c_str()[k];
			cout<<'\t';
		}
		cout<<endl;
	    }
        }
};

#endif
