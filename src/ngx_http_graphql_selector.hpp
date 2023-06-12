#include<vector>
#include<string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "constants.h"
#include "AstNode.hpp"

using namespace std;
using namespace rapidjson;

namespace GraphQLSelector {

  typedef struct pair<TOKEN_TYPE, string> Token;

  typedef vector<Token> TokenCollection;
  // typedef Document AstNode;

  TokenCollection tokenizer(string query);

  /**
   * create ast from tokenizer
   */
  AstNode createAst(TokenCollection tokens);

  /**
   * generate new json data from ast graphql
   */
  Document select(AstNode* ast, Value* input); 

  string dump(Value& value);
}

