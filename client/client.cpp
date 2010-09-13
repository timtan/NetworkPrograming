#include"packet.pb.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include<cstdio>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<openssl/evp.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <typeinfo>
using namespace com::trend;
using namespace std;
using namespace google::protobuf::io;
int calc_dgst(char *msg, unsigned int msg_size, unsigned char *dgst, unsigned int *dgst_len,char * DGST_FUNC)
{
	EVP_MD_CTX mdctx;
	const EVP_MD *md;

	static bool once = true;
	if( once ){
		once = false;
		OpenSSL_add_all_digests();
	}
	md = EVP_get_digestbyname(DGST_FUNC);

	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, md, NULL);
	EVP_DigestUpdate(&mdctx, msg, msg_size);
	EVP_DigestFinal_ex(&mdctx, dgst, dgst_len);
	EVP_MD_CTX_cleanup(&mdctx);


	return 0;
}
int dgst2str(unsigned char *dgst, int dgst_len, char *buf, int buf_len)
{
	int i;

	buf[0] = '\0';
	for(i = 0; i < dgst_len; i++)
		snprintf(buf+strlen(buf), buf_len-strlen(buf), "%02x", dgst[i]);

	return 0;
}



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
	cout<<"ack size recorded as "<< ack_size << endl ;
	int limit = codedInput.PushLimit( ack_size );
	bool ack_parse_ret = ack.ParseFromCodedStream(&codedInput);
	codedInput.PopLimit( limit );
	if( false == ack_parse_ret ){
		cerr<<"parse response error" <<endl;
		return false;
	}
	if(not ack.success()){
		return false;
	}
	else{
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

template<typename PB>
bool PB_serilize( PB& pb , int fd ){
	//FIXME, 50 is ok, but need a better inference.
	int potential_size = 100 + pb.ByteSize() ;
	char* buffer = new char[ potential_size  ] ;

	ArrayOutputStream raw_output( buffer, potential_size   );
	{
		CodedOutputStream codedOutput( &raw_output);
		codedOutput.WriteVarint32(pb.ByteSize());	
		bool serialize_ret = pb.SerializeToCodedStream( &codedOutput ) ;
		if( false == serialize_ret ){
			cerr<<"Serialize error "<< endl ;
			return -1;
		}
	}
	write( fd, buffer, raw_output.ByteCount() ) ;
	delete[] buffer;
}
#define show_var( var, out  )  out<<#var<<":"<<var<<"."<<endl
int main(int argc, char** argv ){

	//Receieving Argument.
	string tcp_host = "127.0.0.1";
	string tcp_port = "2010";
	string file_name = "test.html";
	if( argc != 4  ) {
		cerr<<"The program need to pass host ip port" << endl
			<<"Now, It is in testing mode" << endl ;
	}
	else{
		tcp_host = argv[1];
		tcp_port = argv[2];
		file_name = argv[3];
	}
	show_var( tcp_host, cout  );
	show_var( tcp_port, cout  );
	show_var( file_name, cout );



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

	Header h;
	if( not setup_header( h, file_name.c_str() )  ){
		cerr<<"header setting error" <<endl;
		return -1;
	}

	PB_serilize( h , tcp_socket );
	Ack ack;
	google::protobuf::io::FileInputStream  raw_input( tcp_socket );
	google::protobuf::io::CodedInputStream codedInput( &raw_input);
	if( false == read_ack( ack, codedInput ) ){
		cerr<<"read_ack error"<<endl;
		return -1;
	}

	int seq_num = 0 ;
	ifstream fin( file_name.c_str()  );			
	while(fin){
		cout<< seq_num << endl ;
		const int buffer_size = 1024;
		char buffer[ buffer_size ] ;
		fin.read( buffer , buffer_size ) ;
		
		int readded_size = fin.gcount() ;


		unsigned char bin_dgst[EVP_MAX_MD_SIZE];
		char calclated_digest[EVP_MAX_MD_SIZE*2 + 1];
		unsigned int bin_dgst_len;

		// calculate diget from post_data.
		calc_dgst(buffer, readded_size, bin_dgst, &bin_dgst_len, "MD5");
		dgst2str(bin_dgst, bin_dgst_len, calclated_digest, sizeof(calclated_digest));
		Block block;	
		seq_num+=1;
		block.set_seq_num( seq_num );
		block.set_content( buffer, readded_size );
		block.set_size( readded_size );
		block.set_digest( calclated_digest  );
		block.set_eof( false );
		PB_serilize( block, tcp_socket );
		if( false == read_ack( ack, codedInput ) ){
			cerr<<"read_ack error"<<endl;
			//return -1;
		}
		else{
			cout<<"read_ack ok" << endl;
		}
	}
	Block block;	//last block
	seq_num+=1;
	block.set_seq_num(  0 );
	string dummy_buffer = "dummy" ;
	block.set_content( dummy_buffer.c_str()  , 0   );
	block.set_size( 0 );
	block.set_digest( h.digest() );
	block.set_eof( true);
	PB_serilize( block , tcp_socket );
	if( false == read_ack( ack, codedInput ) ){
		cerr<<"read_ack error"<<endl;
		//return -1;
	}
	else{
		cout<<"read_ack ok" << "file transer complete"<< endl;
	}
	close( tcp_socket );
	return 0 ;
}
