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

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <fstream>
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
bool read_ack( Ack ack, google::protobuf::io::CodedInputStream& codedInput ){
	//TODO: use exception.
	unsigned int ack_size;
	codedInput.ReadVarint32( &ack_size );
	cout<<"ack head size"<< ack_size << endl ;
	bool ack_parse_ret = ack.ParsePartialFromCodedStream(&codedInput);
	if( false == ack_parse_ret ){
		cerr<<"parse response error" <<endl;
		return false;
	}
	if(not ack.success()){
		cerr<<"ack error" <<endl;
		return false;
	}
	else{
		cerr<<"ack success"<<endl;
		return true;
	}
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
	string file_name = "test.html";
	string tcp_host = "127.0.0.1";
	string tcp_port = "2010";
	
	if( not setup_header( h, file_name.c_str() )  ){
		cerr<<"header setting error" <<endl;
		return -1;
	}
	

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

	google::protobuf::io::FileOutputStream raw_output( tcp_socket );
	google::protobuf::io::CodedOutputStream codedOutput( &raw_output);


	codedOutput.WriteVarint32(h.ByteSize());	
    h.SerializeToCodedStream( &codedOutput ) ;
	raw_output.Flush();


	Ack ack;
	google::protobuf::io::FileInputStream  raw_input( tcp_socket , 100);
	google::protobuf::io::CodedInputStream codedInput( &raw_input);
	if( false == read_ack( ack, codedInput ) ){
		cerr<<"read_ack error"<<endl;
		return -1;
	}

	int seq_num = 0 ;
	ifstream fin( file_name.c_str()  );			
	while(fin){

		const int buffer_size = 1024;
		char buffer[ buffer_size ] ;
		fin.read( buffer , buffer_size ) ;
		cout<<"Read some data"
			<<fin.gcount() 
			<<buffer[0] <<endl;
		int readded_size = fin.gcount() ;
		

		Block block;	
		seq_num+=1;
		block.set_seq_num( seq_num );
		block.set_content( buffer, readded_size );
		block.set_size( readded_size );
		block.set_digest( "xxx" );
		block.set_eof( false );

		codedOutput.WriteVarint32(block.ByteSize());	
		block.SerializeToCodedStream( &codedOutput ) ;
		raw_output.Flush();

		if( false == read_ack( ack, codedInput ) ){
			cerr<<"read_ack error"<<endl;
			return -1;
		}
	}
		Block block;	//last block
		seq_num+=1;
		block.set_seq_num(  0 );
		block.set_content( "0" , 1 );
		block.set_size( 0 );
		block.set_digest( 0 );
		block.set_eof( true);
		codedOutput.WriteVarint32(block.ByteSize());	
		block.SerializeToCodedStream( &codedOutput ) ;
		raw_output.Flush();
	return 0 ;
}
