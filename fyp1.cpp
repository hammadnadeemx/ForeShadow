// Hammad Nadeem 150162
#include <iostream>
#include <stdlib.h>     
#include <time.h>
using namespace std;
unsigned long int curtime=0;// time used for time based eviction policies
class cacheblock{
	private:
	unsigned int *memblock;// the fullfat storage -cache lines in our case stores mem addresses
	int blocksize;// number of data lines
	int ep;// eviction policy needed to handle various tasks
	unsigned int *freqcount;// used to store frequency of lfu or time stamp of lru
	int indexf; // used by fifo or lifo to manage data
	public:
	cacheblock(int size=15,int eviction=1){ // setup of variables e -> 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out
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
		memblock=new unsigned int[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]=0;			
		}
		//cout<<"Cache block with "<<blocksize<<" lines built \n";
	}
	void editparameters(int size=15,int eviction=1){
		//if(memblock!=nullptr)
			//delete[] memblock;
		//if(freqcount!=nullptr)
			//delete[]freqcount;// add checks here to prevent segfaults
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
		memblock=new unsigned int[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]=0;			
		}
		cout<<"Cache configuration updated \n";
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
	bool isHit(unsigned int memaddress){// check to see if data at memory is present in cache block
		if(memaddress>=0){
			for(int i=0; i<blocksize;i++){
				if(memblock[i]==memaddress){
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
	void fetchFromMem(unsigned int memaddress){// instructs to place mem in cache
		switch(ep){
			case 1:{ // random replacement 
				int p=rand()%blocksize;
				memblock[p]=memaddress;	
				break;
			}
			case 2:{//least recently used 
				unsigned int minval=freqcount[0],minindex=0;
				for(int i=0; i<blocksize;i++){
					if(minval>=freqcount[i]){
						minindex=i;
						minval=freqcount[i];
					}
				}
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
				memblock[minindex]=memaddress;
				freqcount[minindex]=1;		
				break;
			}
			case 4:{//first in first out
				memblock[indexf]=memaddress;
				indexf++;
				if(indexf>=blocksize)
					indexf=0;// wrap it around !
				break;
			}
			case 5:{//last in first out like a stack
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
				memblock[maxindex]=memaddress;
				freqcount[maxindex]=curtime;			
				break;
			}
			default:
				cout<<"invalid ep value\n";					
		}	
	}
	~cacheblock(){
		//if(memblock!=nullptr)
		//	delete[] memblock;// sort segfaults 
		//if(freqcount!=nullptr)
		//	delete[] freqcount;
	}
};
class cache{// need to turn this into abstract class
	private:
	cacheblock *ptr;// the cache blocks
	int nblocks; // len of ptr in otherwords number of sets
	int type;// 1 is direct mapped ,2 is n-set associative, 3 is fully set associative
	int epolicy;// 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out 
	int lines;// used to tell how many lines per block/set of cache
	unsigned int hits;
	public:
	cache(int sets=1, int ctype=1,int evictionpolicy=1,int linesperset=8){
		nblocks=sets;
		type=ctype;
		hits=0;
		epolicy=evictionpolicy;
		lines=linesperset;
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
	void editcacheconfig(int sets=1, int ctype=1,int evictionpolicy=1,int linesperset=8){
		//if(ptr!=nullptr)
			//delete [] ptr;
		nblocks=sets;
		type=ctype;
		hits=0;
		epolicy=evictionpolicy;
		lines=linesperset;
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
	bool isHit(unsigned int memaddr){// need to call this to maintain time value else update time manually
		for(int i=0;i<nblocks;i++){
			if(ptr[i].isHit(memaddr)){
				hits++;
				return true;		
			}	
		}
		return false;
	}
	void display(){// just to see whats going on 
		for(int i=0;i<nblocks;i++){
			cout<<"Block "<<i<<":\n";
			ptr[i].print();
		}		
	}
	void updatecache(unsigned int memaddress){// where does the pretecher come in ? needs work
		switch(type){
			case 1:{ // direct mapped
				int setnum=memaddress%nblocks;
				ptr[setnum].fetchFromMem(memaddress);
				break;
			}
			case 2:{ // nset associative
				int setnum=memaddress%nblocks;
				ptr[setnum].fetchFromMem(memaddress);
				break;
			}
			case 3:{// fully associative
				ptr->fetchFromMem(memaddress);
				break;
			}
			default:
				cout<<"invalid value for type of cache\n";			
		}
	}
	~cache(){
		//delete[] ptr;	// this creates a seg fault need to debug this	
	}
};
class directmapped: public cache{
	private:
	cache *ptr;
	public:
	directmapped(int numsets=8,int evictionpolicy=1){
		ptr=new cache(numsets,1,evictionpolicy,1);
	}
	void editcacheconfig(int numsets=8,int evictionpolicy=1){
		//if(ptr!=nullptr)
			//delete ptr;
		ptr=new cache(numsets,1,evictionpolicy,1);
	}
	bool ishit(unsigned int memaddr){
		return ptr->isHit(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void updatecache(unsigned int memaddr){
		ptr->updatecache(memaddr);
	}	
	~directmapped(){
		//delete ptr;
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
		//if(ptr!=nullptr)
			//delete ptr;
		ptr=new cache(1,3,evictionpolicy,numoflines);
	}
	bool ishit(unsigned int memaddr){
		return ptr->isHit(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void updatecache(unsigned int memaddr){
		ptr->updatecache(memaddr);
	}	
	~fullyassociative(){
		//delete ptr;
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
		//if(ptr!=nullptr)
			//delete ptr;
		ptr=new cache(numofsets,2,evictionpolicy,numoflines);
	}
	bool ishit(unsigned int memaddr){
		return ptr->isHit(memaddr);
	}
	unsigned int getcurrenthit(){
		return ptr->getcurrenthit();
	}
	void display(){
		ptr->display();
	}
	void updatecache(unsigned int memaddr){
		ptr->updatecache(memaddr);
	}	
	~nsetassociative(){
		//delete ptr;
	}	
};
class simulator{
	private:
	cache **ptr;//  array of caches
	bool setup;// all caches under this simulation ready for use ?
	int numofcaches;// number of caches
	public:
	simulator(int numberofcaches){
		ptr=new cache * [numberofcaches];
		numofcaches=numberofcaches;
	}
	bool setcache(int index, int type, int sets,int lines, int eviction){
		bool op=false;
		if((index<numofcaches)&&(index>=0)){
			switch(type){
				case 1:// direct mapped
					ptr[index]=new directmapped(sets,eviction);
					ptr[index]->editcacheconfig(sets,1,eviction,1);//need to fix polymorphism
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
	bool feedforward(unsigned int memaddr){// the concept of pre-fecthing should be added here--- We cant ask the cache to fit every thing from ram in it !!!
		curtime++; // increament time value
		for(int i=0;i<numofcaches;i++){
			if(!(ptr[i]->isHit(memaddr))){
				ptr[i]->updatecache(memaddr);
				return false;	
			}
		}
		return true;
	}
	void display(){
		for(int i=0;i<numofcaches;i++){
			cout<<"cache "<<i+1<<endl;
			ptr[i]->display();
			cout<<"number of hits "<<ptr[i]->getcurrenthit()<<endl<<endl;		
		}
	}
	~simulator(){
		if(ptr!=nullptr){
			for(int i=0;i<numofcaches;i++){
				if(ptr[i]!=nullptr){
					//delete ptr[i]; fix these seg faults later
				}
			}
			//delete [] ptr;
		}
	}	
};
int main(){
	srand(time(0));
	simulator demo(2);
	demo.setcache(0,1,16,1,2);// 0 is index of first cache , 1 is directmapped, 16 sets ,1 line per cache doesnt apply here ,2 is least recently used 
	demo.setcache(1,3,8,16,3);//1th cache, fully associative,doesnt apply here,16 lines, least frequently used
	unsigned int t=0;
	for(int i=0; i<2000;i++){
		t=rand()%500;
		demo.feedforward(t);
		demo.display();
		cout<<"-----------------------------------------------------\n";			
	}
	return 0;
}
