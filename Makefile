CC := gcc -std=c11
CXX := g++ -std=c++11
CXXFLAGS := -lssl

DIR_BIN := bin

SRCS := $(wildcard *.c)
DEPS := $(SRCS:.c=.d) 

# 可以换成其他程序
sign: sign.o
	$(CC) $(CFLAGS) -o $(DIR_BIN)/$@ $^

# 模式规则，产生.o文件
%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $(filter %.c, $^)

include $(DEPS)

# 模式规则，产生.d文件
%.d : %.c
	@set -e;\
	$(CC) -MM $(CFLAGS) $(filter %.c,$^) | sed 's,\(.*\)\.o[ :]*,\1.o $@: ,g' > $@

format:
	@#for f in $(shell find . -name '*.c' -or -name '*.cpp' -or -name '*.h' -type f); do astyle $$f; done
	astyle --recursive *.h,*.c,*.cpp

doc:
	doxygen Doxyfile

clean:
	rm -f *.o bin/* *.d