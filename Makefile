DEBUG = -g

all: src/root2avro.cpp src/TTreeAvroGenerator.cpp src/scaffold.cpp
	mkdir -p build
	g++ $(DEBUG) src/root2avro.cpp src/TTreeAvroGenerator.cpp src/scaffold.cpp -o build/root2avro \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
