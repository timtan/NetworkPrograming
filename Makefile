THIRD_PARTY_DIR         = ./3rd_party
PROTO_BUF_VERSION       = 2.3.0
ARCHIVE_TYPE            = tar.bz2
PROTOBUF_PKG            = protobuf-$(PROTO_BUF_VERSION)
PROTOBUF_DIR            = $(THIRD_PARTY_DIR)/google
PROTOBUF_LIB_PATH       = $(PROTOBUF_DIR)/build/lib

#Protobuffer compiler
PROTOC                  =$(PROTOBUF_DIR)/build/bin/protoc


# protobuf IDL path and files
PROTO_BUF_DEF_PATH      = ./pbdef
PROTO_NAMES             = 
PROTO_FILES             = $(addprefix $(PROTO_BUF_DEF_PATH)/ ,$(addsuffix .proto , $(PROTO_NAMES) ) )


$(PROTOC):  
	mkdir -p $(PROTOBUF_DIR);\
	tar xjf $(THIRD_PARTY_DIR)/$(PROTOBUF_PKG).$(ARCHIVE_TYPE) -C $(PROTOBUF_DIR);\
	cd $(PROTOBUF_DIR); \
	cd $(PROTOBUF_PKG); \
	./configure --prefix=$(shell pwd)/$(PROTOBUF_DIR)/build $(CXXFLAGS); \
	make; \
	make install; \
	cd -;
