#include "json_builder.h"

using namespace std;

namespace json {

ItemContext &ItemContext::Key(std::string key) {
  builder_.CheckFinished();
  if (!builder_.Empty() && builder_.LastDict() && !builder_.key_) {
    builder_.current_key = std::move(key);
    builder_.key_ = true;
  } else {
    throw std::logic_error("Invalid Key usage"s);
  }
  return *this;
}

ItemContext &ItemContext::Value(Node::Value value) {
  builder_.CheckFinished();
  if (builder_.Empty()) {
    builder_.root_.GetValue() = std::move(value);
    builder_.Finished();
  } else if (builder_.LastDict() && builder_.key_) {
    auto &dict = std::get<Dict>(builder_.nodes_stack_.back()->GetValue());
    dict[builder_.current_key].GetValue() = std::move(value);
    builder_.key_ = false;
  } else if (builder_.LastArray()) {
    auto &array = std::get<Array>(builder_.nodes_stack_.back()->GetValue());
    Node node;
    node.GetValue() = std::move(value);
    array.emplace_back(node);
  } else {
    throw std::logic_error("Invalid Value usage"s);
  }
  return *this;
}

ItemContext &ItemContext::StartDict() {
  builder_.CheckFinished();
  if (builder_.Empty()) {
    builder_.root_ = Dict();
    builder_.nodes_stack_.emplace_back(&builder_.root_);
  } else if (builder_.LastDict() && builder_.key_) {
    auto &dict = std::get<Dict>(builder_.nodes_stack_.back()->GetValue());
    dict[builder_.current_key] = Node(Dict());
    builder_.nodes_stack_.push_back(&dict.at(builder_.current_key));
    builder_.key_ = false;
  } else if (builder_.LastArray()) {
    auto &array = std::get<Array>(builder_.nodes_stack_.back()->GetValue());
    array.emplace_back(Node(Dict()));
    builder_.nodes_stack_.push_back(&array.back());
  } else {
    throw std::logic_error("Invalid StartDict usage"s);
  }
  return *this;
}

ItemContext &ItemContext::StartArray() {
  builder_.CheckFinished();
  if (builder_.Empty()) {
    builder_.root_ = Array();
    builder_.nodes_stack_.emplace_back(&builder_.root_);
  } else if (builder_.LastDict() && builder_.key_) {
    auto &dict = std::get<Dict>(builder_.nodes_stack_.back()->GetValue());
    dict[builder_.current_key] = Node(Array());
    builder_.nodes_stack_.push_back(&dict.at(builder_.current_key));
    builder_.key_ = false;
  } else if (builder_.LastArray()) {
    auto &array = std::get<Array>(builder_.nodes_stack_.back()->GetValue());
    array.emplace_back(Node(Array()));
    builder_.nodes_stack_.push_back(&array.back());
  } else {
    throw std::logic_error("Invalid StartArray usage"s);
  }
  return *this;
}

Builder &ItemContext::EndDict() {
  builder_.CheckFinished();
  if (!builder_.LastDict() || builder_.key_) {
    throw std::logic_error("Invalid EndDict usage"s);
  } else {
    builder_.nodes_stack_.pop_back();
    builder_.Finished();
  }
  return builder_;
}

Builder &ItemContext::EndArray() {
  builder_.CheckFinished();
  if (!builder_.LastArray()) {
    throw std::logic_error("Invalid EndArray usage"s);
  } else {
    builder_.nodes_stack_.pop_back();
    builder_.Finished();
  }
  return builder_;
}

bool Builder::LastDict() {
  return nodes_stack_.back()->IsDict();
}

bool Builder::LastArray() {
  return nodes_stack_.back()->IsArray();
}

bool Builder::Empty() {
  return nodes_stack_.empty();
}

void Builder::Finished() {
  if (Empty()) {
    finished = true;
  }
}

void Builder::CheckFinished() const {
  if (finished) {
    throw std::logic_error("Build process already finished"s);
  }
}

Node Builder::Build() {
  if (!nodes_stack_.empty() || !finished || key_) {
    throw std::logic_error("Call \"Build\" with unfinished object"s);
  } else {
    return root_;
  }
}

}
