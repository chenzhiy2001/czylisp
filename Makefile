# FILENAME = parsing

# build:
# 	@cc -std=c99 -Wall $(FILENAME).c mpc.c -ledit -lm -o $(FILENAME)
# build-windows:
# 	@cc -std=c99 -Wall $(FILENAME).c mpc.c -o $(FILENAME)


# run:build
# 	@./$(FILENAME)

CC = cc
CFLAGS = -std=c99 -Wall -g
LFLAGS = 
FILES = czylisp
PLATFORM = $(shell uname)

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	LFLAGS += -ledit -lm
#	FILES += prompt_unix
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	LFLAGS += -ledit -lm
#	FILES += prompt_unix
endif

ifeq ($(findstring MINGW,$(PLATFORM)),MINGW)
	FILES += prompt_windows
endif

all: $(FILES)

%: %.c mpc.c
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@
  
