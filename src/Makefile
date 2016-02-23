all: root2avro.cpp TTreeAvroGenerator.cpp
	mkdir -p ../target
	g++ root2avro.cpp TTreeAvroGenerator.cpp -o ../target/root2avro \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags --libs) -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
