
OBJ=client.o
PBDEF=packet.pb.cc
PBOBJ=packet.pb.o
DEP=$(PBDEF)
PROJECT=CLIENT
PROTO_INCLUDE=$(shell pwd)/../3rd_party/google/build/include/
INCLUDE=-I$(PROTO_INCLUDE)
LIBPATH = -L$(shell pwd)/../3rd_party/google/build/lib/ -L/lib64
STATIC = -Xlinker -Bstatic
STATIS_LIB = -lprotobuf 
DYNAMIC= -Xlinker -Bdynamic
DYNAMIC_LIB= -lstdc++ -lpthread

CPPFLAGS+=  $(INCLUDE) $(LIBPATH) $(STATIC) $(STATIS_LIB) $(DYNAMIC) $(DYNAMIC_LIB)
COMMAND=$(CXX) -o $@  $(CPPFLAGS)
$(PROJECT): $(DEP)  $(OBJ) $(PBOBJ)
	$(CXX) -o $(PROJECT) $(OBJ) $(PBOBJ)  $(CFLAGS) $(CPPFLAGS)


PROTOC=$(shell pwd)/../3rd_party/google/build/bin/protoc 
PBDEF_PATH= $(shell pwd)/../pbdef/
PBDEF_FILE= $(PBDEF_PATH)/packet.proto 


#TODO, need to change PBDEF someday.
$(PBDEF): $(PBDEF_FILE) 
	$(PROTOC) -I=$(PBDEF_PATH)	--cpp_out=$(shell pwd) $(PBDEF_FILE)

clean:
	rm *.o
	rm $(PROJECT)
