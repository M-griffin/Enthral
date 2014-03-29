#CC = g++ -march=x86_64 -D__LINUX__ -Wall
CC = g++ -D__LINUX__ -Wall
AR = ar -ru
RM = rm -f

OBJS = crc32.o mbase.o message.o lastread.o subpack.o structrw.o

jamlib.a : $(OBJS) 
	$(AR) jamlib.a $(OBJS)

crc32.o:    crc32.cpp jam.h
	$(CC) -c crc32.cpp -o crc32.o

mbase.o:    mbase.cpp jam.h
	$(CC) -c mbase.cpp -o mbase.o

message.o:  message.cpp jam.h
	$(CC) -c message.cpp -o message.o

lastread.o: lastread.cpp jam.h
	$(CC) -c lastread.cpp -o lastread.o

subpack.o:  subpack.cpp jam.h
	$(CC) -c subpack.cpp -o subpack.o

structrw.o:  structrw.cpp jam.h
	$(CC) -c structrw.cpp -o structrw.o

clean:
	$(RM) *.o *.a
