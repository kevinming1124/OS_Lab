CC := gcc
override CFLAGS += -O3 -Wall

SOURCE1 := sender.c
BINARY1 := sender

SOURCE2 := receiver.c
BINARY2 := receiver

SOURCE3 := s.c
BINARY3 := s

SOURCE4 := r.c
BINARY4 := r

all: $(BINARY1) $(BINARY2) $(BINARY3) $(BINARY4)

$(BINARY1): $(SOURCE1) $(patsubst %.c, %.h, $(SOURCE1))
	$(CC) $(CFLAGS) $< -o $@

$(BINARY2): $(SOURCE2) $(patsubst %.c, %.h, $(SOURCE2))
	$(CC) $(CFLAGS) $< -o $@

$(BINARY3): $(SOURCE3) $(patsubst %.c, %.h, $(SOURCE3))
	$(CC) $(CFLAGS) $< -o $@

$(BINARY4): $(SOURCE4) $(patsubst %.c, %.h, $(SOURCE4))
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(BINARY1) $(BINARY2) $(BINARY3) $(BINARY4)