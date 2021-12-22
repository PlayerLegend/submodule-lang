test/tree: src/lang/tree/tree.o
test/tree: src/lang/tree/test/tree.test.o
test/tree: src/lang/tokenizer/tokenizer.o
test/tree: src/convert/def.o
test/tree: src/convert/fd.o
test/tree: src/window/alloc.o
test/tree: src/window/string.o
test/tree: src/log/log.o
test/tree: src/immutable/immutable.o
test/tree: src/table/table.o

test/run-tree: src/lang/tree/test/tree.test.sh

tree-tests: test/tree
tree-tests: test/run-tree
tests: tree-tests

C_PROGRAMS += test/tree
SH_PROGRAMS += test/run-tree
