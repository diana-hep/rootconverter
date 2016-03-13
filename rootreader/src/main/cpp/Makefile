DEBUG = -g

all:
	mkdir -p ../../../target/native/linux-x86-64
	g++ $(DEBUG) datawalker.cpp staticlib.c -o ../../../target/native/linux-x86-64/librootreadercpp.so \
		-fPIC -shared \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
	root-config --version | sed 's/\/.*//' | sed 's/\(.*\)/root.version=\1/' > ../../../target/root-version.properties
