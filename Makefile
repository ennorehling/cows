CFLAGS = -g -Wall -Werror -Wextra
PROGRAMS = hello

ifeq "$(CC)" "clang"
CFLAGS += -Wno-padded -Wno-sign-conversion
endif

all: $(PROGRAMS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)
        
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)
                
hello: hello.o
	$(CC) $(CFLAGS) -o $@ $^ -lfcgi $(LDFLAGS)

clean:
	rm -rf $(PROGRAMS) *.o *~ *.a
