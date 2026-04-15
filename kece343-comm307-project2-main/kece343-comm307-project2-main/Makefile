CC=gcc
CFLAGS=-g -Wall -std=gnu99 -O3
SRCS=main.c util.c proc.c mem.c loader.c
OBJS=$(SRCS:%.c=$(BUILD_DIR)/%.o)
TARGET=kece343-comm307-project2
BUILD_DIR=build

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

.PHONY: clean test help
clean:
	rm -rf *.o $(TARGET) $(BUILD_DIR)

help:
	@echo "The following options are provided with Make\n\t-make:\t\tbuild simulator\n\t-make clean:\tclean the build\n\t-make test:\ttest your simulator"

test: $(TARGET) test_0 test_1 test_2 test_3 test_4 test_5 test_fact test_leaf

test_0: $(TARGET)
	@echo "Testing example0"; \
	./$(TARGET) sample_input/example0.o | diff -Naur sample_output/example0 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_1: $(TARGET)
	@echo "Testing example1"; \
	./$(TARGET) sample_input/example1.o | diff -Naur sample_output/example1 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_2: $(TARGET)
	@echo "Testing example2"; \
	./$(TARGET) sample_input/example2.o | diff -Naur sample_output/example2 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi
test_3: $(TARGET)
	@echo "Testing example3"; \
	./$(TARGET) sample_input/example3.o | diff -Naur sample_output/example3 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_4: $(TARGET)
	@echo "Testing example4"; \
	./$(TARGET) sample_input/example4.o | diff -Naur sample_output/example4 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_5: $(TARGET)
	@echo "Testing example5"; \
	./$(TARGET) sample_input/example5.o | diff -Naur sample_output/example5 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_fact: $(TARGET)
	@echo "Testing example6"; \
	./$(TARGET) sample_input/example6.o | diff -Naur sample_output/example6 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_leaf: $(TARGET)
	@echo "Testing example7"; \
	./$(TARGET) sample_input/example7.o | diff -Naur sample_output/example7 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

submission:
	@echo "Generating a compressed file ($(SUBMIT)) including loader.c and proc.c"; \
	tar cvzf $(SUBMIT) loader.c proc.c

