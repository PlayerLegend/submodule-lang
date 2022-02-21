test/pp-list-occurances:	\
	src/lang/preprocessor/define/test/list-occurances/test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/table/string.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/range/streq.o \
	src/range/range_strdup.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/pp-set-occurances:	\
	src/lang/preprocessor/define/test/set-occurances/test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/table/string.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/range/streq.o \
	src/range/range_strdup.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/pp-lang-define-arg:	\
	src/lang/preprocessor/define/test/set-occurances/test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/table/string.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/range/streq.o \
	src/range/range_strdup.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/pp-invoke:	\
	src/lang/preprocessor/define/test/invoke/test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/table/string.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/range/streq.o \
	src/range/range_strdup.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/pp-list-occurances.run: src/lang/preprocessor/define/test/list-occurances/run.sh
test/pp-set-occurances.run: src/lang/preprocessor/define/test/set-occurances/run.sh
test/pp-lang-define-arg.run: src/lang/preprocessor/define/test/lang-define-arg/run.sh
test/pp-invoke.run: src/lang/preprocessor/define/test/invoke/run.sh

C_PROGRAMS += test/pp-list-occurances
C_PROGRAMS += test/pp-set-occurances
C_PROGRAMS += test/pp-lang-define-arg
C_PROGRAMS += test/pp-invoke

SH_PROGRAMS += test/pp-list-occurances.run
SH_PROGRAMS += test/pp-set-occurances.run
SH_PROGRAMS += test/pp-lang-define-arg.run
SH_PROGRAMS += test/pp-invoke.run

lang-preprocessor-define-tests: test/pp-list-occurances
lang-preprocessor-define-tests: test/pp-set-occurances
lang-preprocessor-define-tests: test/pp-lang-define-arg
lang-preprocessor-define-tests: test/pp-invoke
lang-preprocessor-define-tests: test/pp-list-occurances.run
lang-preprocessor-define-tests: test/pp-set-occurances.run
lang-preprocessor-define-tests: test/pp-lang-define-arg.run
lang-preprocessor-define-tests: test/pp-invoke.run
lang-preprocessor-tests: lang-preprocessor-define-tests
lang-tests: lang-preprocessor-tests
tests: lang-tests

RUN_TESTS += test/pp-list-occurances.run
RUN_TESTS += test/pp-set-occurances.run
RUN_TESTS += test/pp-lang-define-arg.run
RUN_TESTS += test/pp-invoke.run
