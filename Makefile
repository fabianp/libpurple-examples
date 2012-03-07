# 
# Makefile for libpurple-examples
# 
# Default variables. Overwrite them to fit your needs.
# Define VERBOSE=1 to have a more verbose compile.
#

# set extra flags if needed
CFLAGS   = -g
LDFLAGS  = 

# end of customization
CFLAGS  += $(shell pkg-config --cflags purple)
LDFLAGS += $(shell pkg-config --libs purple gthread-2.0)

ifndef VERBOSE
	QUIET_CC       = @echo '   ' CC $@;
	QUIET_CXX      = @echo '   ' CXX $@;
	QUIET_AR       = @echo '   ' AR $@;
	QUIET_LINK     = @echo '   ' LINK $@;
endif

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
