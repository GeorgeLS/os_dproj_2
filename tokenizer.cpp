#include "tokenizer.h"

Tokenizer::Tokenizer(char *stream, size_t length, char delimiter, size_t index)
: stream_{stream}
, length_{length}
, delimiter_{delimiter}
, index_{index}
{
}

bool Tokenizer::has_next() { return index_ != length_; }

char *Tokenizer::next_token() {
  size_t old_pos = index_;
  size_t current_index = index_;
  while (current_index != length_ and stream_[current_index] != delimiter_) {
    ++current_index;
  }

  size_t delimiter_offset = 0U;
  while (current_index != length_ and stream_[current_index] == delimiter_) {
    ++current_index;
    ++delimiter_offset;
  }

  stream_[current_index - delimiter_offset] = '\0';
  index_ = current_index;
  return &stream_[old_pos];
}

size_t Tokenizer::remaining_tokens() {
  if (index_ == length_)
    return 0U;
  size_t current_index = index_;
  size_t remaining_tokens = stream_[length_ - 1U] == delimiter_ ? 0U : 1U;
  while (current_index != length_) {
    if (stream_[current_index] == delimiter_) {
      ++remaining_tokens;

      while (current_index != length_ and stream_[current_index] == delimiter_) {
        ++current_index;
      }
    } else {
      ++current_index;
    }
  }
  return remaining_tokens;
}