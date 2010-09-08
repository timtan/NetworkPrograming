
NAME        = network-programming
VERSION     = 1.0
PKGNAME     = ${NAME}-${VERSION}
SRCPATH     = .
OUTPATH     = build
LIBPATH     = lib

ifndef BUILD_TAG
BUILD_TAG=`date +%Y%m%d`
endif

begin:
	@ant

proto:
	protoc --proto_path=../pbdef/ --java_out=$(SRCPATH) ../pbdef/packet.proto

