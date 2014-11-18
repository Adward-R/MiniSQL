#include "Interpreter.h"

//enum Operation{CREATABLE, INSERT, SELECT, DELECT, DROPINDEX, CRETEINDEX, DROPTABLE, ERROR};
//Interpreter interprt; 
RecordManager record;
IndexManager indexm;
CatalogManager catalog;
Interpreter parsetree;
BufferManager buf;

void welcome();
void ShowResult(Data data, Table tableinfor, vector<Attribute> column);
void Execute();
void AddSeperator(char *command);
short int IsComEnd(char *input);



int main()
{
	welcome();
	vector<Condition> conditions;
	Table tableinfor;
	Index indexinfor;
	Row insertValue;
	Data datas;
	char command[COMLEN] = "";
	char input[INPUTLEN] = "";
	char word[WORDLEN] = "";
	short int ComEnd = 0;
	/*FILE *stream;
	stream=freopen("instruction10.txt","r",stdin);*/
	/*FILE *fileout;
	fileout=freopen("result.txt","w",stdout);*/

	//int count = 0;
	while(1)
	{
		strcpy(command, "");//command����
		ComEnd = 0;
		cout<<" Adward's Database >>";
 		while(!ComEnd)
		{
	/*		count ++;
			if(count % 100 == 0)
				cout << count/100 << "%" << endl;*/
			gets(input);
			if(IsComEnd(input))
				ComEnd = 1;
				strcat(command, input);
			AddSeperator(command);
		}
		parsetree.Parse(command);
		Execute();
	}
	getchar();
	return 0;
}

void welcome(){
	cout << "____Welcome to our micro database system____" << endl;
}

void ShowResult(Data data, Table tableinfor, vector<Attribute> column){
	if(column[0].name == "*"){
		cout << endl <<"+";
		for(int i = 0; i < tableinfor.attriNum; i++){
			for(int j = 0; j < tableinfor.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
		cout << "| ";
		for(int i = 0; i < tableinfor.attriNum; i++){
			cout << tableinfor.attributes[i].name;
			int lengthLeft = tableinfor.attributes[i].length - tableinfor.attributes[i].name.length();
			for(int j = 0; j < lengthLeft; j++){
				cout << ' ';
			}
			cout << "| ";
		}
		cout << endl;
		cout << "+";
		for(int i = 0; i < tableinfor.attriNum; i++){
			for(int j = 0; j < tableinfor.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;

		//����
		for(int i = 0; i < data.rows.size(); i++){
			cout << "| ";
			for(int j = 0; j < tableinfor.attriNum; j++){
				int lengthLeft = tableinfor.attributes[j].length;
				for(int k =0; k < data.rows[i].columns[j].length(); k++){
					if(data.rows[i].columns[j].c_str()[k] == EMPTY) break;
					else{
						cout << data.rows[i].columns[j].c_str()[k];
						lengthLeft--;
					}
				}
				for(int k = 0; k < lengthLeft; k++) cout << " ";
				cout << "| ";
			}
			cout << endl;
		}

		cout << "+";
		for(int i = 0; i < tableinfor.attriNum; i++){
			for(int j = 0; j < tableinfor.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
	}
	else{
		cout << endl <<"+";
		for(int i = 0; i < column.size(); i++){
			int col;
			for(col = 0; col < tableinfor.attributes.size(); col++){
				if(tableinfor.attributes[col].name == column[i].name) break;
			}
			for(int j = 0; j < tableinfor.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
		cout << "| ";
		for(int i = 0; i < column.size(); i++){
			int col;
			for(col = 0; col < tableinfor.attributes.size(); col++){
				if(tableinfor.attributes[col].name == column[i].name) break;
			}
			cout << tableinfor.attributes[col].name;
			int lengthLeft = tableinfor.attributes[col].length - tableinfor.attributes[col].name.length();
			for(int j = 0; j < lengthLeft; j++){
				cout << ' ';
			}
			cout << "| ";
		}
		cout << endl;
		cout << "+";
		for(int i = 0; i < column.size(); i++){
			int col;
			for(col = 0; col < tableinfor.attributes.size(); col++){
				if(tableinfor.attributes[col].name == column[i].name) break;
			}
			for(int j = 0; j < tableinfor.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;

		//����
		for(int i = 0; i < data.rows.size(); i++){
			cout << "| ";
			for(int j = 0; j < column.size(); j++){
				int col;
				for(col = 0; col < tableinfor.attributes.size(); col++){
					if(tableinfor.attributes[col].name == column[j].name) break;
				}
				int lengthLeft = tableinfor.attributes[col].length;
				for(int k =0; k < data.rows[i].columns[col].length(); k++){
					if(data.rows[i].columns[col].c_str()[k] == EMPTY) break;
					else{
						cout << data.rows[i].columns[col].c_str()[k];
						lengthLeft--;
					}
				}
				for(int k = 0; k < lengthLeft; k++) cout << " ";
				cout << "| ";
			}
			cout << endl;
		}

		cout << "+";
		for(int i = 0; i < column.size(); i++){
			int col;
			for(col = 0; col < tableinfor.attributes.size(); col++){
				if(tableinfor.attributes[col].name == column[i].name) break;
			}
			for(int j = 0; j < tableinfor.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
	}
	cout << data.rows.size() << " rows have been found."<< endl;
}
void Execute()
{	
	int i;
	int j;
	int k;
	Table tableinfor;
	Index indexinfor;
	string tempKeyValue;
	int tempPrimaryPosition=-1;
	int rowCount=0;
	Data data;
	switch(parsetree.m_operation)
	{
	case CRETAB:
		parsetree.getTableInfo.attriNum=parsetree.getTableInfo.attributes.size();
		catalog.createTable(parsetree.getTableInfo);
		record.createTable(parsetree.getTableInfo);
		cout<<"Table "<<parsetree.getTableInfo.name<<" has been created successfully"<<endl;
		break;
	case TABLEEXISTED:
		cout<<"The table has been created,please check the database"<<endl;
		break;
	case DRPTAB:
		record.dropTable(parsetree.getTableInfo);
		for(int i = 0; i < parsetree.getTableInfo.attriNum; i++){//���������е�index��ɾ��
			indexinfor = catalog.getIndexInformation(parsetree.getTableInfo.name, i);
			if(indexinfor.index_name != "")
				indexm.dropIndex(indexinfor);
		}
		catalog.dropTable(parsetree.getTableInfo);
		cout<<"Table "<<parsetree.getTableInfo.name<<" has been dropped successfully"<<endl;
		break;
	case INSERT:
		tableinfor = parsetree.getTableInfo;
		if(parsetree.PrimaryKeyPosition==-1&&parsetree.UniquePostion==-1){
			record.insertValue(tableinfor, parsetree.row);
			catalog.update(tableinfor);
			cout<<"One record has been inserted successfully"<<endl;
			break;
		}
		if(parsetree.PrimaryKeyPosition!=-1)
		{
			data=record.select(tableinfor, parsetree.condition);
			if(data.rows.size()>0){
				cout<<"Primary Key Redundancy occurs, thus insertion failed"<<endl;
				break;
			}
		}
		if(parsetree.UniquePostion!=-1){
			
			data=record.select(tableinfor, parsetree.UniqueCondition);
			if(data.rows.size()>0){
				cout<<"Unique Value Redundancy occurs, thus insertion failed"<<endl;
				break;
			}
		}
		record.insertValue(tableinfor,parsetree.row);
		catalog.update(tableinfor);
		cout<<"One record has been inserted successfully"<<endl;
		break;
	case INSERTERR:
		cout << "Incorrect usage of \"insert\" query! Please check your input!" << endl;
		break;
	case SELECT_NOWHERE_CAULSE:
		tableinfor = parsetree.getTableInfo;
		data=record.select(tableinfor);
		if(data.rows.size()!=0)
			ShowResult( data, tableinfor, parsetree.column);
		else{
			cout << "No data is found!!!" << endl;
		}
		break;
	case SELECT_WHERE_CAULSE:
		tableinfor=parsetree.getTableInfo;
		if(parsetree.condition.size()==1){
			for(int i=0;i<parsetree.getTableInfo.attributes.size();i++){
		/*�޸�*/if((parsetree.getTableInfo.attributes[i].isPrimeryKey==true||parsetree.getTableInfo.attributes[i].isUnique==true)&&parsetree.m_colname==parsetree.getTableInfo.attributes[i].name){
					tempPrimaryPosition=i;
					indexinfor=catalog.getIndexInformation(tableinfor.name,i);
					break;
				}
			}
			if(tempPrimaryPosition==parsetree.condition[0].columnNum&&parsetree.condition[0].op==Eq&&indexinfor.table_name!=""){
				
				tempKeyValue=parsetree.condition[0].value;
				data= indexm.selectEqual(tableinfor,indexinfor,tempKeyValue);
			}
			else{

				data=record.select(tableinfor,parsetree.condition);
			}
		}
		else{
			data=record.select(tableinfor,parsetree.condition);
		}
		if(data.rows.size()!=0)
			ShowResult( data, tableinfor, parsetree.column);
		else{
			cout << "No data is found!!!" << endl;
		}
		break;
	case DELETE:
		rowCount = record.deleteValue(parsetree.getTableInfo,parsetree.condition);
		cout<< rowCount <<"  rows have been deleted."<<endl;
		break;
	case CREIND:
		tableinfor = parsetree.getTableInfo;
		indexinfor = parsetree.getIndexInfo;
		if(!tableinfor.attributes[indexinfor.column].isPrimeryKey&&!tableinfor.attributes[indexinfor.column].isUnique){//����primary key�������Խ�index
			cout << "Column " << tableinfor.attributes[indexinfor.column].name <<"  is not unique."<< endl;
			break;
		}
		catalog.createIndex(indexinfor);
		indexm.createIndex(tableinfor, indexinfor);
		catalog.update(indexinfor);
		cout<<"The index "<< indexinfor.index_name << "has been created successfully"<<endl;
		break;
	case INDEXERROR:
		cout<<"The index on primary key of table has been existed"<<endl;
		break;
	case DRPIND:
		indexinfor = catalog.getIndexInformation(parsetree.m_indname);
		if(indexinfor.index_name == ""){
			cout << "Index" << parsetree.m_indname << "does not exist!" << endl;
		}
		indexm.dropIndex(indexinfor);
		catalog.dropIndex(parsetree.m_indname);
		cout<<"The index has been dropped successfully"<<endl;
		break;
	case CREINDERR:
		cout << "Incorrect usage of \"create index\" query! Please check your input!" << endl;
		break;
	case QUIT:
		cout << "Have a good day! Press any key to close this window." << endl;
		getchar();
		exit(0);
		break;
	case EMPTY:
		cout << "Empty query! Please enter your command!" << endl;
		break;
	case UNKNOW:
		cout << "UNKNOW query! Please check your input!" << endl;
		break;
	case SELERR:
		cout << "Incorrect usage of \"select\" query! Please check your input!" << endl;
		break;
	case CRETABERR:
		cout << "Incorrect usage of \"create table\" query! Please check your input!" << endl;
		break;
	case DELETEERR:
		cout << "Incorrect usage of \"delete from\" query! Please check your input!" << endl;
		break;
	case DRPTABERR:
		cout << "Incorrect usage of \"drop table\" query! Please check your input!" << endl;
		break;
	case DRPINDERR:
		cout << "Incorrect usage of \"drop index\" query! Please check your input!" << endl;
		break;
	case VOIDPRI:
		cout << "Error: invalid primary key! Please check your input!" << endl;
		break;
	case VOIDUNI:
		cout << "Error: invalid unique key! Please check your input!" << endl;
		break;
	case CHARBOUD:
		cout << "Error: only 1~255 charactors is allowed! Please check your input!" << endl;
		break;
	case NOPRIKEY:
		cout << "No primary key is defined! Please check your input!" << endl;
		break;
	case TABLEERROR:
		cout<<"Table is not existed,please check the database"<<endl;
		break;
	case INDEXEROR:
		cout<<"Index is not existed,please check the database"<<endl;
		break;
	case COLUMNERROR:
		cout<<"One column is not existed"<<endl;
		break;
	case INSERTNUMBERERROR:
		cout<<"The column number is not according to the columns in our database"<<endl;
		break;
	}
	
}
void AddSeperator(char *command)
{
	unsigned len = strlen(command);
	command[len] = ' ';
	command[len + 1] = '\0';
}

short int IsComEnd(char *input)
{
	unsigned int next = strlen(input) - 1;
	char prev = ' ';
	while(prev == '\t' || prev ==' ')
	{
		prev = input[next];
		next --;
	}
	if(prev == ';')
	{
		input[next + 1] ='\0';
		return 1;
	}
	return 0;
}
