test/tree: src/lang/tree/tree.o
test/tree: src/lang/tree/test/tree.test.o
test/tree: src/lang/tokenizer/tokenizer.o
test/tree: src/convert/source.o
test/tree: src/convert/fd/source.o
test/tree: src/window/alloc.o
test/tree: src/window/string.o
test/tree: src/log/log.o
test/tree: src/table/table.o
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

RUN_TESTS += test/run-tree
