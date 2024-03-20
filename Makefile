# CC = gcc
# CFLAGS = -Wall -Wextra -Iinclude
# LDFLAGS =

# SRCDIR = ./src
# OBJDIR = ./obj
# BINDIR = ./bin
# TESTDIR = ./tests

# SRCFILES := $(wildcard $(SRCDIR)/*.c)
# OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

# USER_SRC := $(TESTDIR)/user.c
# USER_OBJ := $(OBJDIR)/user.o

# EXECUTABLE = $(BINDIR)/user

# all: $(BINDIR) $(EXECUTABLE)

# $(EXECUTABLE): $(OBJFILES) $(USER_OBJ)
# 	$(CC) $(LDFLAGS) $^ -o $@

# $(OBJDIR)/%.o: $(SRCDIR)/%.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(OBJDIR)/user.o: $(USER_SRC)
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(OBJDIR):
# 	mkdir -p $(OBJDIR)

# $(BINDIR):
# 	mkdir -p $(BINDIR)

# .PHONY: clean
# clean:
# 	rm -rf $(OBJDIR) $(BINDIR)

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS =

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin
TESTDIR = ./tests

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

USER_SRC := $(TESTDIR)/user.c
USER_OBJ := $(OBJDIR)/user.o

EXECUTABLE = $(BINDIR)/user

all: $(BINDIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJFILES) $(USER_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/user.o: $(USER_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/tuple_space.o: $(SRCDIR)/node/tuple_space.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

