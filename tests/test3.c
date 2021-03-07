#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "re.h"

#ifdef RE_CSTRINGS
  #define RE_MATCH_START_ONLY re_match_start_only_cstr
  #define text_args(t) t
#else
  #define RE_MATCH_START_ONLY re_match_start_only
  #define text_args(t) t, t + strlen(t)
#endif

void test(const char *regex, const char *text, int expected_match_len)
{
  printf("Pattern '%s' Text '%s'\n", regex, text);
  struct re_context context = {0};

  if (RE_MATCH_START_ONLY(&context, regex, text_args(text))) {
    if (expected_match_len == -1) {
      printf("fail: expected not to match\n");
      exit(1);
    }
    if ((re_length_t)expected_match_len != context.match_length) {
      printf("fail: expected match length %d but got %llu\n", expected_match_len, (unsigned long long)context.match_length);
      exit(1);
    }
  } else {
    if (expected_match_len != -1) {
      printf("fail: expected a match\n");
      exit(1);
    }
  }
  if (context.error) {
    printf("fail: unexpected error %s\n", re_error_name_table[context.error]);
    exit(1);
  }
}

void testbad(const char *regex, const char *text, enum re_error error)
{
  printf("BadPattern '%s' Text '%s'\n", regex, text);
  struct re_context context = {0};

  if (RE_MATCH_START_ONLY(&context, regex, text_args(text))) {
    printf("fail: expected a bad regex error but got a match");
    exit(1);
  }
  if (context.error != error) {
    printf("fail: expected error %s but got %s\n", re_error_name_table[error], re_error_name_table[context.error]);
    exit(1);
  }
}

int main()
{
  test("", "", 0);
  test("a", "", -1);
  test("a", "a", 1);
  test("a", "b", -1);

  test("a?", "a", 0); // match length is 0 because '?' is non-greedy
  test("a?", "b", 0);

  // part (1) of https://github.com/kokke/tiny-regex-c/issues/56
  test("a?", "", 0);
  test("a*", "", 0);

  test("a?b", "", -1);
  test("a?b", "a", -1);
  test("a?b", "b", 1);
  test("a?b", "ab", 2);

  //test("\\0", "\0", 1);
  test("\\S", "\t \n", -1);
  test("\\?", "?", 1);
  test("\\w*\\d?\\w\\?", "GftH9V0AX9e?", 12);

  test("[a-b]", "a", 1);
  test("[a-b]", "!", -1);

  test("[,-0]", "/",  1);
  test("[,-0]", "a", -1);
  test("[,-0]", "-",  1);
  test("[,-0]", "a", -1);
  test("[--0]", "-",  1);
  test("[--0]", "a", -1);

  test("[\\-]*", "", 0);
  test("[\\-]*", "-", 1);

  test("]", "]", 1);
  test("]", "[", -1);
  testbad("[\\]", "", RE_ERROR_MISSING_RIGHT_SQUARE_BRACKET);
  test("[\\]]", "]", 1);
  test("[\\]]", "a", -1);
  test("[\\\\]]", "\\]", 2);
  test("[\\\\]]", "a", -1);
  test("[\\\\]", "\\", 1);
  test("[\\\\]", "a", -1);
  test("[\\\\\\]]", "\\", 1);
  test("[\\\\\\]]", "]", 1);
  test("[\\\\\\]]", "\\]", 1);
  test("[\\\\\\]]", "]\\", 1);
  test("[\\\\\\]]", "a", -1);
  testbad("[\\\\\\]", "", RE_ERROR_MISSING_RIGHT_SQUARE_BRACKET);
  test("[\\\\\\\\]", "\\", 1);

  test("[\\\\]*", "", 0);
  test("[\\\\]*", "\\", 1);

  test("[-1-2]*", "", 0);
  test("[\\-]*", "", 0);
  test("[\\-]*", "-", 1);
  test("[\\-]*", "a", 0);

  testbad("\\", "", RE_ERROR_ENDS_WITH_BACKSLASH);
  testbad("foo\\", "foo", RE_ERROR_ENDS_WITH_BACKSLASH);
  testbad("[", "", RE_ERROR_MISSING_RIGHT_SQUARE_BRACKET);
  testbad("[abc", "", RE_ERROR_MISSING_RIGHT_SQUARE_BRACKET);
  testbad("|", "", RE_ERROR_BRANCH_NOT_IMPLEMENTED);

  const char *special_chars = "\\*+?.$^()[]{}-|";
  const char *char_classes = "sSwWdD";
  const char *escape_chars = "nt";
  const char *escape_char_map = "\n\t";
  for (int i = 1; i <= 255; i++) {
    char regex_c = (char)i;

    char regex_buf[10];
    char text_buf[10];
    int is_special = (strchr(special_chars, regex_c) != NULL);
    int is_char_class = (strchr(char_classes, regex_c) != NULL);
    const char *escape_strchr = strchr(escape_chars, regex_c);
    int is_escape = (escape_strchr != NULL);

    if (is_special || is_escape) {
      char escape_char = regex_c;
      if (is_escape) {
        escape_char = escape_char_map[escape_strchr - escape_chars];
      }

      sprintf(text_buf, "%c", escape_char);
      if (escape_char == '\n' || escape_char == '\t') {
        test("\\s", text_buf, 1);
        test("\\S", text_buf, -1);
      } else {
        test("\\s", text_buf, -1);
        test("\\S", text_buf, 1);
      }
      test("\\d", text_buf, -1);
      test("\\D", text_buf, 1);
      test("\\w", text_buf, -1);
      test("\\W", text_buf, 1);

      sprintf(regex_buf, "\\%c", regex_c);
      test(regex_buf, text_buf, 1);
      test(regex_buf, "a", -1);

      sprintf(regex_buf, "[\\%c]", regex_c);
      test(regex_buf, text_buf, 1);
      test(regex_buf, "a", -1);

    } else if (is_char_class) {
      /// TODO:
    } else {
      sprintf(regex_buf, "\\%c", i);
      testbad(regex_buf, "a", RE_ERROR_BAD_ESCAPE);
      sprintf(regex_buf, "%c", regex_c);
      sprintf(text_buf, "%c", regex_c);
      test(regex_buf, text_buf, 1);
      sprintf(regex_buf, "[%c]", regex_c);
      test(regex_buf, text_buf, 1);
      test(regex_buf, "*", -1);
      sprintf(regex_buf, "[%c]*", regex_c);
      test(regex_buf, text_buf, 1);
      test(regex_buf, "*", 0);
      sprintf(regex_buf, "[\\%c]", regex_c);
      testbad(regex_buf, "a", RE_ERROR_BAD_ESCAPE);
    }
  }

#ifdef RE_DOT_NO_NEWLINE
  printf("RE_DOT_NO_NEWLINE is TRUE\n");
  test("[a-z].[A-Z]", "a\nB", -1);
#else
  printf("RE_DOT_NO_NEWLINE is FALSE\n");
  test("[a-z].[A-Z]", "a\nB", 3);
#endif


  // TODO: add testing with the \0 character, I'll need to use limit strings

  // https://github.com/kokke/tiny-regex-c/issues/53
  test(".*a.*a", "aa", 2);
}
