#include <istream>

#include "type.hpp"

namespace dubu::json {

static UnspecifiedType parse(std::istream&) {
  return Object{};
}

static UnspecifiedType parse(std::string_view data) {
  std::stringstream ss;
  ss << data;
  return parse(ss);
}

}  // namespace dubu::json