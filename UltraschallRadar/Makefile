#
# Makefile:
#DEBUG	= -g -O0
#DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	=  -Wall $(INCLUDE) -Winline -pipe #$(DEBUG)

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm ./temperatur.c ./smbus.c `allegro-config --cflags --libs` -lrt



SRC	=	alegroUltraschall.c \
                allegroUltraschallRealTime.c \
		aleg.c \
		aleg2.c
		

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	$(BINS)


ultraschall3:	ultraschall3.o
	@echo [link]
	@$(CC) -o $@ ultraschall3.o $(LDFLAGS) $(LDLIBS)
	

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
