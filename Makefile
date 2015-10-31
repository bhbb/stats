CC=g++
FILE_ENDING=cpp
CFLAGS = -std=c++11 -Wall -g
OBJS=
OBJS+=util.o
OBJS+=main.o
TARGET=stats
LIBS=


all: compile link

SRC=$(OBJS:%.o=%.${FILE_ENDING})

compile:
	@for src in $(SRC) ; do \
		echo "\tCC\t$$src"; \
		$(CC) $(CFLAGS) -c $$src ; \
	done

link:
	@echo "\tLINK\t $(TARGET)"
	@$(CC) $(OBJS) $(LIBS) -o $(TARGET)

clean:
	@echo "\tRM\t*.o"
	@rm *.o

.PHONY: clean 
