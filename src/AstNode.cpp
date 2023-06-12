#include<iostream>
#include "AstNode.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

MemoryPoolAllocator<rapidjson::CrtAllocator> jsonAlloc;

Document GraphQLSelector::AstNode::toJson() {
  Document output(kObjectType);

  auto allocator = & output.GetAllocator();

  output.AddMember("field", Value(kStringType), *allocator);
  output["field"].SetString(this->field.c_str(), this->field.size());


  if (this->children.size()) {
    Value emptyArray(kArrayType);
    output.AddMember("children", emptyArray, *allocator);

    auto children = output["children"].GetArray();

    for (auto item: this->children) {
      auto child = item->toJson(*allocator);
      children.PushBack(child, *allocator);
    }
  }

  return output;
}

Value GraphQLSelector::AstNode::toJson(MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {

  Value output(kObjectType);

  output.AddMember("field", Value(kStringType), allocator);
  output["field"].SetString(this->field.c_str(), this->field.size());

  if (this->children.size()) {
    Value emptyArray(kArrayType);
    output.AddMember("children", emptyArray, allocator);

    auto children = output["children"].GetArray();

    for (auto item: this->children) {
      auto child = item->toJson(allocator);
      children.PushBack(child, allocator);
    }
  }

  return output;
}
