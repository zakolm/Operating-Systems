# Компилятор
СС := gcc

# Опции компиляции
CFLAGS := -std=c99 -Wall -Werror -pedantic

# Ощие объектные файлы
OBJS := listing_2_3.o err_log.o

ifeq ($(mode), debug)
	CFLAGS += -g3 -ggdb -O0 -fprofile-arcs -ftest-coverage
endif

ifeq ($(mode), release)
	CFLAGS += -DNDEBUG -g0
endif

lab_02.out : $(OBJS) listing_4_7.o
	$(CC) $(CFLAGS) $^ -o $@

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

clean :
	$(RM) *.o *.out
