# Compiler to use - can be replaced by clang for instance
CC := gcc

# Number of random text expressions to generate, for random testing
NRAND_TESTS := 1000

PYTHON != if (python --version 2>&1 | grep -q 'Python 2\..*'); then \
            echo 'python';                                          \
          elif command -v python2 >/dev/null 2>&1; then             \
            echo 'python2';                                         \
          else                                                      \
            echo 'Error: no compatible python version found.' >&2;  \
            exit 1;                                                 \
          fi

# Flags to pass to compiler
CFLAGS := -O3 -Wall -Wextra -std=c99 -I.

all: bin/test1 bin/test1_cstr bin/test3 bin/test3_cstr bin/test2 bin/test2_cstr bin/test_rand bin/test_rand_neg

bin/%: tests/%.c re.c re.h re_deprecated.h
	mkdir -p bin
	$(CC) $(CFLAGS) -DRE_DOT_NO_NEWLINE $< re.c -o $@

bin/%_cstr: tests/%.c re.c re.h re_deprecated.h
	mkdir -p bin
	$(CC) $(CFLAGS) -DRE_DOT_NO_NEWLINE -DRE_CSTRINGS $< re.c -o $@

bin/%_newline: tests/%.c re.c re.h re_deprecated.h
	mkdir -p bin
	$(CC) $(CFLAGS) $< re.c -o $@

bin/%_size_t: tests/%.c re.c re.h re_deprecated.h
	mkdir -p bin
	$(CC) $(CFLAGS) -DRE_DOT_NO_NEWLINE -DRE_USE_SIZE_T $< re.c -o $@

clean:
	rm -rf bin

test1: bin/test1
	./bin/test1

test1_cstr: bin/test1_cstr
	./bin/test1_cstr

test3: bin/test3
	./bin/test3

test3_cstr: bin/test3_cstr
	./bin/test3_cstr

test3_newline: bin/test3_newline
	./bin/test3_newline

test3_size_t: bin/test3_size_t
	./bin/test3_size_t

test2: bin/test2
	./bin/test2

test2_cstr: bin/test2_cstr
	./bin/test2_cstr

test_rand: bin/test_rand
	@$(test $(PYTHON))
	@echo
	@echo Testing patterns against $(NRAND_TESTS) random strings matching the Python implementation and comparing:
	@echo
	$(PYTHON) ./scripts/run_random_tests scripts/regex_test.py tests/rand_patterns $(NRAND_TESTS)

test_rand_neg: bin/test_rand_neg
	@$(test $(PYTHON))
	@echo
	@echo Testing rejection of patterns against $(NRAND_TESTS) random strings also rejected by the Python implementation:
	@echo
	$(PYTHON) ./scripts/run_random_tests scripts/regex_test_neg.py tests/rand_neg_patterns $(NRAND_TESTS)

test: test1 test1_cstr test3 test3_cstr test3_newline test3_size_t test2 test_rand test_rand_neg

test_all_make_targets:
	./scripts/test_all_make_targets
