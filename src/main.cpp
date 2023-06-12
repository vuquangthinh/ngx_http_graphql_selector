#include<iostream>
#include<vector>
#include<string>
#include "ngx_http_graphql_selector.hpp"
#include "rapidjson/prettywriter.h"

using namespace std;
using namespace GraphQLSelector;

// GraphQLSelector query
int main(int argc, char *argv[]) {

  // if (argc < 2) {
  //   cout<<"missing json";
  //   return 0;
  // }

  // string source;
  // source.append("{\"$\": ").append(argv[1]).append("}");

  // auto query = argc == 3 ? string(argv[2]) : " ";
  // cout<<"gql:"<<query<<endl;

  // auto tokens = tokenizer(string("$ {").append(query).append("}"));
  // auto ast = createAst(tokens);

  // // test extract
  // Document doc;
  // doc.Parse(source.c_str(), source.size());

  // StringBuffer bf;
  // Writer<StringBuffer> wr(bf);

  // doc.Accept(wr);
  // cout<<"source:"<<endl<<bf.GetString()<<endl;

  // auto output = select(&ast, &doc);

  // StringBuffer buffer;
  // Writer<StringBuffer> writer(buffer);

  // output.Accept(writer);
  // cout<<"result:"<<endl<< buffer.GetString()<<endl;





  auto tokens = tokenizer(string("$ { data { title } pagination }"));
  // auto tokens = tokenizer(string("$ { pagination { pageSize { d { e } a {} c f } } }"));


  // dump token
  for (auto token: tokens) {
    cout<<"("<<token.first<<" ->"<<token.second<<")"<<endl;
  }

  // auto tokens = tokenizer(string("$ { pagination { current } }"));

  auto ast = createAst(tokens);

 // 3. Stringify the DOM
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);

  ast.toJson().Accept(writer);

  cout << "selector:" << buffer.GetString() << endl;
  writer.Flush();

  // test extract
  Document doc;
  doc.Parse("{\"$\":{\"data\":[{\"title\":{\"xz\":[1,2,4]}}],\"pagination\":{\"current\":1,\"pageSize\":{\"b\":{},\"d\":[{\"e\":4}],\"c\":{\"e\":4},\"a\":3,\"f\":[1,2,4]}}}}");

  cout<<"input: "<<dump(doc)<<endl;

  auto output = select(&ast, &doc);

  cout<<"output:"<<dump(output)<<endl;

  return 0;
}