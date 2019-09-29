
.PHONY: clean distclean depend 

include rules.mk

CFLAGS=-I src -I src/export

libidmgr.a : src/chunk_mgr.o src/avl_tree.o src/id_mgr.o
	ar r libidmgr.a $^

unittest: libidmgr.a test/chunk_mgr_test.o test/avl_test.o test/id_mgr_test.o
	gcc -lm -o chunk_mgr_test.bin test/chunk_mgr_test.o libidmgr.a
	gcc -lm -o avl_test.bin test/avl_test.o libidmgr.a
	gcc -lm -o idmgr_test.bin test/id_mgr_test.o libidmgr.a

demo: example/demo_main.o libidmgr.a
	gcc -lm -o demo.bin $^

clean:
	rm -f test/*.o test/*.d src/*.o src/*.d *.bin *.a core.*

