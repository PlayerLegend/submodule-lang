test/preprocessor-include:	\
	src/lang/preprocessor/include/test/test.o \
	src/lang/tree/tree.o \
	src/table/string.o \
	src/window/alloc.o \
	src/log/log.o \
	src/lang/tokenizer/tokenizer.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/table/table.o \
	src/window/string.o \
	src/lang/transform/transform.o \
	src/lang/preprocessor/file/file.o \
	src/lang/preprocessor/include/include.o \
	src/range/string_init.o \
	src/range/path.o \
	src/range/range_strdup.o \
	src/range/range_streq_string.o \
	src/range/streq.o \
	src/range/string_tokenize.o \
	src/range/strchr.o \
	src/range/alloc.o \
	src/window/path.o

test/preprocessor-include.run: src/lang/preprocessor/include/test/run.sh
lang-preprocessor-tests: test/preprocessor-include
lang-preprocessor-tests: test/preprocessor-include.run
lang-tests: lang-preprocessor-tests
tests: lang-tests

C_PROGRAMS += test/preprocessor-include
SH_PROGRAMS += test/preprocessor-include.run

#RUN_TESTS += test/preprocessor-include.run
