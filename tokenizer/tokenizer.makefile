test/tokenizer: src/log/log.o
test/tokenizer: src/lang/tokenizer/tokenizer.o
test/tokenizer: src/lang/tokenizer/test/tokenizer.test.o
test/tokenizer: src/window/alloc.o
test/tokenizer: src/convert/def.o
test/tokenizer: src/convert/fd.o

test/run-tokenizer: src/lang/tokenizer/test/tokenizer.test.sh

tokenizer-tests: test/tokenizer
tokenizer-tests: test/run-tokenizer
tests: tokenizer-tests

C_PROGRAMS += test/tokenizer
SH_PROGRAMS += test/run-tokenizer
RUN_TESTS += test/run-tokenizer
