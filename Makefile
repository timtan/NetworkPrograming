include Makefile.inc

SERVER_LIB=$(shell pwd)/server/lib/${PROTOBUF_JAVA_RUNTIME} 

all:${SERVER_LIB} CLIENT
	make proto -C server
	make -C server


${SERVER_LIB}: ${PROTOBUF_JAVA_RUNTIME}
	mkdir -p ./server/lib
	cp ${PROTOBUF_JAVA_RUNTIME} ./server/lib;

${PROTOBUF_JAVA_RUNTIME}: ${PROTOC} 
	alias protoc=$(PROTOC); \
	export LD_LIBRARY_PATH=${PROTOBUF_LIB_PATH}:$${LD_LIBRARY_PATH} ; \
	cd ${PROTOBUF_JAVA_RUNTIME_SRC}; \
	$(PROTOC) --java_out=src/main/java -I../src \
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
	./configure --prefix=$(PROTOBUF_DIR)/build $(CXXFLAGS); \
	make; \
	make install; \
	cd -;
#TODO, need to refine 
CLIENT: $(PROTOC)
	make -C client
