#include "json_builder.h"

using namespace std;

namespace json {


KeyContext& DictItemContext::Key(std::string key) {
  return builder_.Key(move(key));
}

Builder& DictItemContext::EndDict() {
  return builder_.EndDict();
}

DictItemContext& ContainersContext::StartDict() {
  return builder_.StartDict();
}

ArrayItemContext& ArrayItemContext::Value(Node::Value value) {
  return builder_.Value(move(value));
}

ArrayItemContext& ContainersContext::StartArray() {
  return builder_.StartArray();
}

ValueContext& KeyContext::Value(Node::Value value) {
  return builder_.Value(move(value));
}

KeyContext& ValueContext::Key(string key) {
  return builder_.Key(move(key));
}

Builder& ArrayItemContext::EndArray() {
  return builder_.EndArray();
}

Builder& ValueContext::EndDict() {
  return builder_.EndDict();
}

bool Builder::LastDict() {
  return (nodes_stack_.back()->IsDict());
}

bool Builder::LastArray() {
  return (nodes_stack_.back()->IsArray());
}

bool Builder::Empty() {
  return (nodes_stack_.empty());
}

void Builder::Finished() {
  if (Empty()) {
    finished = true;
  }
}

void Builder::CheckFinished() const {
  if (finished) {
    throw std::logic_error("Building end"s);
  }
}
Node Builder::Build() {
  if (!nodes_stack_.empty() || !finished || key_) {
    throw std::logic_error("Call \"Build\" with unfinished object");
  }
  else {
    return root_;
  }
}

ArrayItemContext& Builder::StartArray() {
  CheckFinished();
  if (Empty()) {
    root_ = Array();
    nodes_stack_.emplace_back(&root_);
  }
  else if (LastDict() && key_) {
    auto& ar = std::get<Dict>(nodes_stack_.back()->GetValue());
    ar[current_key] = Node(Array());
    nodes_stack_.push_back(&ar.at(current_key));
    key_ = false;
  }
  else if (LastArray()) {
    auto& ar = std::get<Array>(nodes_stack_.back()->GetValue());
    ar.emplace_back(Node(Array()));
    nodes_stack_.push_back(&ar.back());
  }
  else {
    throw std::logic_error("StartArray after"s);
  }
  return *this;
}
Builder& Builder::EndArray() {
  CheckFinished();
  if (!LastArray()) {
    throw std::logic_error("EndArray without Array"s);
  }
  else {
    nodes_stack_.pop_back();
    Finished();
  }
  return *this;
}
KeyContext& Builder::Key(std::string key) {
  CheckFinished();
  if (!Empty() && LastDict() && !key_) {
    current_key = key;
    key_ = true;
  }
  else {
    throw std::logic_error("Dont need key"s);
  }
  return *this;
}
DictItemContext& Builder::StartDict() {
  CheckFinished();
  if (Empty()) {
    root_ = Dict();
    nodes_stack_.emplace_back(&root_);
  }
  else if (LastDict() && key_) {
    auto& ar = std::get<Dict>(nodes_stack_.back()->GetValue());
    ar[current_key] = Node(Dict());
    nodes_stack_.push_back(&ar.at(current_key));
    key_ = false;
  }
  else if (LastArray()) {
    auto& ar = std::get<Array>(nodes_stack_.back()->GetValue());
    ar.emplace_back(Node(Dict()));
    nodes_stack_.push_back(&ar.back());
  }
  else {
    throw std::logic_error("StartDict after..."s);
  }
  return *this;
}
Builder& Builder::EndDict() {
  CheckFinished();
  if (!LastDict() || key_) {
    throw std::logic_error("EndDict without Dict"s);
  }
  else {
    nodes_stack_.pop_back();
    Finished();
  }
  return *this;
}
}