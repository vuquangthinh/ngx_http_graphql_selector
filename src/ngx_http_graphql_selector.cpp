#include<string>
#include<iostream>
#include<utility>
#include<stack>
#include<algorithm>
#include<vector>
#include "AstNode.hpp"
#include "ngx_http_graphql_selector.hpp"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace GraphQLSelector;

namespace GraphQLSelector {
  const std::string chars = "\t\n\v\f\r ";
    std::string& ltrim(std::string& str)
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }
    
    std::string& rtrim(std::string& str)
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }
    
    std::string& trim(std::string& str)
    {
        return ltrim(rtrim(str));
    }
}


TokenCollection GraphQLSelector::tokenizer(string query) {
  size_t current = 0;

  TokenCollection tokens;

  string str = "";
  while (current < query.length()) {
    auto c = query[current];

    if (c == CODE_LEFT_BRACE) {
      tokens.push_back(make_pair(TOKEN_TYPE::LEFT_BRACE, string(&c)));
      ++current;
      continue;
    }

    if (c == CODE_RIGHT_BRACE) {
      tokens.push_back(make_pair(TOKEN_TYPE::RIGHT_BRACE, string(&c)));
      ++current;
      continue;
    }

    // comments
    if (c == CODE_COMMENT) {
      str = "";
      while (c != TOKEN_TYPE::NL && c != TOKEN_TYPE::CR) {
        str += c;
        c = query[++current];
      }
      tokens.push_back(make_pair(TOKEN_TYPE::COMMENT, string(str)));
      continue;
    }

    if (isspace(c) || c == TOKEN_TYPE::NL || c == TOKEN_TYPE::CR) {
      ++current;
      continue;
    }

    str = "";
    while (
      (
        c != CODE_CR
        && c != CODE_NL
        && c != CODE_COMMENT
        && c != CODE_LEFT_BRACE
        && c != CODE_RIGHT_BRACE
      )
      && current < query.size()
    ) {
      str += c;
      c = query[++current];
    }

    str = trim(str);
    if (str.length()) {
      tokens.push_back(make_pair(TOKEN_TYPE::FIELD, string(str)));
    }
  }

  return tokens;
}

void parseNext(AstNode* node, TokenCollection tokens, size_t current) {
  if (tokens.size() <= current) return;

  if (tokens[current].first == TOKEN_TYPE::COMMENT) {
    parseNext(node, tokens, current + 1);
    return;
  }

  if (tokens[current].first == TOKEN_TYPE::LEFT_BRACE) {
    auto newParent = node->children.back();
    parseNext(newParent, tokens, current + 1);
    return;
  }

  if (tokens[current].first == TOKEN_TYPE::RIGHT_BRACE) {
    parseNext(node->parent, tokens, current + 1);
    return;
  }

  node->children.push_back(new AstNode(tokens[current].second, node));
  parseNext(node, tokens, current + 1);
}

string querySelect(string input, string query) {
  return "";
}

GraphQLSelector::AstNode GraphQLSelector::createAst(TokenCollection tokens) {
  // let ast = new AstItem("$");
  // let parents = [ast];
  // parseNext(parents, tokens, 0);

  auto doc = AstNode("$", NULL);

  parseNext(&doc, tokens, 0);

  return *(doc.children[0]);
}

typedef pair<Value*, pair<AstNode*, Value*> > SelectItem;


Document GraphQLSelector::select(AstNode* ast, Value* input) {
  deque<SelectItem> queues; // [result, [node, resource]]

  Document d(kObjectType);
  queues.push_back(make_pair(&d, make_pair(ast, input)));

  // BFS
  while (!queues.empty()) {
    SelectItem front = queues.front();
    queues.pop_front();

    auto r = front.first; // result
    auto n = front.second.first; // node
    auto s = front.second.second; // source

    auto field = Value(n->field.c_str(), n->field.size());

    if (! s->HasMember(field)) {
      continue;
    }

    // is structure ?, is array
    if (s->operator[](field).IsArray()) {

      Value ri(kArrayType);
      r->AddMember(field, ri, d.GetAllocator());

      auto items = s->operator[](field).GetArray();

      if (n->children.size() == 0) {
          r->operator[](field).CopyFrom(s->operator[](field), d.GetAllocator());
      } else {
        for (auto &v : items) {
          Value ri(kNullType);
          r->operator[](field).PushBack(ri, d.GetAllocator());

          for (auto ni: n->children) {
            queues.push_back(make_pair(&ri, make_pair(ni, &v)));
          }
        }
      }      
    } else if (s->operator[](field).IsObject()) {
      Value ri(kNullType);
      r->AddMember(field, ri, d.GetAllocator());
      // auto si = s->operator[](field).GetObject(); // ->GetObject()[field];

      if (n->children.size() == 0) {
        r->operator[](field).CopyFrom(s->operator[](field), d.GetAllocator());
      } else {
        for (auto ni: n->children) {
          queues.push_back(make_pair(& r->operator[](field), make_pair(ni, & s->operator[](field))));
        }
      }

      continue;
    }

    r->AddMember(field, Value(s->operator[](field).GetType()), d.GetAllocator())
    .CopyFrom(s->operator[](field), d.GetAllocator());

    cout<<"copy"<<field.GetString()<<" ";

  }

  return d;
}


std::string GraphQLSelector::dump(Value& val) {
  // 3. Stringify the DOM
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  val.Accept(writer);

  return buffer.GetString();
}