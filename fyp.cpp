// Hammad Nadeem 150162
#include <iostream>
#include <stdlib.h>     
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;
string line1,line2;
int getevfromstr(string inpt){
	if(inpt.find("RR") != std::string::npos){//rr
		return 1;
	}
	if(inpt.find("LFU") != std::string::npos){//lfu
		return 3;
	}
	if(inpt.find("LRU") != std::string::npos){//lru
		return 2;
	}
	if(inpt.find("FIFO") != std::string::npos){//fifo
		return 4;
	}
	if(inpt.find("LIFO") != std::string::npos){//lifo
		return 5;
	}
	if(inpt.find("MRU") != std::string::npos){//mru
		return 6;
	}
	cout<<"invalid input evpfstr\n";
	return 1;
}
void hitwritertofile(){
	fstream wrt;
	wrt.open("hitfile.txt",ios::out);
	wrt<<line1<<"\n";
	wrt<<line2<<"\n";
	wrt.flush();
	wrt.close();
}
unsigned long int curtime=0;// time used for time based eviction policies
class cacheblock{
	private:
	string *memblock;// the fullfat storage -cache lines in our case stores mem addresses
	int blocksize;// number of data lines
	int ep;// eviction policy needed to handle various tasks
	unsigned int *freqcount;// used to store frequency of lfu or time stamp of lru
	int indexf; // used by fifo or lifo to manage data
	public:
	cacheblock(int size,int eviction){ // setup of variables e -> 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out
		memblock=nullptr;
		freqcount=nullptr;
		blocksize=size;
		ep=eviction;
		if((ep==2)||(ep==3)){// need more storage for frequencies or time stamps
			freqcount=new unsigned int[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=0;			
			}
		}
		else if(ep==6){// need more storage for frequencies or time stamps
			freqcount=new unsigned int[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=9999;	// max value so cache can fill initially		
			}
		}
		else{
			freqcount=nullptr;
		}
		indexf=0;
		memblock=new string[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]="0";			
		}
	}
	cacheblock(){
		memblock=nullptr;
		freqcount=nullptr;
		blocksize=0;
		ep=0;
		indexf=0;
	}
	cacheblock(const cacheblock &old){
		this->memblock=nullptr;
		this->freqcount=nullptr;
		this->blocksize=old.blocksize;
		this->ep=old.ep;
		this->indexf=old.indexf;
		if((ep==2)||(ep==3)||(ep=6)){// need more storage for frequencies or time stamps
			freqcount=new unsigned int[blocksize];
			for(int i=0; i<blocksize;i++){
				this->freqcount[i]=old.freqcount[i];			
			}
		}
		memblock=new string[blocksize];
		for(int i=0; i<blocksize;i++){
			this->memblock[i]=old.memblock[i];			
		}
	}
	cacheblock& operator=(const cacheblock &rhs){
		if(this!=&rhs){
			//if(memblock!=nullptr)
				//delete[] this->memblock;
			//if(freqcount!=nullptr)
				//delete[] this->freqcount;// add checks here to prevent segfaults
			this->memblock=nullptr;
			this->freqcount=nullptr;
			this->blocksize=rhs.blocksize;
			this->ep=rhs.ep;
			this->indexf=rhs.indexf;
			if((ep==2)||(ep==3)||(ep=6)){// need more storage for frequencies or time stamps
				freqcount=new unsigned int[blocksize];
				for(int i=0; i<blocksize;i++){
					this->freqcount[i]=rhs.freqcount[i];			
				}
			}
			memblock=new string[blocksize];
			for(int i=0; i<blocksize;i++){
				this->memblock[i]=rhs.memblock[i];			
			}
		}
		return *this;
	}
	void editparameters(int size=15,int eviction=1){
		//if(memblock!=nullptr)
			//delete[] this->memblock;
		//if(freqcount!=nullptr)
			//delete[] this->freqcount;// add checks here to prevent segfaultsmemblock=nullptr;
		this->freqcount=nullptr;
		this->memblock=nullptr;
		blocksize=size;
		ep=eviction;
		if((ep==2)||(ep==3)){// need more storage for frequencies or time stamps
			freqcount=new unsigned int[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=0;			
			}
		}
		else if(ep==6){// need more storage for frequencies or time stamps
			freqcount=new unsigned int[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=9999;	// max value so cache can fill initially		
			}
		}
		else{
			freqcount=nullptr;
		}
		indexf=0;
		memblock=new string[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]="0";			
		}
		//cout<<"Cache configuration updated \n";
	}
	void print(){// print content of block
		if(ep==4){
			cout<<"index is "<<indexf<<endl;
		}
		else if ((ep==2)||(ep==6)){
			cout<<"current time is "<<curtime<<endl;
		}
		for(int i=0;i<blocksize;i++){
			cout<<"Line "<<i<<": "<<memblock[i];
			if(ep==2){
				cout<<" time stamp "<<freqcount[i];
			}
			else if(ep==3){
				cout<<" frequency of use "<<freqcount[i];
			}
			cout<<endl;
		}
	}
	void writec2f(fstream &obj){// write content to file
		string temp;
		for(int i=0;i<blocksize;i++){
			temp="Line ";
			temp+=to_string(i);
			temp+=": ";
			temp+=memblock[i];
			temp+="\n";
			obj<<temp;
		}
	}
	bool isHit(string memaddress){// check to see if data at memory is present in cache block
		if(!(memaddress.empty())){
			for(int i=0; i<blocksize;i++){
				if((memblock[i].compare(memaddress))==0){
					if(ep==3){
						freqcount[i]=freqcount[i]+1;// update useage count
					}
					if((ep==2)||(ep==6)){
						freqcount[i]=curtime;
					}
					return true;
				}
			}
			return false;
		}
		else {
			cout<<"invalid memory address\n";
			return false;
		}
	}
	void fetchFromMem(string memaddress,map<string,int> *ptable){// instructs to place mem in cache and update pollution list
		string memoryremoved;
		map<string,int>::iterator it;
		switch(ep){
			case 1:{ // random replacement 
				int p=rand()%blocksize;
				memoryremoved=memblock[p];
				memblock[p]=memaddress;	
				break;
			}
			case 2:{//least recently used 
				unsigned int minval=freqcount[0],minindex=0;
				for(int i=0; i<blocksize;i++){
					if(minval>freqcount[i]){
						minindex=i;
						minval=freqcount[i];
					}
				}
				memoryremoved=memblock[minindex];
				memblock[minindex]=memaddress;
				freqcount[minindex]=curtime;			
				break;
			}
			case 3:{//least frequently used
				unsigned int minval=-1,minindex=0;
				for(int i=0; i<blocksize;i++){
					if(minval>freqcount[i]){
						minindex=i;
						minval=freqcount[i];
					}
				}
				memoryremoved=memblock[minindex];
				memblock[minindex]=memaddress;
				freqcount[minindex]=1;		
				break;
			}
			case 4:{//first in first out
				memoryremoved=memblock[indexf];
				memblock[indexf]=memaddress;
				indexf++;
				if(indexf>=blocksize)
					indexf=0;// wrap it around !
				break;
			}
			case 5:{//last in first out like a stack
				memoryremoved=memblock[indexf];
				memblock[indexf]=memaddress;
				indexf++;
				if(indexf>=blocksize)
					indexf=blocksize-1;
				break;
			}
			case 6:{//most recently used 
				unsigned int maxval=freqcount[0],maxindex=0;
				for(int i=0; i<blocksize;i++){
					if(maxval<freqcount[i]){
						maxindex=i;
						maxval=freqcount[i];
					}
				}
				memoryremoved=memblock[maxindex];
				memblock[maxindex]=memaddress;
				freqcount[maxindex]=curtime;			
				break;
			}
			default:
				cout<<"invalid ep value\n";					
		}
		if(stoll(memoryremoved)>10){
			(*ptable)[memoryremoved]++;// simple solution !
		}
	}
	~cacheblock(){
		if(this->memblock!=nullptr){
			//delete[] this->memblock;// sort segfaults 
			this->memblock=nullptr;
		}
		if(freqcount!=nullptr){
			//delete[] this->freqcount;
			this->freqcount=nullptr;
		}
	}
};
class cache{// need to turn this into abstract class
	private:
	cacheblock *ptr;// the cache blocks
	int nblocks; // len of ptr in otherwords number of sets
	int type;// 1 is direct mapped ,2 is n-set associative, 3 is fully set associative
	int epolicy;// 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out  5 lifo 6 mru
	int lines;// used to tell how many lines per block/set of cache
	map<string,int> pollution;
	unsigned int hits; // the number of hits achieved so far
	unsigned long long executiontime;// theoratical estimate of time considering averages of fetching from mem or fetching from cache 
	public:
	cache(int sets, int ctype,int evictionpolicy,int linesperset){
		ptr=nullptr;
		nblocks=sets;
		type=ctype;
		hits=0;
		epolicy=evictionpolicy;
		lines=linesperset;
		executiontime=0;
		if((type==3)||(nblocks==1)){
			ptr=new cacheblock(lines,epolicy);
		}
		else{
			ptr=new  cacheblock[nblocks]();
			for(int i=0;i<nblocks;i++){// edit parameters here or use std vector !!!
				ptr[i].editparameters(lines,epolicy);
			}
		}	
	}
	cache(){
		ptr=nullptr;
		nblocks=0;
		type=0;
		hits=0;
		epolicy=0;
		lines=0;
		executiontime=0;
	}
	cache(const cache &old){
		ptr=nullptr;
		this->nblocks=old.nblocks;
		this->type=old.type;
		this->hits=old.hits;
		this->epolicy=old.epolicy;
		this->lines=old.lines;
		this->executiontime=old.executiontime;
		if(nblocks<=1){
			ptr=new cacheblock(lines,epolicy);
			*ptr=*(old.ptr);
		}
		else{
			ptr=new  cacheblock[nblocks]();
			for(int i=0;i<nblocks;i++){// edit parameters here or use std vector !!!
				this->ptr[i]=old.ptr[i];
			}
		}
	}
	int getnumofsets(){
		return nblocks;
	}
	int getlines(){
		return lines;
	}
	string getevp(){
		//int epolicy;// 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out  5 lifo 6 mru
		if (type==1)
			return "Modulo";
		if (epolicy==1)
			return "RR";
		else if (epolicy==2)
			return "LRU";
		else if (epolicy==3)
			return "LFU";
		else if (epolicy==4)
			return "FIFO";
		else if (epolicy==5)
			return "LIFO";
		else
			return "MRU";
	}
	cache& operator=(const cache &rhs){
		if(this!=&rhs){
			//if(ptr!=nullptr)
				//delete [] this->ptr;
			this->ptr=nullptr;
			this->nblocks=rhs.nblocks;
			this->type=rhs.type;
			this->hits=rhs.hits;
			this->epolicy=rhs.epolicy;
			this->lines=rhs.lines;
			this->executiontime=rhs.executiontime;
			if(nblocks<=1){
				ptr=new cacheblock(lines,epolicy);
				*ptr=*(rhs.ptr);
			}
			else{
				ptr=new  cacheblock[nblocks]();
				for(int i=0;i<nblocks;i++){// edit parameters here or use std vector !!!
					this->ptr[i]=(rhs.ptr[i]);
				}
			}
		}
		return *this;
	}
	string getctype(){// 1 is direct mapped ,2 is n-set associative, 3 is fully set associative
		if(type==1)
			return "Direct Mapped";
		else if(type==2)
			return "N-set Associative";
		else 
			return "Fully Associative";
	}
	void editcacheconfig(int sets=1, int ctype=1,int evictionpolicy=1,int linesperset=8){
		//if(ptr!=nullptr)
			//delete [] this->ptr;
		this->ptr=nullptr;
		nblocks=sets;
		type=ctype;
		hits=0;
		epolicy=evictionpolicy;
		lines=linesperset;
		executiontime=0;
		if((type==3)||(nblocks==1)){
			ptr=new cacheblock(lines,epolicy);
		}
		else{
			ptr=new  cacheblock[nblocks]();
			for(int i=0;i<nblocks;i++){// edit parameters here or use std vector !!!
				ptr[i].editparameters(lines,epolicy);
			}
		}	
	}
	unsigned int getcurrenthit(){
		return hits;
	}
	bool isHit(string memaddr){
		for(int i=0;i<nblocks;i++){
			if(ptr[i].isHit(memaddr)){
				hits++;
				executiontime+=rand()%4 + 5;// average of 7ns to fecth from l2 cache between 5 to 8 ns
				return true;		
			}	
		}
		executiontime+=rand()%11 + 90;// takes 100ns to fecth from main memory so 90-100ns
		return false;
	}
	bool isAlreadyinCache(string memaddr){// different from above function to prevent duplicate entries
		for(int i=0;i<nblocks;i++){
			if(ptr[i].isHit(memaddr)){
				return true;		
			}	
		}
		return false;
	}
	unsigned long long getcurEtime(){
		return executiontime;
	}
	void display(){// just to see whats going on 
		for(int i=0;i<nblocks;i++){
			cout<<"Block "<<i<<":\n";
			ptr[i].print();
		}		
	}
	void print2file(fstream &obj){// to link with the interface we store data on files
		string temp;
		for(int i=0;i<nblocks;i++){
			temp="";
			temp+="Block ";
			temp+=to_string(i);
			temp+=":\n";
			obj<<temp;
			ptr[i].writec2f(obj);
		}
	}
	void updatecache(string memaddress){// where does the pretecher come in ? needs work
		unsigned long long setnum=stoll(memaddress,0,16);
		setnum=setnum%nblocks;
		switch(type){
			case 1:{ // direct mapped
				ptr[setnum].fetchFromMem(memaddress,&pollution);
				break;
			}
			case 2:{ // nset associative
				ptr[setnum].fetchFromMem(memaddress,&pollution);
				break;
			}
			case 3:{// fully associative
				ptr->fetchFromMem(memaddress,&pollution);
				break;
			}
			default:
				cout<<"invalid value for type of cache\n";			
		}
	}
	vector<std::pair<std::string,int>> getTopNpol(int n){// top n cache polluting items in order of the times taken in and out of the cache
		std::vector<std::pair<std::string, int>> top_n(n);
		std::partial_sort_copy(pollution.begin(),pollution.end(),top_n.begin(),top_n.end(),[](std::pair<const std::string, int> const& l,std::pair<const std::string, int> const& r){
			return l.second > r.second;
		});
		return top_n;
	}
	~cache(){
		if(ptr!=nullptr){
			//delete [] this->ptr;
			this->ptr=nullptr;
		}
	}
};
class directmapped: public cache{
	private:
	cache *ptr;
	public:
	directmapped(int numsets=8){
		ptr=new cache(numsets,1,1,1);
	}
	void editcacheconfig(int numsets=8,int evictionpolicy=1){
		if(ptr!=nullptr)
			//delete this->ptr;
		ptr=new cache(numsets,1,evictionpolicy,1);
	}
	bool ishit(string memaddr){
		return ptr->isHit(memaddr);
	}
	bool isitincache(string memaddr){
		return ptr->isAlreadyinCache(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void print2file(fstream &obj){
		ptr->print2file(obj);
	}
	unsigned long long getexectime(){
		return ptr->getcurEtime();
	}
	void updatecache(string memaddr){
		ptr->updatecache(memaddr);
	}	
	~directmapped(){
		if(ptr!=nullptr){
			//delete this->ptr;
			this->ptr=nullptr;
		}
	}	
};
class fullyassociative: public cache{
	private:
	cache *ptr;
	public:
	fullyassociative(int numoflines=8,int evictionpolicy=1){
		ptr=new cache(1,3,evictionpolicy,numoflines);
	}
	void editcacheconfig(int numoflines=8,int evictionpolicy=1){
		if(ptr!=nullptr)
			//delete this->ptr;
		ptr=new cache(1,3,evictionpolicy,numoflines);
	}
	bool ishit(string memaddr){
		return ptr->isHit(memaddr);
	}
	bool isitincache(string memaddr){
		return ptr->isAlreadyinCache(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void print2file(fstream &obj){
		ptr->print2file(obj);
	}
	unsigned long long getexectime(){
		return ptr->getcurEtime();
	}
	void updatecache(string memaddr){
		ptr->updatecache(memaddr);
	}	
	~fullyassociative(){
		if(ptr!=nullptr){
			//delete this->ptr;
			ptr=nullptr;
		}
	}	
};
class nsetassociative: public cache{
	private:
	cache *ptr;
	public:
	nsetassociative(int numoflines=8,int evictionpolicy=1,int numofsets=4){
		ptr=new cache(numofsets,2,evictionpolicy,numoflines);
	}
	void editcacheconfig(int numoflines=8,int evictionpolicy=1,int numofsets=4){
		if(ptr!=nullptr)
			//delete this->ptr;
		ptr=new cache(numofsets,2,evictionpolicy,numoflines);
	}
	bool ishit(string memaddr){
		return ptr->isHit(memaddr);
	}
	bool isitincache(string memaddr){
		return ptr->isAlreadyinCache(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void print2file(fstream &obj){
		ptr->print2file(obj);
	}
	unsigned long long getexectime(){
		return ptr->getcurEtime();
	}
	void updatecache(string memaddr){
		ptr->updatecache(memaddr);
	}	
	~nsetassociative(){
		if(ptr!=nullptr){
			//delete this->ptr;
			this->ptr=nullptr;
		}
	}	
};
class prefetcher{
	public:
	virtual void predictn(string,string*)=0;
	virtual int getsize()=0;
};
class streambuffer: public prefetcher{
	private:
	int len;// specifies the number of next best possible addresses which could be asked for by cpu
	public:
	streambuffer(int lenght=4){
		len=lenght;
	}
	void predictn(string address, string* result){// need to add addition logic and conversion here
		unsigned long long setnum=stoll(address,0,16),temp;
		for(int i=0;i<len;i++){
			temp=setnum+1+i;
			stringstream sstream;
			sstream<<std::hex<<temp;
			result[i]=sstream.str();
		}
	}
	int getsize(){
		return len;
	}	
};
class simulator{
	private:
	cache **ptr;//  array of caches
	bool setup;// all caches under this simulation ready for use ?
	int numofcaches;// number of caches
	string predictres[4];
	prefetcher* sbpf;
	fstream fileout;
	public:
	simulator(int numberofcaches){
		ptr=new cache * [numberofcaches];
		numofcaches=numberofcaches;
		sbpf=new streambuffer(4);
		fileout.open("outputdump",ios::out);
	}
	bool setcache(int index, int type, int sets,int lines, int eviction){
		bool op=false;
		if((index<numofcaches)&&(index>=0)){
			switch(type){
				case 1:// direct mapped
					ptr[index]=new directmapped(sets);
					ptr[index]->editcacheconfig(sets,1,1,1);//need to fix polymorphism
					op=true;
				break;
				case 2:// nset associative
					ptr[index]=new nsetassociative(lines,eviction,sets);
					ptr[index]->editcacheconfig(sets,2,eviction,lines);
					op=true;
				break;
				case 3:// fully associative 
					ptr[index]=new fullyassociative(lines,eviction);
					ptr[index]->editcacheconfig(1,3,eviction,lines);
					op=true;
				break;
				default:
					cout<<"invalid cache type specified\n";		
			}
		}
		else{
			cout<<"invalid index\n";
		}
		return op;		
	}
	void emulatebehaviour(string memaddr){// the concept of pre-fecthing should be added here--- We cant ask the cache to fit every thing from ram in it !!!
		curtime++; // increament time value
		string temp="Requested mem: ";
		temp+=memaddr;
		temp+="\n";
		fileout<<temp;
		for(int i=0;i<numofcaches;i++){
			if(!(ptr[i]->isHit(memaddr))){// if not hit
				sbpf->predictn(memaddr,predictres);// get sequence of address to fetch
				for(int j=0;j<sbpf->getsize();j++){// prefetch those sequences
					if(!(ptr[i]->isAlreadyinCache(predictres[j])))// as long as its not already in the cache ( dont want duplicates)
						ptr[i]->updatecache(predictres[j]);	
				}
			}
		}
	}
	void display(){
		for(int i=0;i<numofcaches;i++){
			cout<<"cache "<<i+1<<endl;
			ptr[i]->display();
			cout<<"number of hits "<<ptr[i]->getcurrenthit()<<endl<<endl;		
		}
	}
	void write2file(){// need to replace with file writers
		string temp;
		for(int i=0;i<numofcaches;i++){
			temp="";
			temp+="Cache ";
			temp+=to_string(i+1);
			temp+="\n";
			fileout<<temp;
			ptr[i]->print2file(fileout);
			temp="number of hits ";
			temp+=to_string(ptr[i]->getcurrenthit());
			temp+="\n\n";	
			fileout<<temp;
		}
		fileout<<"-----------------------------------------------------\n";
	}
	void writepollution2file(){
		string temp1="";
		string temp2="";
		fstream wtr;
		wtr.open("pollutionlog.txt",ios::out);
		for(int i=0;i<numofcaches;i++){
			vector<std::pair<std::string,int>> ls=ptr[i]->getTopNpol(10);
			temp1="";
			temp2="";
			for( vector<std::pair<std::string,int>>::iterator it = ls.begin(); it != ls.end(); ++it ){
				string p_a = it->first;
				int p_b = it->second;
				temp1+=p_a;
				temp1+=",";
				temp2+=to_string(p_b);
				temp2+=",";
				}
			temp1+="\n";
			temp2+="\n";
			wtr<<temp1;
			wtr<<temp2;
		}
		wtr.flush();
		wtr.close();
	}
	void writepercentages2file(int iterations){
		string temp="";
		fstream wtr;
		wtr.open("hitrates.txt",ios::out);
		for(int i=0;i<numofcaches;i++){
			float pr=float(ptr[i]->getcurrenthit());
			pr=pr/float(iterations);
			pr=pr*100;
			temp+=to_string(pr);
			temp+=",";
		}
		wtr<<temp;
		wtr.flush();
		wtr.close();
	}
	void writehits2file(){// need to replace with file writers
		line1+=to_string(ptr[0]->getcurrenthit());
		line1+=",";
		line2+=to_string(ptr[1]->getcurrenthit());
		line2+=",";
	}
	void writestate2file(fstream & wrtr,int iterations){
		string temp;
		for(int i=0;i<numofcaches;i++){
			temp="";
			temp+="Cache ";
			temp+=to_string(i+1);
			temp+="\n";
			wrtr<<temp;
			temp="";
			temp.assign(ptr[i]->getctype());
			temp+="\n";
			wrtr<<temp;
			temp="";
			temp.assign(to_string(ptr[i]->getnumofsets()));
			temp+="\n";
			wrtr<<temp;
			temp="";
			temp.assign(to_string(ptr[i]->getlines()));
			temp+="\n";
			wrtr<<temp;
			temp="";
			temp.assign(ptr[i]->getevp());
			temp+="\n";
			wrtr<<temp;
			temp="";
			temp.assign(to_string(ptr[i]->getcurrenthit()));
			temp+="\n";
			wrtr<<temp;
			float t=float(ptr[i]->getcurrenthit())/float(iterations);
			temp="";
			temp.assign(to_string(t));
			temp+="\n";
			wrtr<<temp;
		}
	}
	void writeEtime2file(){
		fstream wtrt;
		wtrt.open("executiontime.txt",ios::out);
		string temp;
		for(int i=0;i<numofcaches;i++){
			temp="";
			temp+=to_string(ptr[i]->getcurEtime());
			temp+="\n";
			wtrt<<temp;
		}
		wtrt.flush();
		wtrt.close();
	}
	~simulator(){
		if(ptr!=nullptr){
			for(int i=0;i<numofcaches;i++){
				if(ptr[i]!=nullptr){
					//delete this->ptr[i]; 
					this->ptr[i]=nullptr;
				}
			}
			//delete [] this->ptr;
			ptr=nullptr;
		}
		fileout.flush();// doesnt flush need to check if file writes completely
	}	
};
int main(){
	srand(time(0));
	int numofiterations,numofcaches,numofsets,linesperset,lc=0;
	fstream freader;
	string tempstr,mempath,ctype,evictionp,preftype;
	freader.open("data.txt",ios::in);
	while(getline(freader,tempstr)){
		cout<<tempstr<<endl;
		if (lc==0)
			mempath.assign(tempstr);
		if (lc==1)
			numofcaches=stoi(tempstr);
		if (lc==2)
			numofiterations=stoi(tempstr);
		lc++;
	}
	freader.close();
	simulator demo(numofcaches);
	cout<<"traces path : "<<mempath<<endl;
	cout<<"number of caches : "<<numofcaches<<endl;
	cout<<"iterations : "<<numofiterations<<endl;
	freader.open("page2.txt",ios::in);
	for(int i=0;i<numofcaches;i++){
		getline(freader,tempstr);
		ctype.assign(tempstr);
		getline(freader,tempstr);
		numofsets=stoi(tempstr.c_str());
		getline(freader,tempstr);
		linesperset=stoi(tempstr.c_str());
		getline(freader,tempstr);
		evictionp.assign(tempstr);
		getline(freader,tempstr);// need to implement this
		preftype.assign(tempstr);
		cout<<"cache "<<i+1<<" of "<<numofcaches<<endl;
		cout<<"cache type : "<<ctype<<endl;
		cout<<"number of sets : "<<numofsets<<endl;
		cout<<"lines per set : "<<linesperset<<endl;
		cout<<"eviction policy : "<<evictionp<<endl;
		cout<<"prefetcher type : "<<preftype<<endl;
		if(ctype.find("Direct") != std::string::npos){
			demo.setcache(i,1,numofsets,1,getevfromstr(evictionp));// 0 is index of the cache , 1 is directmapped, 16 sets ,1 line per cache, doesnt apply here ,1 rr but it doesnt apply here
		}
		else if(ctype.find("Fully") != std::string::npos){//3
			demo.setcache(i,3,1,linesperset,getevfromstr(evictionp));
		}
		else if(ctype.find("N-set") != std::string::npos){//2
			demo.setcache(i,2,numofsets,linesperset,getevfromstr(evictionp));
		}
		else{
			cout<<"invalid cache type specified check file reading \n";
			cout<<ctype<<" ctype recieved\n";
			return 0;
		}
	}
	freader.close();
	fstream input;
	string line;
	lc=0;
	input.open("memtrace",ios::in); // cant read file if its stored in string for some odd reason !
	freader.open("state.txt",ios::out);
	while(getline(input,line)){
		if (lc>=numofiterations)
			break;
		lc++;
		line.erase(0,2);
		demo.emulatebehaviour(line);
		demo.write2file();
		demo.display();
		demo.writehits2file();
		demo.writestate2file(freader,lc);
		cout<<"-----------------------------------------------------\n";			
	}
	demo.writepercentages2file(numofiterations);// change with number of iterations
	demo.writepollution2file();
	demo.writeEtime2file();
	hitwritertofile();
	freader.flush();
	freader.close();
	cout<<"sim ended\n";
	return 0;
}
