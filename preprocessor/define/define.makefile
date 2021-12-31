test/lang-preprocessor-define-list-occurances:	\
	src/lang/preprocessor/define/test/lang-preprocessor-define-list-occurances.test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/immutable/immutable.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/lang-preprocessor-define-list-occurances-replace:	\
	src/lang/preprocessor/define/test/lang-preprocessor-define-list-occurances-replace.test.o \
	src/convert/fd/source.o \
	src/window/alloc.o \
	src/lang/tree/tree.o \
	src/immutable/immutable.o \
	src/log/log.o \
	src/lang/preprocessor/define/define.o \
	src/lang/tokenizer/tokenizer.o \
	src/table/table.o \
	src/window/string.o \
	src/range/alloc.o \
	src/convert/source.o

test/run-lang-preprocessor-define-list-occurances: src/lang/preprocessor/define/test/run-lang-preprocessor-define-list-occurances.sh
test/run-lang-preprocessor-define-list-occurances-replace: src/lang/preprocessor/define/test/run-lang-preprocessor-define-list-occurances-replace.sh

C_PROGRAMS += test/lang-preprocessor-define-list-occurances
C_PROGRAMS += test/lang-preprocessor-define-list-occurances-replace

SH_PROGRAMS += test/run-lang-preprocessor-define-list-occurances
SH_PROGRAMS += test/run-lang-preprocessor-define-list-occurances-replace

lang-preprocessor-define-tests: test/lang-preprocessor-define-list-occurances
lang-preprocessor-define-tests: test/lang-preprocessor-define-list-occurances-replace
lang-preprocessor-define-tests: test/run-lang-preprocessor-define-list-occurances
lang-preprocessor-define-tests: test/run-lang-preprocessor-define-list-occurances-replace
lang-preprocessor-tests: lang-preprocessor-define-tests
lang-tests: lang-preprocessor-tests
tests: lang-tests

RUN_TESTS += test/run-lang-preprocessor-define-list-occurances
RUN_TESTS += test/run-lang-preprocessor-define-list-occurances-replace
