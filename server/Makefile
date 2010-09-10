
NAME        = network-programming
VERSION     = 1.0
PKGNAME     = ${NAME}-${VERSION}
SRCPATH     = .
OUTPATH   	= class
LIBPATH     = lib

ifndef BUILD_TAG
BUILD_TAG=`date +%Y%m%d`
endif

begin:
	@ant

proto:
	../3rd_party/google/build/bin/protoc --proto_path=../pbdef/ --java_out=$(SRCPATH) ../pbdef/packet.proto
	
clean:
	rm -r $(LIBPATH)
	rm -r $(OUTPATH)
	rm -r $(SRCPATH)/com
