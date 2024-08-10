CC = gcc
CFLAGS = -Wall -O2
LIBS = -lX11
TARGET = dwmbar
SRCS = dwmbar.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule to build object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# Install target (optional)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin

# Uninstall target (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
