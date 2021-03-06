# Sriram Balasubramaniam
# Charm Patel
# Karthik Shankar
# Brian Stebar II 

# ECE 6102 - Spring 2014
# Term Project - Distributed File System

CC        = gcc
INC_DIRS  =
LIBS_DIRS =
CPP_FLAGS = -Wall -pedantic
LIBS      = -luuid -lm

COMMON_SRC = common.c crc32.c die_with_error.c errors.c messages.c UDPclient.c UDPserver.c
COMMON_OBJ = $(patsubst %.c,%.o,$(COMMON_SRC))
SERVER_SRC = bacs_server.c blocks.c file_metadata.c server_functions.c
SERVER_OBJ = $(patsubst %.c,%.o,$(SERVER_SRC))
CLIENT_SRC = bacs_client.c send_file.c
CLIENT_OBJ = $(patsubst %.c,%.o,$(CLIENT_SRC))

EXECS = bacs_client bacs_server

all: $(EXECS)

bacs_server: $(COMMON_OBJ) $(SERVER_OBJ)
	$(CC) $(LIBS_DIRS) $(COMMON_OBJ) $(SERVER_OBJ) -o $@ $(LIBS)

bacs_client: $(COMMON_OBJ) $(CLIENT_OBJ)
	$(CC) $(LIBS_DIRS) $(COMMON_OBJ) $(CLIENT_OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) -c $(CPP_FLAGS) $< -o $@ $(LIBS)

clean:
	rm -f $(EXECS) *.o
	rm -rf bacs_data
