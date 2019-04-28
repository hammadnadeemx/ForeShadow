#include <iostream>
#include <stdlib.h>     
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
void preparedata(string filename,string desgname,unsigned int n){// turns sequential data into 2d matrix of n+1 columns where n+1 is the next address accesed after the first n are.
	cout<<"Processing data.\n";
	unsigned long *datarray;
	unsigned long min;// used to make all address relative ( because the mem space is huge and will help speed up the nn training)
	datarray=new unsigned long [n];
	fstream input,outputx,outputy;
	string line="",temp="";
	input.open(filename,ios::in);
	getline(input,line);
	istringstream converter(line);
	unsigned long va;
	converter>>hex>>va;
	min=va;
	while(!input.eof()){
		getline(input,line);
		istringstream converter(line);
		unsigned long vas;
		converter>>hex>>vas;
		if(min>vas)
			min=vas;
	}
	input.close();
	input.open(filename,ios::in);
	string d1,d2;
	d1=desgname;
	d2=desgname;
	d1+="x.csv";
	d2+="y.csv";
	outputx.open(d1,ios::out);
	outputy.open(d2,ios::out);
	unsigned long count=0;
	while(!input.eof()){
		if(count>=n){// skips the first n elements for obvious reasons
			temp="";
			for(unsigned int j=0;j<n;j++){
				temp+=to_string(datarray[(count-n+j)%n]);
				temp+=",";
			}
			temp.erase(temp.length()-1,1);
			temp+="\n";
			outputx<<temp;
			temp="";
			getline(input,line);
			istringstream converter(line);
			unsigned long value;
			converter>>hex>>value;
			datarray[count%n]=value-min;
			temp+=to_string(value-min);
			temp+="\n";
			outputy<<temp;
		}
		else{
			getline(input,line);
			istringstream converter(line);
			unsigned long value;
			converter>>hex>>value;
			datarray[count%n]=value-min;
		}
		count++;
	}
	input.close();
	outputx.close();
	outputy.close();
	delete [] datarray;
	cout<<"Data count is "<<count<<".\n";
}
int main(void)
{
	preparedata("memtrace","testdata",5);// 5 columns for x and one for y
	return 0;
}
