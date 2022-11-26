#include <codecvt>
#include <istream>
#include <locale>

#include "type.hpp"

namespace dubu::json {

namespace detail {

class Parser {
  std::istream& in;

  inline char peek() { return static_cast<char>(in.peek()); }
  inline char get() { return static_cast<char>(in.get()); }
  inline void skip() { get(); }

  bool isWhitespace(char c) { return c == 0x20 || c == 0x0a || c == 0x0d || c == 0x09; }

  void ws() {
    while (isWhitespace(peek())) skip();
  }

  void expect(char c) {
    if (get() != c) throw std::runtime_error("Unexpected!");
  }
  void expect(std::string_view expected) {
    for (char c : expected) expect(c);
  }

  Boolean btrue() {
    expect("true");
    return true;
  }
  Boolean bfalse() {
    expect("false");
    return false;
  }
  Value null() {
    expect("null");
    return Value{};
  }

  char hex() {
    char c = peek();
    if (std::isdigit(c)) {
      return get() - '0';
    } else if (c >= 'a' && c <= 'f') {
      return get() - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      return get() - 'A' + 10;
    }
    throw std::runtime_error("Unexpected hex digit");
  }
  void escape(std::string& text) {
    switch (get()) {
    case '"':
      text += '"';
      break;
    case '\\':
      text += '\\';
      break;
    case '/':
      text += '/';
      break;
    case 'b':
      text += '\b';
      break;
    case 'f':
      text += '\f';
      break;
    case 'n':
      text += '\n';
      break;
    case 'r':
      text += '\r';
      break;
    case 't':
      text += '\t';
      break;
    case 'u': {
      // https://en.cppreference.com/w/cpp/locale/codecvt/out#Example
      static std::mbstate_t mb;
      static auto const&    conv_16_8 =
          std::use_facet<std::codecvt<char16_t, char8_t, std::mbstate_t>>(std::locale());
      static char8_t  buffer[3];
      const char16_t* from_next;
      char8_t*        to_next;

      const auto c = static_cast<char16_t>((hex() << 12) | (hex() << 8) | (hex() << 4) | hex());

      const auto result = conv_16_8.out(mb, &c, &c + 1, from_next, buffer, buffer + 3, to_next);

      if (result == std::codecvt_base::ok) {
        for (char8_t* p = buffer; p != to_next; ++p) {
          text += static_cast<char>(*p);
        }
      } else {
        throw std::runtime_error("Failed to convert from UTF-16 to UTF-8!");
      }
      break;
    }
    }
  }
  void character(std::string& text) {
    char c = get();
    if (c == '\\') {
      escape(text);
    } else if (!std::iscntrl(c)) {
      text += c;
    }
  }
  String characters() {
    std::string text;
    while (peek() != '"') {
      character(text);
    }
    return text;
  }
  String string() {
    expect('"');
    auto text = characters();
    expect('"');
    return text;
  }

  void sign(std::string& text) {
    char c = peek();
    if (c == '-' || c == '+') {
      text += get();
    }
  }
  void exponent(std::string& text) {
    char c = peek();
    if (c != 'e' && c != 'E') return;
    text += get();
    sign(text);
    digits(text);
  }
  void fraction(std::string& text) {
    if (peek() != '.') return;
    text += get();
    digits(text);
  }
  void digits(std::string& text) {
    while (std::isdigit(peek())) {
      text += get();
    }
  }
  void integer(std::string& text) {
    sign(text);
    digits(text);
  }
  Number number() {
    std::string text;
    integer(text);
    fraction(text);
    exponent(text);
    return std::stod(text);
  }

  void member(Object& obj) {
    ws();
    auto key = string();
    ws();
    expect(':');
    obj[key] = element();
  }
  void members(Object& obj) {
    member(obj);
    while (peek() == ',') {
      skip();
      member(obj);
    }
  }
  Object object() {
    Object obj;
    expect('{');
    ws();
    if (peek() != '}') {
      members(obj);
    }
    expect('}');
    return obj;
  }

  void elements(Array& arr) {
    arr.push(element());
    while (peek() == ',') {
      skip();
      arr.push(element());
    }
  }
  Array array() {
    Array arr;
    expect('[');
    ws();
    if (peek() != ']') {
      elements(arr);
    }
    expect(']');
    return arr;
  }

  Value value() {
    switch (peek()) {
    case '{':
      return object();
    case '[':
      return array();
    case '"':
      return string();
    case 't':
      return btrue();
    case 'f':
      return bfalse();
    case 'n':
      return null();
    default:
      return number();
    }
  }

public:
  Parser(std::istream& _in)
      : in(_in) {}

  Value element() {
    ws();
    Value v = value();
    ws();
    return v;
  }
};

}  // namespace detail

inline Value parse(std::istream& in) {
  detail::Parser parser(in);
  return parser.element();
}

inline Value parse(std::string_view data) {
  std::stringstream ss;
  ss << data;
  return parse(ss);
}

}  // namespace dubu::json