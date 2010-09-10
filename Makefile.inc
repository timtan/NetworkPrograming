
THIRD_PARTY_DIR         = $(shell pwd)/3rd_party
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
