src = $(wildcard src/*.c)
headers = $(wildcard src/*.h)
rstdobj = $(wildcard rstd/bin/*.o)
out = bin/chip8.out

CFLAGS = -gdwarf-2 -lSDL2 -lm

all: $(out)

$(out):
	$(CC) $(CFLAGS) $(src) $(rstdobj) -Isrc -Irstd/src -o $@

clean:
	rm -f $(out)
