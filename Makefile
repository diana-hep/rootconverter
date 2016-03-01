DEBUG = -g

all:
	mkdir -p build
	g++ $(DEBUG) src/root2avro.cpp src/datawalker.cpp -o build/root2avro \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
