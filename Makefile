CC_ARGS=-Wall -Werror
OBJECT=main.o

all: $(OBJECT)
	$(CC) $(CC_ARGS) $(OBJECT) -o tarot

.c.o: 
	$(CC) $(CC_ARGS) -c $^ -o $@

clean: 
	-rm a.out
	-rm main
	-rm tarot
	-rm $(OBJECT)

