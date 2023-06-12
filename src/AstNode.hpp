
#pragma once

#include<vector>
#include<string>
#include "rapidjson/document.h"
#include "rapidjson/allocators.h"

using namespace std;
using namespace rapidjson;

namespace GraphQLSelector {

  class AstNode {
    public:
      string field;

      AstNode* parent;

      vector<AstNode*> children;
      
      AstNode(string field, AstNode* parent): field(field), children(), parent(parent) {}
      
      Value toJson(MemoryPoolAllocator<CrtAllocator> &allocator);

      Document toJson();

  };

}