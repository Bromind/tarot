CC_FLAGS=`pkg-config --cflags gtk+-3.0`
CC_LIBS=`pkg-config --libs gtk+-3.0`
CC_ARGS=-Wall -Werror -g
OBJECT=partie.o gui.o

all: $(OBJECT)
	$(CC) $(CC_ARGS) $(OBJECT) -o tarot $(CC_LIBS)

.c.o: 
	$(CC) $(CC_FLAGS) $(CC_ARGS) -c $^ -o $@

clean: 
	-rm a.out
	-rm main
	-rm tarot
	-rm $(OBJECT)

