CC = gcc 
CFLAGS = -static -Wall -Wextra -pedantic -g
TARGET = losSimonzon #nombre del binario

all: $(TARGET)

SRCS = losSimonzon.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(TARGET)

.PHONY: all clean

## gcc -Wall -Wextra -pedantic -c hola.c -o hola.o
## gcc -Wall -Wextra -pedantic hola.o -o hola

#makefile no tiene extension. se puede poner en minuscula el archivo
#  comando: el codigo debe estar en la linea de abajo identado

# ^ en compu labo es altGr+ tecla({)