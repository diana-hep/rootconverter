DEBUG = -g

all:
	mkdir -p build
	g++ $(DEBUG) src/root2avro.cpp src/datawalker.cpp -o build/root2avro \
		-Wl,--no-as-needed $(shell root-config --cflags --ldflags) -L/opt/root/lib -lm -lCore -lRIO -lTree -lTreePlayer \
		$(shell pkg-config avro-c --cflags --libs) \
		$(shell pkg-config jansson --cflags --libs)
