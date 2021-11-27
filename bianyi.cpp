#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<unordered_map>
#include<set>
#define IL (raw[col]>='a'&&raw[col]<='z')||(raw[col]>='A'&&raw[col]<='Z')
#define ID (raw[col]>='0'&&raw[col]<='9')
using namespace std;
void block();
void Var();
void Const();
void statement();
void exp();
void lexp();
void body();
void term();
void factor();
void ifendline();//如果读到分号 进入这个函数判断下一个是不是行结束符 如果不是报错然后跳到下一行
void gotonextline();//出错后跳到下一行
//读文件的类
class readcode{
	private:
		ifstream ifs;
		vector<string> buf;
	public:
		readcode(){
			ifs.open("D:\\Codes\\compile\\pascal.txt",ios::in);
			buf.clear();
		}
		readcode(string text){
			ifs.open(text,ios::in);
			buf.clear();
		}
		bool readbuf(){
			if(!ifs.is_open())return false;
			string temp;
			while(getline(ifs,temp)){
				buf.push_back(temp);
			}
			return 1;
		}
		vector<string> getbuf(){
			return this->buf;
		}
};
//记录每个词的信息
typedef struct INT{
	int type;
	int xp;
	int yp;
}INT; 
/*
行结束符
用来出错后跳过该行到下一�?
设string为stop
INT.type=-10
*/
vector<pair<string,INT>> ANS;//string 是内�? int 是类�?  每行结束插入一个行结束�?
vector<string> ERR;//错误集合
set<string> proce;
string ERROR;//错误信息
//存储常量和变�? 常量标识符和数字 变量只有标识�? 数字取INT_MAX
unordered_map<string,int> nums;
//0是关键字 1是op 2是lop �?=是赋�?   10是标识符  9是数�?
unordered_map<string,int> MP={
	{"if",0},{"then",0},{"begin",0},{"else",0},{"end",0},{"while",0},{"for",0},
	{"var",0},{"const",0},{"program",0},{"procedure",0},{"do",0},{"call",0},{"read",0},{"write",0},
	{"+",1},{"*",1},{"-",1},{"/",1},
	{"=",2},{">",2},{"<",2},{">=",2},{"<=",2},{"<>",2},{"odd",2},
	{":=",3},
	{"(",4},{"[",4},{"]",4},{")",4},
	{",",5},{";",5}
};
//词法分析
void analysis(string raw,int row){
	int col=0;
	int n=raw.size();
	string useforadd;
	if(n==0)return;
	string temp="";
	INT z;
	//如果第一个字符是字母
	while(col<n){
		while(col<n&&raw[col]==' ')++col;
		if(IL){
			while(col<n&&(IL||ID)){
				temp+=raw[col];
				++col;
			}
			if(MP.count(temp)){
				z.type=0;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(temp,z));
			}
			else{
				z.type=10;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(temp,z));
			}
			temp="";
		}
		else if(ID){
			while(col<n&&ID){
				temp+=raw[col];
				++col;
			}
			//数字后面跟字�? 不符合变量命名规�?
			if(IL){
				ERROR=to_string(row)+" "+to_string(col)+" Identifier error " +temp;
				ERR.push_back(ERROR);
			}
			//数字范围太大 溢出
			else if(temp.length()<13){
				z.type=9;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(temp,z));//9是数�?
			}
			//数字命名错误
			else{
				ERROR=to_string(row)+" "+to_string(col)+" Number error "+temp;
				ERR.push_back(ERROR);
			}
			temp="";
		}
		//需要判断是不是负数
		else if(raw[col]=='-'){
			temp+="-";
			++col;
			//如果-后面跟了数字
			if(col<n&&ID){
				while(col<n&&ID){
					temp+=raw[col];
					++col;
				}
				//数字后面跟字�? 不符合变量命名规�?
				if(IL){
					ERROR=to_string(row)+" "+to_string(col)+" Identifier error " +temp;
					ERR.push_back(ERROR);
				}
				//数字范围太大 溢出
				else if(temp.length()<13){
					z.type=9;
					z.yp=col;
					z.xp=row;
					ANS.push_back(make_pair(temp,z));//9是数�?
				}
				//数字命名错误
				else{
					ERROR=to_string(row)+" "+to_string(col)+" Number error "+temp;
					ERR.push_back(ERROR);
				}
			}
			//不是数字
			else{
				z.type=MP[temp];
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(temp,z));
			}
			temp="";
		}
		//需要判断是不是赋值符�?
		else if(raw[col]==':'){
			++col;
			if(col<n&&raw[col]=='='){
				z.type=3;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(":=",z));
				++col;
			}
			else{
				z.type=-1;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(temp,z));
				ERROR=to_string(row)+" "+to_string(col)+" Symbol error " + raw[col-1];
				ERR.push_back(ERROR);
			}
		}
		else{
			useforadd=raw[col];
			if(MP.count(useforadd)){
				if(raw[col]=='<'){
					++col;
					if(col<n&&raw[col]=='='){
						//ADDONE("<=",2);
						useforadd+="=";
						z.type=2;
						z.yp=col;
						z.xp=row;
						ANS.push_back(make_pair(useforadd,z));
						++col;
					}
					else if(col<n&&raw[col]=='>'){
						//ADDONE("<>",2);
						useforadd+=">";
						z.type=2;
						z.yp=col;
						z.xp=row;
						ANS.push_back(make_pair(useforadd,z));
						++col;
					}
					else{
						--col;
						//ADDONE("<",2);
						z.type=2;
						z.yp=col;
						z.xp=row;
						ANS.push_back(make_pair(useforadd,z));
					}
				}
				else if(raw[col]=='>'){
					++col;
					if(col<n&&raw[col]=='='){
						//ADDONE(">=",2);
						useforadd+="=";
						z.type=2;
						z.yp=col;
						z.xp=row;
						ANS.push_back(make_pair(useforadd,z));
						++col;
					}
					else{
						--col;
						//ADDONE(">",2);
						z.type=2;
						z.yp=col;
						z.xp=row;
						ANS.push_back(make_pair(useforadd,z));
					}
				}
				else{
					useforadd=raw[col];
					//ADDONE(useforadd,MP[useforadd]);
					z.type=MP[useforadd];
					z.yp=col;
					z.xp=row;
					ANS.push_back(make_pair(useforadd,z));
				}
				++col;
			}
			//错误
			else{
				useforadd=raw[col];
				//ADDONE(useforadd,0,row,col);
				++col;
				z.type=-1;
				z.yp=col;
				z.xp=row;
				ANS.push_back(make_pair(useforadd,z));
				ERROR=to_string(row)+" "+to_string(col)+" Symbol error "+useforadd;
				ERR.push_back(ERROR);
			} 
		}
	}
}

void deal_code(){
	readcode * rd=new readcode();
	rd->readbuf();
	vector<string> temp=rd->getbuf();
	INT z;
	z.type=-10;
	z.yp=-10;
	//for(string t:temp)cout<<t<<"\n";
	for(int i=0;i<temp.size();++i){
		analysis(temp[i],i);
		z.xp=i+1;
		ANS.push_back(make_pair("stop",z));//每行尾加一个stop更好的恐慌处理
	}
}
int MOVE=0;
//读下一个内容
pair<string,INT> get_pair(){
	if(MOVE<ANS.size())
	{
		return ANS[MOVE++];
	}
	else{
		INT z;
		z.type=-1;
		z.xp=-1;
		z.yp=-1;
		return make_pair("null",z);
	}
}
//回退到上一个内容
void withdraw(){
	MOVE--;
}
//判断是否到了行尾
void ifendline(){
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first!="stop"){
		ERROR = to_string(pir.second.xp+1) + " line has an error nearby please check ";
		ERR.push_back(ERROR);
		while(pir.first!="stop"){
			pir=get_pair();
		}
	}
	else{
		//pir=get_pair();
	}
}
//跳转到下一行
void gotonextline(){
	pair<string,INT> pir;
	while(pir.first!="stop"){
		pir=get_pair();
	}
}
//判断statement是否正规 用来规避中间的statement没有;的恐慌
void endstatement(){
	pair<string,INT> pir;
	while(pir.first!=";"&&pir.first!="stop")
		pir=get_pair();
	if(pir.first==";"){
		withdraw();
	}
	else if(pir.first=="stop"){
		withdraw();
		withdraw();
	}
}
//# proc-> program id; <block>
void proc(){
	pair<string,INT> pir;
	pir=get_pair();
	//如果缺少Program
	if(pir.first=="program"){
		pir=get_pair();
		//如果缺少ID
		if(pir.second.type==10){
			pir=get_pair();
			if(pir.first==";"){
				ifendline();
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+"missing ;";
				ERR.push_back(ERROR);
				if(pir.first!="stop"){
					ifendline();
				}
			}
		}
		else{
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing program name ";
			ERR.push_back(ERROR);
			gotonextline();
		}
	}
	else{
		ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+ " missing program ";
		ERR.push_back(ERROR);
		gotonextline();//寻找下一行分�?
	}
	cout<<"now analysing block \n";
	block();
}

void judgeprocedure(){
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first=="const"||pir.first=="var"||pir.first=="begin"){
	withdraw();
	block();
	}
}
// block ->[condecl][vardecl][proc]<body>
void block(){
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first=="const"){
		cout<<"now analysing condecl \n";
		Const();
		pir=get_pair();
		while(pir.first==","){
			Const();
			pir=get_pair();
		}
		if(pir.first==";"){
			ifendline();
			pir=get_pair();
		}
		else{
			ERROR ="line "+to_string(pir.second.xp+1)+" missing ; or missing , ";
			ERR.push_back(ERROR);
			if(pir.first!="stop")
				gotonextline();
			pir=get_pair();
		}
	}
	if(pir.first=="var"){
		cout<<"now analysing vardecl \n";
		Var();
		pir=get_pair();
		while(pir.first==","){
			Var();
			pir=get_pair();
		}
		if(pir.first==";"){
			ifendline();
			pir=get_pair();
		}
		else{
			ERROR ="line "+to_string(pir.second.xp+1)+" missing ; or missing ,";
			ERR.push_back(ERROR);
			if(pir.first!="stop")
				gotonextline();
			pir=get_pair();
		}
	}
	if(pir.first=="procedure")
	{
		while(pir.first=="procedure"){
			cout<<"now analysing procedure \n";
			pir=get_pair();
			if(pir.second.type==10){
				proce.insert(pir.first);
				pir=get_pair();
					if(pir.first=="("){
						pir=get_pair();
						//有参�?
						if(pir.first!=")"){
							nums.insert(make_pair(pir.first,INT_MAX));
							pir=get_pair();
							while(pir.first==","){
								pir=get_pair();
								if(pir.first!=")"){
									nums.insert(make_pair(pir.first,INT_MAX));
									pir=get_pair();
								}
								else{
									ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" error identifier ";
									ERR.push_back(ERROR);
									//gotonextline();
									//judgeprocedure();
								}
							}
						}
						if(pir.first==")"){
							pir=get_pair();
							if(pir.first==";"){
								ifendline();
								block();
							}
							else{
								ERROR="line "+to_string(pir.second.xp)+" missing ; ";
								ERR.push_back(ERROR);
								gotonextline();
								judgeprocedure();
							}
						}
						else{
							ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ) or missing ,";
							ERR.push_back(ERROR);
							gotonextline();
							judgeprocedure();
						}
				}
				else{
					ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ( ";
					ERR.push_back(ERROR);
					gotonextline();
					judgeprocedure();
				}
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" can't use this as id ";
				ERR.push_back(ERROR);
				gotonextline();
				judgeprocedure();
			}
		}
	}
	else withdraw();	
	body();
}
void statementanalysis(){
	pair<string,INT> pir;
	statement();
	pir=get_pair();
	while(pir.first==";"){
		ifendline();
		statement();
		pir=get_pair();
	}
	withdraw();
}
void wholestatement(){
	pair<string,INT> pir;
	ifendline();
	statementanalysis();
	pir=get_pair();
	while(pir.first=="stop"){
		pir=get_pair();
		if(pir.first=="write"||pir.first=="read"||pir.first=="if"||pir.first=="while"||pir.second.type==10){
			withdraw();
			ERROR="line "+to_string(pir.second.xp)+" probably missing ; please check";
			ERR.push_back(ERROR);
			statementanalysis();
			pir=get_pair();
		}
	}
	if(pir.first=="end"){
		pir=get_pair();
		if(pir.first!="stop"){
			withdraw();
		}
	}	
	else{
		ERROR="line "+to_string(pir.second.xp)+" missing end or above statements may have some errors please check";
		ERR.push_back(ERROR);
		if(pir.first=="begin")withdraw();
		else gotonextline();
	}
}
void body(){
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first=="begin"){
		wholestatement();
	}
	else{
		ERROR="line "+to_string(pir.second.xp+1)+" likely missing begin nearby please check ";
		ERR.push_back(ERROR);
		gotonextline();
		pir=get_pair();
		if(pir.first!="null"){
			if(pir.first=="var"){
				withdraw();
				ERROR=to_string(pir.second.xp)+" likely missing const please check ";
				ERR.push_back(ERROR);
				block();
			}
			else if(pir.first=="procedure"){
				withdraw();
				ERROR=to_string(pir.second.xp)+" likely missing const or var please check ";
				ERR.push_back(ERROR);
				block();
			}
			else if(pir.first=="begin"){
				withdraw();
				ERROR=to_string(pir.second.xp)+" there is an error nearby please check ";
				ERR.push_back(ERROR);
				block();
			}
			else if(pir.first=="write"||pir.first=="read"||pir.first=="if"||pir.first=="while"||pir.second.type==10){
				withdraw();
				wholestatement();
			}
			else{
				cout<<"\nprogram quit for fatal errors\n";
			}
		}
	}
}
void statement(){
	//防止最后一个也加冒�?
	//cout<<"now analysing statement \n";
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first=="end"){
		ERROR="line "+to_string(pir.second.xp)+" has an error about ; nearby ";
		ERR.push_back(ERROR);
		withdraw();
		return;
	}
	if(pir.second.type==10){
		if(nums.count(pir.first)){
			pir=get_pair();
			exp();
		}
		else{
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" can't resolve this type "+pir.first;
			ERR.push_back(ERROR);
			pir=get_pair();
			while(pir.first!=";")pir=get_pair();
			withdraw();
		}
	}
	else if(pir.first=="if"){
		lexp();
		pir=get_pair();
		if(pir.first=="then"){
			statement();
			pir=get_pair();
			if(pir.first=="else"){
				statement();
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing keyword else ";
				ERR.push_back(ERROR);
				endstatement();
			}
		}
		else{
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing keyword then ";
			ERR.push_back(ERROR);
			endstatement();
		}
	}
	else if(pir.first=="while"){
		lexp();
		pir=get_pair();
		if(pir.first=="do"){
			gotonextline();
			statement();
		}
		else{
			ERROR="line "+to_string(pir.second.xp)+" missing keyword do ";
			ERR.push_back(ERROR);
			endstatement();
		}
	}
	else if(pir.first=="call"){
		pir=get_pair();
		if(pir.second.type!=10){
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" invalid procedure id ";
			ERR.push_back(ERROR); 
			endstatement();
		}
		else{
			if(proce.count(pir.first)){
				pir=get_pair();
				if(pir.first=="("){
					exp();
					pir=get_pair();
					while(pir.first==","){
						exp();
						pir=get_pair();
					}
					if(pir.first==")"){
						// pir=get_pair();
						// if(pir.first==";"){
						// 	//完成CALL
						// }
						// else{
						// 	ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+" missing ; ";
						// 	ERR.push_back(ERROR);
						// }
					}
					else{
						ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ) or missing , ";
						ERR.push_back(ERROR);
						endstatement();
					}
				}
				else{
					ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing exp or missing ( ";
					ERR.push_back(ERROR);
					endstatement();
				}
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing procedure name ";
				ERR.push_back(ERROR);
				endstatement();
			}
		}
	}
	else if(pir.first=="read"){
		pir=get_pair();
		if(pir.first=="("){
			pir=get_pair();
			if(pir.second.type==10){
				pir=get_pair();
				while(pir.first==","){
					pir=get_pair();
					if(pir.second.type==10){
						pir=get_pair();
					}
					else{
						ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" identifier error ";
						ERR.push_back(ERROR);
						endstatement();
					}
				}
				withdraw();
			}
			else if(pir.second.type==9){
				ERROR="line "+to_string(pir.second.xp+1)+" can't have const value ";
				ERR.push_back(ERROR);
				pir=get_pair();
				while(pir.first!=")")pir=get_pair();
				withdraw();
			}
			pir=get_pair();
			if(pir.first==")"){
				// pir=get_pair();
				// if(pir.first==";"){
				// 	//finish
				// }
				// else{
				// 	ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+" missing ; ";
				// 	ERR.push_back(ERROR);
				// }
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ) or missing ; ";
				ERR.push_back(ERROR);
				endstatement();
			}
		}
		else{
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ( ";
			ERR.push_back(ERROR);
			endstatement();
		}
	}
	else if(pir.first=="write"){
		pir=get_pair();
		if(pir.first=="("){
			exp();
			pir=get_pair();
			while(pir.first==","){
				exp();
				pir=get_pair();
			}
			if(pir.first==")"){
				// pir=get_pair();
				// if(pir.first==";"){
				// 	//finish
				// }
				// else{
				// 	ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+" missing ; ";
				// 	ERR.push_back(ERROR);
				// }
			}
			else{
				ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ) or missing , please check ";
				ERR.push_back(ERROR);
				endstatement();
			}
		}
		else{
			ERROR=to_string(pir.second.xp+1)+ " "+to_string(pir.second.yp)+" missing ( ";
			ERR.push_back(ERROR);
			endstatement();
		}
	}
	else if(pir.first=="begin"){
		withdraw();
		body();
		pir=get_pair();
		if(pir.first!=";")withdraw();
		withdraw();
	}
	else{
		ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+" this line is not a statement please check ";
		ERR.push_back(ERROR);
		endstatement();
	}
}
void exp(){
	pair<string,INT> pir;
	pir=get_pair();
	//如果有正�?
	if(pir.first=="+"||pir.first=="-"){
		//执行某些操作
		pir=get_pair();
	}
	else{
		withdraw();
	}
	if(pir.first==")"){
		ERROR="line "+to_string(pir.second.xp+1)+" has a extra , please check ";
		ERR.push_back(ERROR);
	}
	term();
	pir=get_pair();
	if(pir.second.type==1){
		term();
	}
	else{
		withdraw();
	}
}
void lexp(){
	//cout<<"now analysing lexp \n";
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.first=="odd"){
		pir=get_pair();
		exp();
	}
	else{
		withdraw();
	}
	exp();
	pir=get_pair();
	if(pir.second.type==2){
		exp();
	}
	else{
		ERROR="line "+to_string(pir.second.xp+1)+" missing lop ";
		ERR.push_back(ERROR);
		withdraw();
	}
}
void Const(){
	pair<string,INT> pir;
	string first;int second;
	pir=get_pair();
	if(pir.second.type==10){
		first=pir.first;
		pir=get_pair();
		if(pir.first==":="){
			pir=get_pair();
			if(pir.second.type==9){
				second=pir.second.type;
				nums.insert(make_pair(first,second));
			}
			else{
				ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+pir.first+" NAN error ";
				ERR.push_back(ERROR);
				endstatement();
			}
		}
		else{
			ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+pir.first+" Symbol error ";
			ERR.push_back(ERROR);
			endstatement();
		}
	}
	else{
		ERROR="line "+to_string(pir.second.xp+1)+" can't use "+pir.first+" as an identifier ";
		ERR.push_back(ERROR);
		endstatement();
	}
}
void Var(){
	pair<string,INT> pir;
	pir=get_pair();
	if(pir.second.type==10){
		nums.insert(make_pair(pir.first,INT_MAX));
	}
	else{
		ERROR="line "+to_string(pir.second.xp)+"  "+pir.first+"   "+" identifier error ";
		ERR.push_back(ERROR);
		endstatement();
	}
}
void term(){
	pair<string,INT> pir;
	factor();
	pir=get_pair();
	if(pir.first=="*"||pir.first=="/"){
		factor();
	}
	else{
		withdraw();
	}
}
void factor(){
	pair<string,INT> pir;
	pir=get_pair();
	if(nums.count(pir.first)){

	}
	else if(pir.second.type==9){

	}
	else if(pir.first=="("){
		pir=get_pair();
		exp();
		pir=get_pair();
		if(pir.first==")"){

		}
		else {
			//ERROR
			ERROR=to_string(pir.second.xp)+ " "+to_string(pir.second.yp)+pir.first+" missing ) ";
			ERR.push_back(ERROR);
		}
	}
	else if(pir.first==")"){
		withdraw();
	}
	else{
		ERROR="line "+to_string(pir.second.xp)+pir.first+" factor error ";
		ERR.push_back(ERROR);
	}
}
int main(){
	deal_code();
	proc();
	// for(auto& it:ANS){
	// cout<<it.first<< " "<<it.second.xp<<" "<<it.second.yp<<'\n';
	// }
	if(ERR.size()!=0)
	for(auto& it:ERR){
		cout<<it<<"\n";
	}
	else cout<<" successfully compile ";
}
