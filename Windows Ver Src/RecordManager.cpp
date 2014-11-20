#include "RecordManager.h"

Row RecordManager::SplitRow(Table table, string row){
    Row pieces;
    int begin=0,end=0;
    for(int i=0;i<table.attriNum;i++){
        begin=end;
	    end+=table.attributes[i].length;
	    string colomn;       
	    for(int j=begin;j<end;j++)colomn+=row[j];
        pieces.columns.push_back(colomn);
    }
    return pieces;
}
	
string RecordManager::ConnectRow(Table table,Row splitedrow){
    string temp,stringrow;
    for(int i=0;i<splitedrow.columns.size();i++){
        temp=splitedrow.columns[i];
        for(;temp.length()<table.attributes[i].length;temp+= EMPTY);
        stringrow+=temp;
    }
    return stringrow;
}

bool RecordManager::Comparator(Table table,Row row,vector<Condition>conditions){
    for(int i=0;i<conditions.size();i++){
        int columnNum=conditions[i].columnNum;
        string s1="";
        string s2=conditions[i].value;
	    int l1=0;
	    int l2=s2.length();
     	for(int k=0;k<row.columns[columnNum].length();k++){
	        if(row.columns[columnNum].c_str()[k] == EMPTY){
                l1=k;
                break;
            }
	        s1+=row.columns[columnNum].c_str()[k];
	    }
	    float f1=atof(s1.c_str());
	    float f2=atof(s2.c_str());
	    switch(table.attributes[columnNum].type){
	    case CHAR:
            switch(conditions[i].op){
            case Lt:	
            if(s1<s2) return 0;
		    break;
	        case Le:
		    if(s1>s2) return 0;
		    break;
	        case Gt:
		    if(s1<=s2) return 0;
		    break;
	        case Ge:
		    if(s1<s2) return 0;
		    break;
	        case Eq:
		    if(s1!=s2) return 0;
		    break;
	        case Ne:
		    if(s1==s2) return 0;
		     break;
	        }	
	        break;
    	case INT:
	        switch(conditions[i].op){
		    case Lt:	
	            if(l1>l2) return 0;
		        if(l1<l2) break;
	            if(s1>=s2) return 0;
		        break;
		    case Le:
		        if(l1>l2) return 0;
		        if(l1<l2) break;
		        if(s1>s2) return 0;
		        break;
		    case Gt:
		        if(l1<l2) return 0;
		        if(l1>l2) break;
		        if(s1<=s2) return 0;
		        break;
		    case Ge:
		        if(l1<l2) return 0;
		        if(l1>l2) break;
		        if(s1<s2) return 0;
		        break;
	        case Eq:
		        if(l1!=l2) return 0;
		        if(s1!=s2) return 0;
		        break;
		    case Ne:
		        if(l1!=l2) break;
		        if(s1==s2) return 0;
		        break;
	        }
	        break;
	    case FLOAT:
	        switch(conditions[i].op){
	        case Lt:	
	        if(f1>=f2) return 0;
		    break;
		    case Le:
		    if(f1>f2) return 0;
		    break;
		    case Gt:
		    if(f1<=f2) return 0;
		    break;
		    case Ge:
		    if(f1<f2) return 0;
		    break;
		    case Eq:
		    if(f1!=f2) return 0;
		    break;
		    case Ne:
		    if(f1==f2) return 0;
		    break;
		    }
        }
    }  
    return 1;
}

Data RecordManager::Select(Table& table){
    string filename=table.name+".table";
    string stringrow;
    Row pieces;
    Data datas;
    int length=table.totalLength+1;//加多一位来判断这条记录是否被删除了
    int recordNum=BLOCKSIZE/length;
    for(int i=0;i<table.blockNum;i++){
        int bufferID=buf.IfinBuffer(filename,i);
	if(bufferID==-1){
	    bufferID=buf.GetEmptyBlock();
	    buf.LoadBlock(filename,i,bufferID);
	}
        for(int j=0;j<recordNum;j++){
            int position=j*length;
            stringrow=buf.bufferblocks[bufferID].getcontents(position, position+length);
	        if(stringrow.c_str()[0] == EMPTY) continue;//inticate that this row of record have been deleted
                stringrow.erase(stringrow.begin());//把第一位去掉
	        pieces=SplitRow(table,stringrow);
	        datas.rows.push_back(pieces);
        }
    }
    return datas;
}
	
Data RecordManager::Select(Table table,vector<Condition>conditions){
    Data datas;
    string stringrow;
    Row pieces;
    if(conditions.size()==0){
        datas=Select(table);
	    return datas;
    }
    string filename=table.name+".table";
    int length=table.totalLength+1;//加多一位来判断这条记录是否被删除了
    int recordNum=BLOCKSIZE/length;
    for(int i=0;i<table.blockNum;i++){
	    int bufferID=buf.IfinBuffer(filename,i);
	    if(bufferID==-1){
	        bufferID=buf.GetEmptyBlock();
	        buf.LoadBlock(filename,i,bufferID);
        }
        for(int j=0;j<recordNum;j++){
            int position=j*length;
	        stringrow=buf.bufferblocks[bufferID].getcontents(position,position+length);
	        if(stringrow.c_str()[0]!=EMPTY){
	            stringrow.erase(stringrow.begin());//把第一位去掉
		        pieces=SplitRow(table, stringrow);
		        if(Comparator(table,pieces,conditions))//如果满足条件，就把结果push到datas里面去
		            datas.rows.push_back(pieces);  
	        }
        }
    }
    return datas;
}
	
void RecordManager::InsertValue(Table& table, Row& splitedrow){
    string stringrow=ConnectRow(table, splitedrow);
    InsertPosition ip=buf.GetInsertPosition(table);
    buf.bufferblocks[ip.bufferID].setcontent(ip.position,NOTEMPTY);
    for(int i=0;i<table.totalLength;i++){
        buf.bufferblocks[ip.bufferID].setcontent(ip.position+1+i,stringrow.c_str()[i]);
        buf.bufferblocks[ip.bufferID].written=1;;
    }
}
 
int RecordManager::DeleteValue(Table table){
    string filename=table.name+".table";
    int position,count=0;
    int recordNum=BLOCKSIZE/(table.totalLength+1);	//加多一位来判断这条记录是否被删除了
    for(int i=0;i<table.blockNum;i++){
        int bufferID=buf.IfinBuffer(filename,i);
	    if(bufferID==-1){
	        bufferID=buf.GetEmptyBlock();
	        buf.LoadBlock(filename,i,bufferID);
	    }
        for(int j=0;j<recordNum;j++){
            position=j*(table.totalLength+1);
	        if(buf.bufferblocks[bufferID].getcontent(position)!=EMPTY){
	            buf.bufferblocks[bufferID].setcontent(position,EMPTY);
	    	    count++;
	        }
        }
    }
    return count;
}
	
int RecordManager::DeleteValue(Table table,vector<Condition>conditions){
    string filename=table.name+".table";
    string stringrow;
    Row pieces;
    int position,count=0;
    int recordNum=BLOCKSIZE/(table.totalLength+1);
    for(int i=0;i<table.blockNum;i++){
	    int bufferID=buf.IfinBuffer(filename,i);
	    if(bufferID==-1){
	        bufferID=buf.GetEmptyBlock();
	        buf.LoadBlock(filename,i,bufferID);
	    }
        for(int j=0;j<recordNum;j++){
            position=j*(table.totalLength+1);
	        stringrow=buf.bufferblocks[bufferID].getcontents(position,position+table.totalLength+1);
	        if(stringrow.c_str()[0]!=EMPTY){
	            stringrow.erase(stringrow.begin());//把第一位去掉
		        pieces=SplitRow(table,stringrow);
		        if(Comparator(table,pieces,conditions)){//如果满足条件，就把记录delete掉 
		            buf.bufferblocks[bufferID].setcontent(position,DELETED);
		            count++;
                }
            }
        }
        buf.bufferblocks[bufferID].written=1;;
    }
    return count;
}
	
void RecordManager::DropTable(Table table){
    string filename=table.name+".table";
    if(remove(filename.c_str())!=0)
        perror( "Error deleting file" );
    else
	buf.SetUnused(filename);//when a file is deleted, a table or an index, all the value in buffer should be set invalid
}
	
void RecordManager::CreateTable(Table table){
    string filename=table.name+".table";
    fstream fout(filename.c_str(),ios::out);
    fout.close();
}
