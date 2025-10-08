# Python + Flask makefile

VENV = .venv
VENV_PYTHON3 = $(VENV)/bin/python3

ADMIN = roberto.lotan:b0630b86f0b5
PROGRAM = ./client

CC = gcc
CFLAGS = -Wall -g
DEPS = helpers.h requests.h parson.h
OBJ = client.o helpers.o requests.o parson.o buffer.o

all: venv deps client

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

venv: $(VENV_PYTHON3)
$(VENV_PYTHON3):
	python3 -m venv "$(VENV)"

deps: venv
	$(VENV_PYTHON3) -m pip install -r requirements.txt

A ?= --debug --admin "$(ADMIN)"
run:
	$(VENV_PYTHON3) checker.py $(PROGRAM) $(A)

clean:
	rm -f *.o client
