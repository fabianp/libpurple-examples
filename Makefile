# 
# Makefile for libpurple-examples
# 
# set extra flags if needed
CFLAGS  = -g $(shell pkg-config --cflags purple)
LDFLAGS = $(shell pkg-config --libs purple gthread-2.0)

all: buddy_status

help:
	@ echo "\nUsage: make <target>,  where <target> is one of:"
	@ echo " "
	@ echo "    buddy_status    --  create the buddy_status example."
	@ echo " "
	@ echo "    all              --  creates all executables."
	@ echo " "
	@ echo "    clean            --  erase all the .o and excutable files" 
	@ echo " "


SRC		:= buddy_status.c send_file.c
OBJ 	:= $(patsubst %.c, %.o, $(SRC))
TARGET	:= buddy_status send_file

all: $(TARGET)

% : %.c common.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	$(RM) -rf *.o $(OBJ) $(TARGET) *.dSYM buddy_status
