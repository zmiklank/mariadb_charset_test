CXX=gcc
CXXFLAGS= -g `mariadb_config --cflags --libs`

all: charset_combinations_test base_test

charset_combinations_test: charset_combinations_test.c
	$(CXX) $(CXXFLAGS) charset_combinations_test.c -o charset_combinations_test

base_test: base_test.c
	$(CXX) $(CXXFLAGS) base_test.c -o base_test

clean:
	rm charset_combinations_test base_test

.PHONY: clean
