#ifndef EXERCISE_II__TOKENIZER_H_
#define EXERCISE_II__TOKENIZER_H_

#include <cstddef>
struct Tokenizer {
  explicit Tokenizer(char *stream, size_t length, char delimiter = ' ', size_t index = 0U);
  bool has_next();
  char *next_token();
  size_t remaining_tokens();
 private:
  char *stream_;
  size_t length_;
  size_t index_;
  char delimiter_;
};

#endif //EXERCISE_II__TOKENIZER_H_
