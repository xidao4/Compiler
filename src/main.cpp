#include<iostream>
#include<fstream>
using namespace std;
int main(int argc,char* argv[]){
	
	ifstream infile;
	infile.open("/root/Lab/src/hello.txt",ios::in);
	if(!infile.is_open()){
		cout<<"can't open the file!"<<endl;
		return 0;
	}
	char ch;
	while(!infile.eof()){
		infile.get(ch);
		cout<<ch;
	}
	infile.close();
}
