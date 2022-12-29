CROSS_COMPILE =
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)g++
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm

STRIP       = $(CROSS_COMPILE)strip
OBJCOPY     = $(CROSS_COMPILE)objcopy
OBJDUMP     = $(CROSS_COMPILE)objdump

CFLAGS := -Wall -O2 -std=c++11
LDFLAGS := -rdynamic

BIN = tree

all: $(BIN)

LINK_OBJ = main.o \
	tree.o \
	# coroutine_scheduler.o \

$(BIN): $(LINK_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $(filter %.cpp, $^)

PHONY += clean
clean:
	rm -rf $(LINK_OBJ) $(BIN)

.PHONY: $(PHONY)

