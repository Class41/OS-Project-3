CC = gcc
CFLAGS = -g
TARGET1 = master
TARGET2 = palin
OBJS1 = master.o
OBJS2 = palin.o

all: master palin
$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1)
$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)
master.o: master.c
	$(CC) $(CFLAGS) -c master.c shared.h
palin.o: palin.c
	$(CC) $(CFLAGS) -c palin.c shared.h
clean:
	/bin/rm -f *.o $(TARGET1) $(TARGET2)