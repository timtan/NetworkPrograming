THIRD_PARTY_DIR         = ./3rd_party
PROTO_BUF_VERSION       = 2.3.0
ARCHIVE_TYPE            = tar.bz2
PROTOBUF_PKG            = protobuf-$(PROTO_BUF_VERSION)
PROTOBUF_DIR            = $(THIRD_PARTY_DIR)/google
PROTOBUF_LIB_PATH       = $(PROTOBUF_DIR)/build/lib

#Protobuffer compiler
PROTOC                  =$(PROTOBUF_DIR)/build/bin/protoc


#JAVA
PROTOBUF_JAVA_RUNTIME     = protobuf-java-$(PROTO_BUF_VERSION).jar
PROTOBUF_JAVA_RUNTIME_SRC = ${PROTOBUF_DIR}/protobuf-${PROTO_BUF_VERSION}/java


# protobuf IDL path and files
PROTO_BUF_DEF_PATH      = ./pbdef
PROTO_NAMES             = 
PROTO_FILES             = $(addprefix $(PROTO_BUF_DEF_PATH)/ ,$(addsuffix .proto , $(PROTO_NAMES) ) )


${PROTOBUF_JAVA_RUNTIME}: ${PROTOC}
	alias protoc=$(PROTOC); \
	export LD_LIBRARY_PATH=${PROTOBUF_LIB_PATH}:$${LD_LIBRARY_PATH} ; \
	cd ${PROTOBUF_JAVA_RUNTIME_SRC}; \
	protoc  --java_out=src/main/java -I../src \
	../src/google/protobuf/descriptor.proto; \
	mkdir -p build; \
	javac -d build src/main/java/com/google/protobuf/*; \
	jar   -cvf $@ -C build/ .;
	mv ${PROTOBUF_JAVA_RUNTIME_SRC}/$@ .;

$(PROTOC):  
	mkdir -p $(PROTOBUF_DIR);\
	tar xjf $(THIRD_PARTY_DIR)/$(PROTOBUF_PKG).$(ARCHIVE_TYPE) -C $(PROTOBUF_DIR);\
	cd $(PROTOBUF_DIR); \
	cd $(PROTOBUF_PKG); \
	./configure --prefix=$(shell pwd)/$(PROTOBUF_DIR)/build $(CXXFLAGS); \
	make; \
	make install; \
	cd -;
