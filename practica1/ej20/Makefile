CC = gcc 
CFLAGS = -static -Wall -Wextra -pedantic -g

TARGET_SERVIDOR = servidorEj20
TARGET_CLIENTE1 = cliente1
TARGET_CLIENTE2 = cliente2

all: servidor hacer_cliente1 hacer_cliente2

# ---- SERVIDOR ----
servidor: $(TARGET_SERVIDOR)

$(TARGET_SERVIDOR): servidor.o
	$(CC) $(CFLAGS) $^ -o $@

# ---- CLIENTE 1 ----
hacer_cliente1: $(TARGET_CLIENTE1)

$(TARGET_CLIENTE1): cliente1.o
	$(CC) $(CFLAGS) $^ -o $@

# ---- CLIENTE 2 ----
hacer_cliente2: $(TARGET_CLIENTE2)

$(TARGET_CLIENTE2): cliente2.o
	$(CC) $(CFLAGS) $^ -o $@

# ---- REGLA GENERAL ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET_SERVIDOR) $(TARGET_CLIENTE1) $(TARGET_CLIENTE2)

.PHONY: all clean servidor cliente1 cliente2