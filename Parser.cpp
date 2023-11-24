#include"Parser.h"

void Parser::add_to_list(std::string c, Node* p) {
	if (var_map.find(c) != var_map.end()) {
		var_map.emplace(std::make_pair(c, p));
	}
	else {
		var_map.at(c) = p;
	}
}

void Parser::add_to_node_list(Node* p) {
	auto process = std::make_shared<Tree>();
	process->set_root(p);
	node_list.push_back(process);
	std::cout << "adding to node list" << std::endl;
}

Node* Parser::set_content(char value) {
	std::string v(1, value);
	return new Node("cons", v, 0);
}