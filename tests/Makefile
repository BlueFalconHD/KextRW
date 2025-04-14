TESTS = $(wildcard *.c)
CC = clang
CFLAGS = -Wall -Wextra -I../build/include
LDFLAGS = -L../build/lib -lkextrw
FRAMEWORKS = -framework IOKit -framework CoreFoundation
BUILD_DIR = ../build

all:
	mkdir -p $(BUILD_DIR)/bin
	$(CC) $(CFLAGS) kextrw_test.c -o $(BUILD_DIR)/bin/kextrw_test $(LDFLAGS) $(FRAMEWORKS)
	ldid -Sentitlements.plist $(BUILD_DIR)/bin/kextrw_test

clean:
	rm -rf $(BUILD_DIR)/bin
