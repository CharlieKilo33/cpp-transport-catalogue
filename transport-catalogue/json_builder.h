#pragma once
#include "json.h"

using namespace std::literals;

namespace json {

class Builder;
class ArrayItemContext;
class KeyContext;
class ValueContext;
class DictItemContext;

class BaseBuildContext {
 public:
  explicit BaseBuildContext(Builder& builder) :builder_(builder) {}
 protected:
  Builder& builder_;
};

class ContainersContext : public BaseBuildContext {
 public:
  explicit ContainersContext(Builder& builder) : BaseBuildContext(builder) {}

  DictItemContext& StartDict();
  ArrayItemContext& StartArray();
};

class ArrayItemContext : public ContainersContext {
 public:
  explicit ArrayItemContext(Builder& builder) : ContainersContext(builder) {}
  ArrayItemContext& Value(Node::Value value);
  Builder& EndArray();
};

class ValueContext : public BaseBuildContext {
 public:
  explicit ValueContext(Builder& builder) : BaseBuildContext(builder) {}
  KeyContext& Key(std::string key);
  Builder& EndDict();
};

class KeyContext : public ContainersContext {
 public:
  explicit KeyContext(Builder& builder) : ContainersContext(builder) {}
  ValueContext& Value(Node::Value value);
};

class DictItemContext : public BaseBuildContext {
 public:
  explicit DictItemContext(Builder& builder) : BaseBuildContext(builder) {}
  KeyContext& Key(std::string key);
  Builder& EndDict();
};

class Builder : virtual public DictItemContext, virtual public ArrayItemContext, virtual public ValueContext, virtual public KeyContext {
 public:
  Builder() :DictItemContext(*this), ArrayItemContext(*this), ValueContext(*this), KeyContext(*this) {};

  Node Build();

  bool LastDict();
  bool LastArray();
  bool Empty();
  void Finished();
  void CheckFinished()const;


  Builder& Value(Node::Value value) {
    CheckFinished();
    if (Empty()) {
      root_.GetValue() = value;
      Finished();
    }
    else if (LastDict() && key_) {
      auto& ar = std::get<Dict>(nodes_stack_.back()->GetValue());
      ar[current_key].GetValue() = value;
      key_ = false;
    }
    else if (LastArray()) {
      auto& ar = std::get<Array>(nodes_stack_.back()->GetValue());
      Node node;
      node.GetValue() = value;
      ar.emplace_back(node);
    }
    else {
      throw std::logic_error("Value after"s);
    }
    return *this;
  }

  ArrayItemContext& StartArray();
  Builder& EndArray();
  KeyContext& Key(std::string key);
  DictItemContext& StartDict();
  Builder& EndDict();


 private:
  bool finished = false;
  bool key_ = false;
  std::string current_key;
  Node root_;
  std::vector<Node*> nodes_stack_;
};

}