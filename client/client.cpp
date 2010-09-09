
#include"packet.pb.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
using namespace com::trend;
using namespace std;
int main(int argc, char** argv ){
	Header h;
	//TODO, get file name from cli 
	char * file_name = "CLIENT";
	h.set_file_name(file_name);
		
	struct stat st;
	//TODO, check file existance.
	stat( file_name , & st );

	h.set_file_size( st.st_size ) ;

	
	
	return 0 ;
}
