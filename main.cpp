#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <exception>
#include <map>
#include <queue>
#include<iomanip>
using namespace std;

typedef struct condition{
	int columnid; //表示按第几列来排序 
	int asc; //1表示升序，0表示降序 
}condition;

class StockItem{
	public:
		StockItem(){};
		void print();
		void print() const;
		void* getvalue(int i) const;
		const StockItem & operator = (const StockItem & item);
	private:
		int date;
		char symbol[20];
		double open;
		double high;
		double low;
		double close;
		double adjclose;
		int volume;
		void input(std::ifstream &in);
		void output(std::ofstream &out) const;
	friend std::ifstream & operator >> (std::ifstream &in, StockItem &s);
    friend std::ofstream & operator << (std::ofstream &out, const StockItem &s);
};

class StockArray{
	public:
		StockArray(int maxlen){
			this->maxlen = maxlen;
			sarray = new StockItem[maxlen];
		}
		StockItem* getptr(int index){
			if(index >= len){
				cout << "out of array length" << endl;
				exit(1);
			}
			return sarray + index;
		}
		StockItem& getitem(int index){
			if(index >= len){
				cout << "out of array length" << endl;
				exit(1);
			}
			return sarray[index];
		}
		void inputheader(string header){
			stringstream ss(header);
			string temp;
			for(int i = 0; i < 8; i++){
				getline(ss, temp, ',');
				columnname[i] = temp;
			}
		}
		void additem(ifstream& in){
			in >> sarray[len];
			if(*(int*)sarray[len].getvalue(0) != 0){
				len ++;
			}
		}
		void storeheader(ofstream& out){
			for(int i = 0; i < 8; i++){
				out << columnname[i] << ",";
			}
			out << endl;
		}
		void storeitem(ofstream& out, int index){
			if(index >= len){
				cout << "out of array length" << endl;
				exit(1);
			}
			out << sarray[index];
		}
		const int getlen(){
			return len;
		}
		void singlesort(string strcondition){ // 例如：1,1|0,1
			//构造比较条件
			list = vector<condition>(); //先清空 
			stringstream ss(strcondition);
			char comma;
			string temp;
			condition t;
			while(getline(ss,temp,'|')){
				stringstream tempss(temp);
				tempss >> t.columnid >> comma >> t.asc;
				list.push_back(t); 
			}
			for(int i = 0; i < len-1; i++){
				for(int j = 0; j < len-1-i; j++){
					if(compare(sarray[j], sarray[j+1])>0){
						StockItem item = sarray[j];
						sarray[j] = sarray[j+1];
						sarray[j+1] = item;
					}
				}
			}
		}
		
		void clear(){
			delete []sarray;
			sarray = new StockItem[maxlen];
			len = 0;
		}
		
		~StockArray(){
			delete []sarray;
		}
	private:
		int compare(StockItem &a, StockItem &b){
			int comp = 0;
			for(int i = 0; i < list.size(); i++){
				void* ap = a.getvalue(list[i].columnid);
				void* bp = b.getvalue(list[i].columnid);
				if(list[i].columnid==0 || list[i].columnid==7){
					int avalue = *(int*)ap;
					int bvalue = *(int*)bp;
					if(avalue > bvalue){
						comp = 1;
					}else if(avalue == bvalue){
						comp = 0;
					}else{
						comp = -1;
					}
				}else if(list[i].columnid==1){
					char avalue[20];
					char bvalue[20];
					strncpy(avalue, (const char*)ap, 20);
					strncpy(bvalue, (const char*)bp, 20);
					comp = strcmp(avalue, bvalue);
				}else{
					double avalue = *(double*)ap;
					double bvalue = *(double*)bp;
					if(avalue > bvalue){
						comp = 1;
					}else if(avalue == bvalue){
						comp = 0;
					}else{
						comp = -1;
					}
				}
				if(list[i].asc==0){
					comp *= -1;
				}
				if(comp != 0){
					break;
				};
			}
			return comp;
		} 
		string columnname[8];
		StockItem* sarray;
		int maxlen;
		int len = 0;
		vector<condition> list;
};

std::ifstream & operator >> (std::ifstream &in, StockItem &s)
{
    s.input(in);
    return in;
}

std::ofstream & operator << (std::ofstream &out, const StockItem &s)
{
    s.output(out);
    return out;
}

int convert_date_to_int(string sdate){
	stringstream ss(sdate);
	int year;
	int month;
	int day;
	char deli;
	ss >> year >> deli >> month >> deli >> day;
	return year*10000 + month * 100 + day; 
} 

string convert_int_to_date(int date){
	int day = date % 100;
	date = (date - day)/100;
	int month = date %100;
	date = (date - month)/100;
	stringstream ss;
	ss << date << "/";
	ss << setw(2) << setfill('0') << month << "/";
	ss << setw(2) << setfill('0') << day;
	return ss.str();
}

template<class out_type>
out_type convert(string t)
{
	stringstream stream;
	stream<<t;
	out_type result;
	stream>>result;
	return result;
}

void StockItem::input(std::ifstream &in){
	//string datestring;
	string oneline;
	in >> oneline;
	std::stringstream strm(oneline);
	string token[8];
	for(int i = 0; i < 8; i++){ //把流按分隔符实现分割 
		getline(strm,token[i],',');
	}
	if(token[0] != ""){
		this->date = convert_date_to_int(token[0]); //date 
		strcpy(this->symbol, token[1].c_str()); //symbol
		this->open = convert<double>(token[2]); //open
		this->high = convert<double>(token[3]); //high
		this->low = convert<double>(token[4]); //low 
		this->close = convert<double>(token[5]); //close
		this->adjclose = convert<double>(token[6]); //adjclose
		this->volume = convert<int>(token[7]); //volume
	}else{
		this->date = 0;
	}
}

void StockItem::output(std::ofstream &out) const{
	out << fixed << convert_int_to_date(date) << "," << symbol << "," << open << "," << high << "," << low << "," << close << "," << adjclose << "," << volume << endl;
}

void* StockItem::getvalue(int i) const
{
	switch(i){
		case 0:
			return (void*)&date;
			break;
		case 1:
			return (void*)symbol;
			break;
		case 2:
			return (void*)&open;
			break;
		case 3:
			return (void*)&high;
			break;
		case 4:
			return (void*)&low;
			break;
		case 5:
			return (void*)&close;
			break;
		case 6:
			return (void*)&adjclose;
			break;
		case 7:
			return (void*)&volume;
			break;
	}
}

const StockItem & StockItem::operator = (const StockItem & item){
	this->date = item.date;
	strcpy(this->symbol, item.symbol); //symbol
	this->open = item.open; //open
	this->high = item.high; //high
	this->low = item.low; //low 
	this->close = item.close; //close
	this->adjclose = item.adjclose; //adjclose
	this->volume = item.volume; //volume
}

void StockItem::print() const{
	cout << fixed << convert_int_to_date(date) << "," << symbol << "," << open << "," << high << "," << low << "," << close << "," << adjclose << "," << volume << endl;
}

void StockItem::print(){
	cout << fixed << convert_int_to_date(date) << "," << symbol << "," << open << "," << high << "," << low << "," << close << "," << adjclose << "," << volume << endl;
}

struct item_in_queue
{
	StockItem item;
	string filename;
	vector<condition> list;
	item_in_queue(StockItem it, string filen, string strcondition){
		item = it;
		filename = filen;
		list = vector<condition>(); //先清空 
		stringstream ss(strcondition);
		char comma;
		string temp;
		condition t;
		while(getline(ss,temp,'|')){
			stringstream tempss(temp);
			tempss >> t.columnid >> comma >> t.asc;
			list.push_back(t); 
		}
	}
	bool operator < (const item_in_queue &bitem) const
	{
		const StockItem &a = item;
		const StockItem &b = bitem.item;
		int comp = 0;
		for(int i = 0; i < list.size(); i++){
			void* ap = a.getvalue(list[i].columnid);
			void* bp = b.getvalue(list[i].columnid);
			if(list[i].columnid==0 || list[i].columnid==7){
				int avalue = *(int*)ap;
				int bvalue = *(int*)bp;
				if(avalue > bvalue){
					comp = 1;
				}else if(avalue == bvalue){
					comp = 0;
				}else{
					comp = -1;
				}
			}else if(list[i].columnid==1){
				char avalue[20];
				char bvalue[20];
				strncpy(avalue, (const char*)ap, 20);
				strncpy(bvalue, (const char*)bp, 20);
				comp = strcmp(avalue, bvalue);
			}else{
				double avalue = *(double*)ap;
				double bvalue = *(double*)bp;
				if(avalue > bvalue){
					comp = 1;
				}else if(avalue == bvalue){
					comp = 0;
				}else{
					comp = -1;
				}
			}
			if(list[i].asc==0){
				comp *= -1;
			}
			if(comp != 0){
				break;
			};
		}
		return comp>0; //小顶堆 
	}
};

class SortManager{
	public:
		SortManager(string path, string output, int size=400, int way=4){
			this->path = path;
			this->output = output;
			this->size = size;
			this->way = way;
			partition();
			for(int i = low; i < high; i++){
				filelist.push(i);
			}
		}
		void merge_sort();
		int gethigh(){
			return high;
		}
	private:
		//void merge(string filenamelist, int deep);
		void partition();
		void merge(vector<int> filenamelist);
		queue<int> filelist;
		string path;
		string output;
		string columnname;
		int high;
		int low;
		int way;
		int size;
};

void SortManager::partition(){
	std::ifstream fin(this->path.c_str());
	int filenum = 0;
	this->low = filenum;
	if(!fin) 
	{
		cout<<"open fail."<<endl;
		exit(1);
	}
	else{
		bool firstline = true;
		//cout<<"before sorting"<<endl;
		StockArray array(this->size);
		string header;
		while(!fin.eof()){
			if(firstline){
				fin >> header;
				this->columnname = header;
				array.inputheader(header);
				firstline = false;
				continue; //the first line is the column name, we can jump it! 
			}
			if(array.getlen()>=this->size){
				array.singlesort("1,1|0,1");
				stringstream sstemp;
				sstemp << filenum << ".csv";
				std::ofstream fout(sstemp.str().c_str());
				//array.storeheader(fout);
				for(int i = 0; i < this->size; i++){
					array.storeitem(fout, i);
				}
				fout.close();
				filenum ++;
				array.clear();
			}else{
				array.additem(fin);
				cout << filenum << " " << array.getlen()-1 << endl;
			}
			
			//array.getptr(array.getlen()-1)->print();
		}
	}
	fin.close();
	this->high = filenum;
} 

void SortManager::merge(vector<int> filenamelist){
	map<string, int> file2int;
	ifstream inarr[way];
	priority_queue<item_in_queue> itemagg;
	string filename;
	int filenum = 0;
	for(int i = 0; i < filenamelist.size(); i++){
		filename = to_string(filenamelist[i])+".csv";
		inarr[filenum].open(filename.c_str());
		inarr[filenum].seekg(0, ios::beg);
		//cout << &inarr[filenum] << endl;
		StockItem s;
		inarr[filenum] >> s;
		//s.print();
		itemagg.push(item_in_queue(s, filename, "1,1|0,1"));
		//mergeagg[filename] = inarr[filenum].tellg();
		file2int[filename] = filenum;
		//cout << filename << " tellg:" << inarr[filenum].tellg() << endl;
		//in.close();
		filenum ++;
	}
	//cout<<"hhh"<<endl;
	//int k = mergeagg.size(); 
	ofstream fout(std::to_string(this->high)+".csv");
	cout << std::to_string(this->high)+".csv" << ends;
	int i = 0;
	while(!itemagg.empty()){
		//cout << i << endl;
		fout << itemagg.top().item;
		i++;
		string filename = itemagg.top().filename;
		itemagg.pop();
		if(file2int.find(filename)==file2int.end()){
			//cout << "1" <<endl; 
			continue;
		}
		ifstream& in = inarr[file2int[filename]];
		//in.seekg(mergeagg[filename]-5, ios::beg);
		//cout << filename << " seekg:" << in.tellg() << endl;
		if(in.eof()){
			in.close();
			//mergeagg.erase(filename);
			file2int.erase(filename);
			//cout << "2" <<endl; 
			continue;
		}
		StockItem s;
		in >> s;
		//s.print();
		if(*(int*)s.getvalue(0) == 0){
			//cout << "3" <<endl; 
			continue;
		}
		itemagg.push(item_in_queue(s, filename, "1,1|0,1"));
		//mergeagg[filename] = in.tellg();
		//cout << in.tellg() << endl;
		//in.close();
		//i++;
	}
	fout.close();
	filelist.push(high);
	cout << i << endl;
	high++;
} 

void SortManager::merge_sort(){
	while(filelist.size() > 1){ //如果最后只有一个总的文件放进来了，那么就退出循环 
		vector<int> v;
		for(int i = 0; i < way; i++){
			v.push_back(filelist.front());
			cout << filelist.front() << endl;
			if(!filelist.empty()){
				filelist.pop();
			}
		}
		merge(v); //归并几个小文件，并把新产生的文件加入列表中 
	}
	for(int i = low; i < high-1; i++){
		string file = to_string(i)+".csv";
		remove(file.c_str());
	}
	string oldname = to_string(high-1) + ".csv";
	ofstream out("output.csv");
	out << this->columnname << endl;
	ifstream in(oldname.c_str());
	while(!in.eof()){
		string temp;
		in >> temp;
		out << temp << endl;
	}
	out.close();
	in.close();
	remove(oldname.c_str());
	cout<<"complete sort, the output file is " << this->output << endl;
}

class IndexNode{
	public:
		IndexNode(){};
		IndexNode(string symbol, int yearmonth, int offset);
		void print();
		void input(std::ifstream &in);
		void output(std::ofstream &out) const;
		pair<int, string> get_pair() const {
			return make_pair(yearmonth, symbol);
		}
		int get_offset() const {
			return offset;
		}
	private:
		string symbol;
		int yearmonth;
		int offset;
	friend std::ifstream & operator >> (std::ifstream &in, IndexNode &i);
	friend std::ofstream & operator << (std::ofstream &out, const IndexNode &i);
};

IndexNode::IndexNode(string symbol, int yearmonth, int offset){
	this->symbol = symbol;
	this->yearmonth = yearmonth;
	this->offset = offset;
} 

void IndexNode::print(){
	cout << yearmonth << "," << this->symbol << "," << this->offset << endl;
}

void IndexNode::input(std::ifstream &in){
	string oneline;
	in >> oneline;
	std::stringstream strm(oneline);
	string token[3];
	for(int i = 0; i < 3; i++){ //把流按分隔符实现分割 
		getline(strm,token[i],',');
	}
	if(token[0] != ""){
		this->yearmonth = convert<int>(token[0]); //yearmonth 
		this->symbol = token[1]; //symbol
		this->offset = convert<int>(token[2]); //offset
	}else{
		this->yearmonth = 0;
	} 
}

void IndexNode::output(std::ofstream &out) const{
	out << yearmonth << "," << this->symbol << "," << this->offset << endl;
}

class IndexArray{
	public:
		IndexArray(string datafile, string filename){
			this->datafile = datafile;
			this->indexfile = filename;
		};
		void buildIndex();
		void loadindex();
		int find(string symbol, int yearmonth);
	private:
		map<pair<int, string>, int> array;
		string datafile;
		string indexfile;
};

void IndexArray::buildIndex(){
	
	ofstream build(this->indexfile.c_str());
	ifstream fin(this->datafile.c_str());
	if(!fin) 
	{
		cout<<"open fail."<<endl;
		exit(1);
	}
	else{
		bool firstline = true;
		string header;
		int last_yearmonth = 0;
		string last_symbol = "";
		while(!fin.eof()){
			if(firstline){
				fin >> header;
				firstline = false;
				continue; //the first line is the column name, we can jump it! 
			}
			StockItem s;
			int offset = fin.tellg();
			fin >> s;
			int now_yearmonth = (*(int*)s.getvalue(0))/100;
			string now_symbol = string((char*)s.getvalue(1));
			if(now_yearmonth != last_yearmonth || now_symbol != last_symbol){
				if(now_yearmonth > 0){
					IndexNode inn(now_symbol, now_yearmonth, offset);
					inn.print();
					build << inn;
				}else{
					break;
				}
			}
			last_yearmonth = now_yearmonth;
			last_symbol = now_symbol;
		}
	}
	fin.close();
	build.close();
}

void IndexArray::loadindex(){
	ifstream fin(this->indexfile.c_str());
	if(!fin) 
	{
		cout<<"open fail."<<endl;
		exit(1);
	}else{
		int count = 0;
		while(!fin.eof()){
			IndexNode i;
			fin >> i;
			array[i.get_pair()] = i.get_offset();
		}
	}
}

int IndexArray::find(string symbol, int yearmonth){
	pair<int, string> t = make_pair(yearmonth, symbol);
	return array[t];
}

std::ifstream & operator >> (std::ifstream &in, IndexNode &i)
{
    i.input(in);
    return in;
}

std::ofstream & operator << (std::ofstream &out, const IndexNode &i)
{
    i.output(out);
    return out;
}

int main(){
	//SortManager m("../all_us_stock_daily_small.csv", "output.csv", 400, 4);
	//m.merge_sort();
	IndexArray iarr("output.csv", "index.csv");
	//iarr.buildIndex();
	iarr.loadindex();
	int offset1 = iarr.find("AXLE", 199703); 
	int offset2 = iarr.find("AXLE", 199704); 
	ifstream fin("output.csv");
	fin.seekg(offset1-10);
	string temp;
	fin >> temp;
	//StockItem s;
	//fin >> s;
	//s.print();
	while(fin.tellg() < offset2-5){
		StockItem s;
		fin >> s;
		s.print();
	}
}
