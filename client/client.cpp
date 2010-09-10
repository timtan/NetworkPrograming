#include"packet.pb.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iostream>
#include<cstdio>
#include<sstream>
using namespace com::trend;
using namespace std;
int main(int argc, char** argv ){
	Header h;
	//TODO, get file name from cli 
	char * file_name = "/tmp/test.html";
	h.set_file_name(file_name);
		
	struct stat st;
	//TODO, check file existance.
	stat( file_name , &st );
	h.set_file_size( st.st_size ) ;

	//whole file md5
	ostringstream command;
	//TODO escape all special character 
	command<<"md5sum \"" << file_name << "\"" <<endl ;
	FILE* command_output_fp = popen( command.str().c_str() , "r");
	char *command_output_line = NULL ;
	size_t command_output_line_sz = 0 ;

	//TODO error checking
	getline(&command_output_line, &command_output_line_sz, command_output_fp);
	istringstream  command_intput ;
	command_intput.str( command_output_line ) ;	
	string md5sum = 	
	
	return 0 ;
}
