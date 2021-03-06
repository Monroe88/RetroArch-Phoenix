#ifndef NALL_STRING_UTILITY_HPP
#define NALL_STRING_UTILITY_HPP

namespace nall {

template<bool Insensitive>
bool chrequal(char x, char y) {
  if(Insensitive) return chrlower(x) == chrlower(y);
  return x == y;
}

template<bool Quoted, typename T>
bool quoteskip(T *&p) {
  if(Quoted == false) return false;
  if(*p != '\'' && *p != '\"') return false;

  while(*p == '\'' || *p == '\"') {
    char x = *p++;
    while(*p && *p++ != x);
  }
  return true;
}

template<bool Quoted, typename T>
bool quotecopy(char *&t, T *&p) {
  if(Quoted == false) return false;
  if(*p != '\'' && *p != '\"') return false;

  while(*p == '\'' || *p == '\"') {
    char x = *p++;
    *t++ = x;
    while(*p && *p != x) *t++ = *p++;
    *t++ = *p++;
  }
  return true;
}

unsigned strlcpy(string &dest, const char *src, unsigned length) {
  dest.reserve(length);
  return strlcpy(dest(), src, length);
}

unsigned strlcat(string &dest, const char *src, unsigned length) {
  dest.reserve(length);
  return strlcat(dest(), src, length);
}

string substr(const char *src, unsigned start, unsigned length) {
  string dest;
  if(length == ~0u) {
    //copy entire string
    dest = src + start;
  } else {
    //copy partial string
    strlcpy(dest, src + start, length + 1);
  }
  return dest;
}

string sha256(const uint8_t *data, unsigned size) {
  sha256_ctx sha;
  uint8_t hash[32];
  sha256_init(&sha);
  sha256_chunk(&sha, data, size);
  sha256_final(&sha);
  sha256_hash(&sha, hash);
  string result;
  foreach(byte, hash) result.append(hex<2>(byte));
  return result;
}

/* arithmetic <> string */

template<unsigned length_, char padding> string integer(intmax_t value) {
  bool negative = value < 0;
  if(negative) value = abs(value);

  char buffer[64];
  unsigned size = 0;

  do {
    unsigned n = value % 10;
    buffer[size++] = '0' + n;
    value /= 10;
  } while(value);
  buffer[size++] = negative ? '-' : '+';
  buffer[size] = 0;

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = length - 1, y = 0; x >= 0 && y < size; x--, y++) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

template<unsigned length_, char padding> string linteger(intmax_t value) {
  bool negative = value < 0;
  if(negative) value = abs(value);

  char buffer[64];
  unsigned size = 0;

  do {
    unsigned n = value % 10;
    buffer[size++] = '0' + n;
    value /= 10;
  } while(value);
  buffer[size++] = negative ? '-' : '+';
  buffer[size] = 0;

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = 0, y = size - 1; x < length && y >= 0; x++, y--) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

template<unsigned length_, char padding> string decimal(uintmax_t value) {
  char buffer[64];
  unsigned size = 0;

  do {
    unsigned n = value % 10;
    buffer[size++] = '0' + n;
    value /= 10;
  } while(value);
  buffer[size] = 0;

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = length - 1, y = 0; x >= 0 && y < size; x--, y++) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

template<unsigned length_, char padding> string ldecimal(uintmax_t value) {
  char buffer[64];
  unsigned size = 0;

  do {
    unsigned n = value % 10;
    buffer[size++] = '0' + n;
    value /= 10;
  } while(value);
  buffer[size] = 0;

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = 0, y = size - 1; x < length && y >= 0; x++, y--) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

template<unsigned length_, char padding> string hex(uintmax_t value) {
  char buffer[64];
  unsigned size = 0;

  do {
    unsigned n = value & 15;
    buffer[size++] = n < 10 ? '0' + n : 'a' + n - 10;
    value >>= 4;
  } while(value);

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = length - 1, y = 0; x >= 0 && y < size; x--, y++) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

template<unsigned length_, char padding> string binary(uintmax_t value) {
  char buffer[256];
  unsigned size = 0;

  do {
    unsigned n = value & 1;
    buffer[size++] = '0' + n;
    value >>= 1;
  } while(value);

  unsigned length = (length_ == 0 ? size : length_);
  char result[length + 1];
  memset(result, padding, length);
  result[length] = 0;

  for(signed x = length - 1, y = 0; x >= 0 && y < size; x--, y++) {
    result[x] = buffer[y];
  }

  return (const char*)result;
}

//using sprintf is certainly not the most ideal method to convert
//a double to a string ... but attempting to parse a double by
//hand, digit-by-digit, results in subtle rounding errors.
unsigned fp(char *str, double value) {
  char buffer[256];
  sprintf(buffer, "%f", (float)value);

  //remove excess 0's in fraction (2.500000 -> 2.5)
  for(char *p = buffer; *p; p++) {
    if(*p == '.') {
      char *p = buffer + strlen(buffer) - 1;
      while(*p == '0') {
        if(*(p - 1) != '.') *p = 0;  //... but not for eg 1.0 -> 1.
        p--;
      }
      break;
    }
  }

  unsigned length = strlen(buffer);
  if(str) strcpy(str, buffer);
  return length + 1;
}

string fp(double value) {
  string temp;
  temp.reserve(fp(0, value));
  fp(temp(), value);
  return temp;
}

string fp(double value, unsigned precision) {
  char buf[256];
  snprintf(buf, sizeof(buf), "%.*f", precision, (float)value);
  return buf;
}

}

#endif
