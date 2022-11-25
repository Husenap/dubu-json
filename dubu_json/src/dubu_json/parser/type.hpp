#include <any>
#include <unordered_map>
#include <vector>

namespace dubu::json {

struct Object;
struct Array;
using String  = std::string;
using Number  = double;
using Boolean = bool;

struct UnspecifiedType;

class Object {
  std::unordered_map<std::string, UnspecifiedType> members;

public:
  auto begin() { return members.begin(); }
  auto end() { return members.end(); }
  auto cbegin() const { return members.cbegin(); }
  auto cend() const { return members.cend(); }
  auto size() const { return members.size(); }
};
class Array {
  std::vector<UnspecifiedType> elements;

public:
  auto begin() { return elements.begin(); }
  auto end() { return elements.end(); }
  auto rbegin() { return elements.rbegin(); }
  auto rend() { return elements.rend(); }
  auto cbegin() const { return elements.cbegin(); }
  auto cend() const { return elements.cend(); }
  auto crbegin() const { return elements.crbegin(); }
  auto crend() const { return elements.crend(); }
  auto size() const { return elements.size(); }
};

class UnspecifiedType {
  std::any value;

  template <typename T>
  T const& as() const {
    return std::any_cast<T const&>(value);
  }

  template <typename T>
  T& as() {
    return std::any_cast<T&>(value);
  }

public:
  UnspecifiedType() {}
  UnspecifiedType(Object v)
      : value(v) {}
  UnspecifiedType(Array v)
      : value(v) {}
  UnspecifiedType(String v)
      : value(v) {}
  UnspecifiedType(Number v)
      : value(v) {}
  UnspecifiedType(Boolean v)
      : value(v) {}

  void operator=(Object const& v) { value = v; }
  void operator=(Array const& v) { value = v; }
  void operator=(String const& v) { value = v; }
  void operator=(std::string_view v) { value = String{v}; }
  void operator=(const char* v) { value = String{v}; }
  void operator=(Number const& v) { value = v; }
  void operator=(Boolean const& v) { value = v; }
  template <typename T>
  void operator=(T v) = delete;

  bool isNull() const { return !value.has_value(); }

  Object&  asObject() { return as<Object>(); }
  Array&   asArray() { return as<Array>(); }
  String&  asString() { return as<String>(); }
  Number&  asNumber() { return as<Number>(); }
  Boolean& asBoolean() { return as<Boolean>(); }

  Object const&  asObject() const { return as<Object>(); }
  Array const&   asArray() const { return as<Array>(); }
  String const&  asString() const { return as<String>(); }
  Number const&  asNumber() const { return as<Number>(); }
  Boolean const& asBoolean() const { return as<Boolean>(); }
};

}  // namespace dubu::json