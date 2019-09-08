EXTENSION = c
COMPILER  = gcc
PACKAGES  =
EXTRAINCL =
EXTRALIBS = -pthread

MAINS   = v1.c v2.c
CFLAGS  = -W -Wall -pedantic $(ALL_HDRS)
LDFLAGS = $(ALL_LIBS)

ALL_HDRS = $(foreach pack, $(PACKAGES), $(shell echo `pkg-config --cflags $(pack)`)) $(EXTRAINCL)
ALL_LIBS = $(foreach pack, $(PACKAGES), `pkg-config --libs $(pack)`) $(EXTRALIBS)

EXT     = $(EXTENSION)
CC      = $(COMPILER)
ALLSRC  = $(wildcard *.$(EXT))
SRC     = $(filter-out $(MAINS),$(ALLSRC))
OBJ     = $(SRC:.$(EXT)=.o)

.SECONDEXPANSION:
.PHONY: all clean mrproper list pkgs-avail pkgs-version help

#####
# COMPILATION TARGETS
all: pkgs-avail $(MAINS:.$(EXT)=)

$(MAINS:.$(EXT)=): $(OBJ) $$(@).o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.$(EXT)
	$(CC) -o $@ -c $< $(CFLAGS)

#####
# PHONY TARGETS
clean:
	rm -rf $(OBJ) $(MAINS:.$(EXT)=.o)

mrproper: clean
	rm -rf $(MAINS:.$(EXT)=)

list:
	@echo -e "Sources $(SRC)\nObjets  $(OBJ)\nMains   $(MAINS)"

pkgs-version:
	@for p in $(PACKAGES); do \
		echo -n "$$p : "; \
		pkg-config --modversion $$p; \
	done

pkgs-avail:
	@for p in $(PACKAGES); do \
		pkg-config --exists $$p; \
		if [ $$? -ne 0 ]; then \
			echo -en "\033[31m" > /dev/stderr; \
			echo -en "The package $$p does not exist" > /dev/stderr; \
			echo -e  "\033[0m" > /dev/stderr; \
		fi; \
	done

