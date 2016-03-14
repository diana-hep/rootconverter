DEBUG = -g

all:
	mkdir -p ../../../target/native/linux-x86-64
	g++ $(DEBUG) datawalker.cpp staticlib.c -o ../../../target/native/linux-x86-64/libRootReaderCPP.so \
		-fPIC -shared \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer
	root-config --version | sed 's/\/.*//' | sed 's/\(.*\)/root.version=\1/' > ../../../target/root-version.properties
