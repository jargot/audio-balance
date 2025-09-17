CC = gcc
CFLAGS = -Wall -Wextra -O2
FRAMEWORKS = -framework CoreAudio -framework AudioToolbox -framework CoreFoundation
TARGET = audio-balance
SOURCE = src/set_balance.c
OUTDIR = out

$(TARGET): $(SOURCE)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(TARGET) $(SOURCE) $(FRAMEWORKS)

install: $(TARGET)
	cp $(OUTDIR)/$(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

clean:
	rm -f $(OUTDIR)/$(TARGET)
	rmdir $(OUTDIR) 2>/dev/null || true

.PHONY: install uninstall clean