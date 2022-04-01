test/tokenizer: src/log/log.o
test/tokenizer: src/lang/tokenizer/tokenizer.o
test/tokenizer: src/lang/tokenizer/test/tokenizer.test.o
test/tokenizer: src/window/alloc.o
test/tokenizer: src/convert/source.o
test/tokenizer: src/convert/fd/source.o

test/run-tokenizer: src/lang/tokenizer/test/tokenizer.test.sh

tokenizer-tests: test/tokenizer
tokenizer-tests: test/run-tokenizer
tests: tokenizer-tests

C_PROGRAMS += test/tokenizer
SH_PROGRAMS += test/run-tokenizer

run-tests: run-tokenizer-tests
run-tokenizer-tests:
	DEPENDS=tokenizer-tests sh run-tests.sh test/run-tokenizer

depend: tokenizer-depend
tokenizer-depend:
	sh makedepend.sh src/lang/tokenizer/tokenizer.makefile

# DO NOT DELETE

src/lang/tokenizer/tokenizer.o: src/lang/tokenizer/tokenizer.h
src/lang/tokenizer/tokenizer.o: src/convert/source.h src/range/def.h
src/lang/tokenizer/tokenizer.o: src/window/def.h src/convert/status.h
src/lang/tokenizer/tokenizer.o: src/lang/error/error.h
src/lang/tokenizer/test/tokenizer.test.o: src/lang/tokenizer/tokenizer.h
src/lang/tokenizer/test/tokenizer.test.o: src/convert/source.h
src/lang/tokenizer/test/tokenizer.test.o: src/range/def.h src/window/def.h
src/lang/tokenizer/test/tokenizer.test.o: src/convert/status.h
src/lang/tokenizer/test/tokenizer.test.o: src/lang/error/error.h
src/lang/tokenizer/test/tokenizer.test.o: src/convert/fd/source.h
src/lang/tokenizer/test/tokenizer.test.o: src/window/alloc.h src/log/log.h
