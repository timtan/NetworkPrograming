#include"packet.pb.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iostream>
#include<cstdio>
#include<sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

using namespace com::trend;
using namespace std;
bool setup_header( Header& h,const char * file_name ){
	h.set_file_name(file_name);
		
	struct stat st;
	//TODO, check file existance.
	stat( file_name , &st );
	h.set_file_size( st.st_size ) ;

	//whole file md5
	ostringstream command;
	//TODO escape all special character 
	command<<"md5sum \"" << file_name << "\"" <<endl ; FILE* command_output_fp = popen( command.str().c_str() , "r"); char *command_output_line = NULL ; size_t command_output_line_sz = 0 ;

	//TODO error checking
	getline(&command_output_line, &command_output_line_sz, command_output_fp);
	fclose( command_output_fp );
	istringstream  command_intput ;
	command_intput.str( command_output_line ) ;	
	string md5sum;
	command_intput>>md5sum;
	h.set_digest( md5sum );

	return true;	
}
int Connect(int tcp_socket, struct addrinfo* server_addr ){
	if( server_addr == NULL )
		return -1 ;
	int connect_ret = connect( tcp_socket, server_addr->ai_addr,  server_addr->ai_addrlen ) ;

	//TODO: use exception.
	if( connect_ret != 0 ){
		perror("connect failed");
	}	
	return connect_ret ;
}
int main(int argc, char** argv ){
	Header h;
	//TODO, get file name from cli 
	string file_name = "/tmp/test.html";
	string tcp_host = "127.0.0.1";
	string tcp_port = "8010";
	
	setup_header( h, file_name.c_str() ) ;
	

    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints;
    struct addrinfo* server_addr = NULL;

	//Address Structure fill.
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET ;
    hints.ai_socktype = SOCK_STREAM ;
    hints.ai_protocol = 0 ; 

	int ret;
    if ((ret = getaddrinfo(tcp_host.c_str(), tcp_port.c_str(), &hints, &server_addr)) != 0)
    {   
		    //TODO refine error message.
            cout << gai_strerror(ret) << endl;
			return -1;
    }   

	if( 0 != Connect( tcp_socket, server_addr ) ){
		return -1;	
	}

    h.SerializeToFileDescriptor( tcp_socket );	
	Ack ack;
	bool parse_ack_result = ack.ParseFromFileDescriptor( tcp_socket );
	if(not ack.success()){
		cerr<<"ack error" <<endl;
		return -1;
	}
	else{
		cerr<<"ack success"<<endl;
	}
	while(true){
		
	}
	return 0 ;
}
