CC = gcc
CFLAGS = -c -Wall -Wextra -Wcast-qual -g -DPJ_DEBUG
LDFLAGS = 
SOURCES = example.c pj.c
OBJECTS = $(SOURCES:.c=.o)

vpath %.c src/

EXECUTABLE = build/pj_example

.PHONY: clean clean-all

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)

clean-all:
	rm -rf *.o && rm $(EXECUTABLE)
