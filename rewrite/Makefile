PROG_NAME = temple

FILES := $(wildcard *.c)
OBJECTS = $(patsubst %.c, build/%.o, $(FILES))

DEPENDENCIES = libh
CFLAGS := -I$(DEPENDENCIES)
LDFLAGS := -L./$(DEPENDENCIES)/build -l:$(DEPENDENCIES).a

build/$(PROG_NAME): $(OBJECTS)
	$(MAKE) -C $(DEPENDENCIES) # build libh dependencies before final target
	$(CC) -o $@ $^ $(LDFLAGS)

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -o $@ $< -c $(CFLAGS)

clean:
	rm -r build
