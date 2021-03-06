CXX := g++ -std=c++11
CXXFLAGS := -lssl -lcrypto
INCLUDES :=-I/usr/local/opt/openssl@1.1/include
LIBS := -L/usr/local/opt/openssl@1.1/lib

DIR_BIN := bin
DIRS := $(DIR_BIN)
.PHONY: all run unit_test unit_test_run main main_run

all: mt_unit_test bf_unit_test mt_and_bf_test

run: mt_unit_test_run bf_unit_test_run bf_unit_test_run

LIB_SOURCES1 := ./test/merkle_tree_unit_test.cpp ./src/merkle_tree.cpp ./util/hash.cpp ./util/coding.cpp ./util/bloom_filter.cpp
mt_unit_test: $(DIR_BIN)
	$(CXX) $(INCLUDES) $(LIBS) -o $(DIR_BIN)/$@ $(LIB_SOURCES1) $(CXXFLAGS)

mt_unit_test_run:
	./$(DIR_BIN)/mt_unit_test

LIB_SOURCES2 := ./test/bloom_filter_unit_test.cpp ./util/bloom_filter.cpp ./util/hash.cpp
bf_unit_test: $(DIR_BIN)
	$(CXX) $(INCLUDES) $(LIBS) -o $(DIR_BIN)/$@ $(LIB_SOURCES2) $(CXXFLAGS)

bf_unit_test_run:
	./$(DIR_BIN)/bf_unit_test

LIB_SOURCES3 := ./test/mt_and_bf_test.cpp ./src/merkle_tree.cpp ./util/bloom_filter.cpp ./util/coding.cpp  ./util/hash.cpp 
mt_and_bf_test: $(DIR_BIN)
	$(CXX) $(INCLUDES) $(LIBS) -o $(DIR_BIN)/$@ $(LIB_SOURCES3) $(CXXFLAGS)

mt_and_bf_test_run:
	./$(DIR_BIN)/mt_and_bf_test

LIB_SOURCES4 := ./test/main.cpp ./util/hash.cpp ./util/coding.cpp
main: $(DIR_BIN)
	$(CXX) $(INCLUDES) $(LIBS) -o $(DIR_BIN)/$@ $(LIB_SOURCES4) $(CXXFLAGS)

main_run:
	./$(DIR_BIN)/main

$(DIRS):
	mkdir $@

clean:
	rm -rf $(DIR_BIN)