# Sriram Balasubramaniam
# Charm Patel
# Karthik Shankar
# Brian Stebar II 

# ECE 6102 - Spring 2014
# Term Project - Distributed File System

CC        = gcc
INC_DIRS  =
LIBS_DIRS =
CPP_FLAGS = -Wall -ansi -pedantic
LIBS      = 

COMMON_SRC = 
COMMON_OBJ = $(patsubst %.c,%.o,$(COMMON_SRC))
SERVER_SRC = 
SERVER_OBJ = $(patsubst %.c,%.o,$(SERVER_SRC))
CLIENT_SRC = 
CLIENT_OBJ = $(patsubst %.c,%.o,$(CLIENT_SRC))

all: bacs-server bacs-client

bacs-server: $(COMMON_OBJ) $(SERVER_OBJ)
	$(CC) $(LIBS_DIRS) $(COMMON_OBJ) $(SERVER_OBJ) -o $@ $(LIBS)

bacs-client: $(COMMON_OBJ) $(CLIENT_OBJ)
	$(CC) $(LIBS_DIRS) $(COMMON_OBJ) $(CLIENT_OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) -c $(CPP_FLAGS) $< -o $@

clean:
	rm -f bacs-server bacs-client *.o
