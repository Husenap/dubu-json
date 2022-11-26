#include <any>
#include <string>
#include <vector>

#include "tree.hpp"

namespace dubu::json {

using String  = std::string;
using Number  = double;
using Boolean = bool;

class Value;

class Object {
  ordered_map<std::string, Value> members;

public:
  Value&       operator[](std::string key) { return members[key]; }
  Value const& operator[](std::string const& key) const { return members.at(key); }

  auto begin() { return members.begin(); }
  auto end() { return members.end(); }
  auto cbegin() const { return members.cbegin(); }
  auto cend() const { return members.cend(); }
  auto size() const { return members.size(); }
};
class Array {
  std::vector<Value> elements;

public:
  Value&       operator[](std::size_t index) { return elements[index]; }
  Value const& operator[](std::size_t index) const { return elements[index]; }

  void         push(Value&& v) { elements.push_back(v); }
  void         pop() { elements.pop_back(); }
  Value&       front() { return elements.front(); }
  Value const& front() const { return elements.front(); }
  Value&       back() { return elements.back(); }
  Value const& back() const { return elements.back(); }

  auto begin() { return elements.begin(); }
  auto end() { return elements.end(); }
  auto cbegin() const { return elements.cbegin(); }
  auto cend() const { return elements.cend(); }
  auto size() const { return elements.size(); }
};

class Value {
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
  Value() {}
  Value(Object v)
      : value(v) {}
  Value(Array v)
      : value(v) {}
  Value(String v)
      : value(v) {}
  Value(Number v)
      : value(v) {}
  Value(Boolean v)
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