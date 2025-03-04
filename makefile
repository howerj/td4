CFLAGS= -Wall -Wextra -pedantic -O2 -std=c99
TARGET= td4
STEPS= 32
PROG=ex.hex

.PHONY: default all test run clean

default all: ${TARGET}

run: ${TARGET} ${PROG}
	./${TARGET} hex ${PROG} ${STEPS}

%.hex: %.asm ${TARGET}
	./${TARGET} dump $< > $@

