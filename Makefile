TARGET_ASTAR = astar
TARGET_UCS = ucs

SRC_ASTAR = astar.c
SRC_UCS = ucs.c
SRC = pq.c
HDR = pq.h

OBJ := $(SRC:.c=.o)
OBJ_ASTAR = $(SRC_ASTAR:.c=.o)
OBJ_UCS = $(SRC_UCS:.c=.o)

CC = gcc
CFLAGS = -W -Wall -ansi -g -O0 -D_XOPEN_SOURCE=500
LDFLAGS = -lm -lcurses

all : $(TARGET_ASTAR) $(TARGET_UCS)

$(TARGET_ASTAR) : $(OBJ) $(OBJ_ASTAR)
	$(CC) -o $@ $(OBJ) $(OBJ_ASTAR) $(LDFLAGS)

$(TARGET_UCS) : $(OBJ) $(OBJ_UCS)
	$(CC) -o $@ $(OBJ) $(OBJ_UCS) $(LDFLAGS)

depend : $(SRC) $(SRC_ASTAR) $(SRC_UCS) $(HDR)
	$(CC) $(CFLAGS) -MM $(SRC) $(SRC_ASTAR) $(SRC_UCS) > depend

-include depend

.PHONY : clean

clean :
	-rm -f depend
	-rm -f $(TARGET_ASTAR) $(TARGET_UCS) $(OBJ) $(OBJ_ASTAR) $(OBJ_UCS)
	-rm -f *~
