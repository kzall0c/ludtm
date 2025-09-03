CC = gcc
TGT = ludtm
OBJS = ludtm

all: $(TGT)

$(TGT): $(OBJS)
	$(CC) -w -pthread -ggdb -O0 ludtm.c -o $@ $^

test: $(TGT)
	./$(TGT)

list:
	 $(CC) -ggdb -O0 list.c -o list

hashmap:
	 $(CC) -ggdb -O0 hashmap.c -o hashmap

lru:
	 $(CC) -ggdb -O0 lru.c -o lru

rbtree:
	 $(CC) -ggdb -O0 rbtree.c -o rbtree

btree:
	 $(CC) -ggdb -O0 btree.c -o btree

.PHONY: clean
clean:
	@rm -rf $(OBJS) $(TGT)
