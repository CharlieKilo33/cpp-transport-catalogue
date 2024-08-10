#pragma once
#include "json.h"

using namespace std::literals;

namespace json {

class Builder;
class ItemContext;

class BaseBuildContext {
 public:
  explicit BaseBuildContext(Builder& builder) : builder_(builder) {}
 protected:
  Builder& builder_;
};

class ItemContext : public BaseBuildContext {
 public:
  explicit ItemContext(Builder& builder) : BaseBuildContext(builder) {}
  ItemContext& Key(std::string key);
  ItemContext& Value(Node::Value value);
  ItemContext& StartDict();
  ItemContext& StartArray();
  Builder& EndDict();
  Builder& EndArray();
};

class Builder : public ItemContext {
 public:
  Builder() : ItemContext(*this) {}

  Node Build();

  bool LastDict();
  bool LastArray();
  bool Empty();
  void Finished();
  void CheckFinished() const;

 private:
  friend class ItemContext;
  bool finished = false;
  bool key_ = false;
  std::string current_key;
  Node root_;
  std::vector<Node*> nodes_stack_;
};

}
