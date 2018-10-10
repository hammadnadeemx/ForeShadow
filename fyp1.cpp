// Hammad Nadeem 150162
#include <iostream>
#include <stdlib.h>     
#include <time.h>
using namespace std;
unsigned long int curtime=0;// time used for time based eviction policies
class cacheblock{
	private:
	unsigned short *memblock;// the fullfat storage -cache lines in our case
	int blocksize;// number of data lines
	int ep;// eviction policy needed to handle various tasks
	unsigned short *freqcount;// used to store frequency of lfu or time stamp of lru
	int indexf; // used by fifo or lifo to manage data
	public:
	cacheblock(int size=15,int eviction=1){ // setup of variables e -> 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out
		blocksize=size;
		ep=eviction;
		if((ep==2)||(ep==3)){// need more storage for frequencies or time stamps
			freqcount=new unsigned short[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=0;			
			}
		}
		else if(ep==6){// need more storage for frequencies or time stamps
			freqcount=new unsigned short[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=9999;	// max value so cache can fill initially		
			}
		}
		else{
			freqcount=NULL;
		}
		indexf=0;
		memblock=new unsigned short[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]=0;			
		}
		cout<<"Cache block with "<<blocksize<<" lines built \n";
	}
	void editparameters(int size=15,int eviction=1){
		delete[] memblock;
		delete[]freqcount;// add checks here to prevent segfaults
		blocksize=size;
		ep=eviction;
		if((ep==2)||(ep==3)){// need more storage for frequencies or time stamps
			freqcount=new unsigned short[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=0;			
			}
		}
		else if(ep==6){// need more storage for frequencies or time stamps
			freqcount=new unsigned short[blocksize];
			for(int i=0; i<blocksize;i++){
				freqcount[i]=9999;	// max value so cache can fill initially		
			}
		}
		else{
			freqcount=NULL;
		}
		indexf=0;
		memblock=new unsigned short[blocksize];
		for(int i=0; i<blocksize;i++){
			memblock[i]=0;			
		}
		cout<<"Cache updated \n";
	}
	void display(){// print content of block
		//cout<<"blocksize: "<<blocksize<<" ep: "<<ep<<" index: "<<indexf<<endl;
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
	bool isHit(unsigned short memaddress){// check to see if data at memory is present in cache block
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
	void fetchFromMem(unsigned short memaddress){// instructs to place mem in cache
		switch(ep){
			case 1:{ // random replacement 
				int p=rand()%blocksize;
				memblock[p]=memaddress;	
				break;
			}
			case 2:{//least recently used 
				unsigned short minval=freqcount[0],minindex=0;
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
				unsigned short minval=-1,minindex=0;
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
				unsigned short maxval=freqcount[0],maxindex=0;
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
		delete[] memblock;// sort segfaults 
	}
};
class cache{
	private:
	cacheblock *ptr;// the cache blocks
	int nblocks; // len of ptr in otherwords number of sets
	int type;// 1 is direct mapped ,2 is n-set associative, 3 is fully set associative
	int epolicy;// 1 is random replacement ,2 is least recently used, 3 is least frequently used, 4 is first in first out 
	int lines;// used to tell how many lines per block/set of cache
	public:
	cache(int sets=1, int ctype=1,int evictionpolicy=1,int linesperset=8){
		nblocks=sets;
		type=ctype;
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
	bool isHit(unsigned short memaddr){// need to call this to maintain time value else update time manually
		curtime++; // increament time value
		for(int i=0;i<nblocks;i++){
			if(ptr[i].isHit(memaddr))
				return true;			
		}
		return false;
	}
	void display(){// just to see whats going on 
		for(int i=0;i<nblocks;i++){
			cout<<"Block "<<i<<":\n";
			ptr[i].display();
			cout<<endl;
		}		
	}
	void updatecache(unsigned short memaddress){// where does the pretecher come in ? needs work
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
		delete[] ptr;	// this creates a seg fault need to debug this	
	}
};
int main(){
	srand(time(0));
	cache test(4,2,3,8); //4-way set associative, 2 means n-set associative,lfu policy, 8 lines per set
	unsigned short t=0,hit=0;
	for(int i=0; i<2000;i++){
		t=rand()%3000;
		if(test.isHit(t)){
			hit++;			
		}
		else{
			test.updatecache(t);
		}
		test.display();
	}
	cout<<hit<<" hits out of 2000\n";
	return 0;
}
