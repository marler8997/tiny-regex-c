//
// Support the old re_match function interface so I don't have to change
// how the tests call it yet.
//
#include <assert.h>

#ifdef RE_CSTRINGS
  #define RE_MATCH re_match_cstr
  #define text_args(t) t
#else
  #define RE_MATCH re_match
  #define text_args(t) t, text_cstr_end(t)
  static inline const char *text_cstr_end(const char *cstr) {
    const char *end = cstr;
    for (; end[0]; end++) { }
    return end;
  }
#endif

static inline int re_match_deprecated(const char* regex, const char* text, int* matchlength)
{
  struct re_context context = {0};
  re_length_t match_start;
  int result = RE_MATCH(&context, regex, text_args(text), &match_start);
  assert(context.error == 0);
  if (result) {
    *matchlength = (int)context.match_length;
    return (int)match_start;
  }
  return -1;
}
