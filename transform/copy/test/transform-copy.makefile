test/transform-copy:	\
	src/lang/transform/copy/test/test.o \
	src/lang/tree/tree.o \
	src/window/alloc.o \
	src/log/log.o \
	src/lang/tokenizer/tokenizer.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/table/table.o \
	src/window/string.o \
	src/lang/transform/transform.o \
	src/lang/transform/copy/copy.o \
	src/table/string.o \
	src/range/streq.o \
	src/range/range_strdup.o

test/transform-copy.run: src/lang/transform/copy/test/run.sh
lang-transform-tests: test/transform-copy
lang-transform-tests: test/transform-copy.run
lang-tests: lang-transform-tests
tests: lang-tests

C_PROGRAMS += test/transform-copy
SH_PROGRAMS += test/transform-copy.run

RUN_TESTS += test/transform-copy.run
