#include<iostream>
#include<fstream>
using namespace std;
int main(int argc,char* argv[]){
	char data[100];
	ifstream infile;
	infile.open("hello.txt",ios::in);
	if(!infile.is_open()){
		cout<<"can't open the file!"<<endl;
		return 0;
	}
	infile>>data;
	cout<<data<<endl;
	infile.close();
}
