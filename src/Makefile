all: root2avro.cpp TTreeAvroGenerator.cpp scaffold.cpp
	mkdir -p ../target
	g++ -g root2avro.cpp TTreeAvroGenerator.cpp scaffold.cpp -o ../target/root2avro \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
