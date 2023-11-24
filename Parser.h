#ifndef PARSER_H
#define PARSER_H
#include<initializer_list>
#include<vector>
#include<map>
#include<string>
#include<memory>
#include"Tree.h"


class Parser {
public:
	std::map<std::string, Node*> var_map;
	std::vector<std::shared_ptr<Tree>> node_list;


	void add_to_node_list(Node*);
	void add_to_list(std::string, Node*);

	Node* set_content(char value);
};
#endif