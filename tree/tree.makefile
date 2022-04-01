test/tree: src/lang/tree/tree.o
test/tree: src/lang/tree/test/tree.test.o
test/tree: src/lang/tokenizer/tokenizer.o
test/tree: src/convert/source.o
test/tree: src/convert/fd/source.o
test/tree: src/window/alloc.o
test/tree: src/window/string.o
test/tree: src/log/log.o
test/tree: src/table/string.o
test/tree: src/range/streq.o
test/tree: src/range/string_init.o
test/tree: src/range/strdup.o

test/run-tree: src/lang/tree/test/tree.test.sh

tree-tests: test/tree
tree-tests: test/run-tree
tests: tree-tests

C_PROGRAMS += test/tree
SH_PROGRAMS += test/run-tree

depend: tree-depend
tree-depend:
	sh makedepend.sh src/lang/tree/tree.makefile

run-tests: run-tree-tests
run-tree-tests:
	DEPENDS=tree-tests sh run-tests.sh test/run-tree

# DO NOT DELETE

src/lang/tree/test/tree.test.o: src/lang/tree/tree.h src/table/string.h
src/lang/tree/test/tree.test.o: src/range/def.h src/window/def.h
src/lang/tree/test/tree.test.o: src/convert/status.h src/convert/source.h
src/lang/tree/test/tree.test.o: src/lang/error/error.h
src/lang/tree/test/tree.test.o: src/convert/fd/source.h src/window/alloc.h
src/lang/tree/test/tree.test.o: src/log/log.h
src/lang/tree/tree.o: src/lang/tree/tree.h src/table/string.h src/range/def.h
src/lang/tree/tree.o: src/window/def.h src/convert/status.h
src/lang/tree/tree.o: src/convert/source.h src/lang/error/error.h
src/lang/tree/tree.o: src/lang/tokenizer/tokenizer.h src/window/alloc.h
src/lang/tree/tree.o: src/convert/fd/source.h src/log/log.h
