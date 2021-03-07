![CI](https://github.com/marler8997/tiny-regex-c/workflows/CI/badge.svg)

# tiny-regex-c

A small and portable [Regular Expression](https://en.wikipedia.org/wiki/Regular_expression) (regex) library written in C.

Design is inspired by Rob Pike's regex-code for the book *"Beautiful Code"* [available online here](http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html).

Supports a subset of the syntax and semantics of the Python standard library implementation (the `re`-module).

## Design goals

The main design goal of this library is to be small, correct, self contained and use few resources while retaining acceptable performance and feature completeness. Clarity of the code is also highly valued.

### Notable features and omissions

- Small code and binary size: just over 300 SLOC, ~3kb binary for x86.
- No use of dynamic memory allocation (i.e. no calls to `malloc` / `free`).
- No support for capturing groups or named capture: `(^P<name>group)` etc.
- Thorough testing : [exrex](https://github.com/asciimoo/exrex) is used to randomly generate test-cases from regex patterns, which are fed into the regex code for verification. Try `make test` to generate a few thousand tests cases yourself.
- Verification-harness for [KLEE Symbolic Execution Engine](https://klee.github.io), see [formal verification.md](https://github.com/kokke/tiny-regex-c/blob/master/formal_verification.md).
- Provides character length of matches.
- Compiled for x86 using GCC 7.2.0 and optimizing for size, the binary takes up ~2-3kb code space and allocates ~0.5kb RAM :
  ```
  > gcc -Os -c re.c
  > size re.o
      text     data     bss     dec     hex filename
      2404        0     304    2708     a94 re.o

  ```

### API

Refer to [re.h](re.h) for the current API.

### License
All material in this repository is in the public domain.
