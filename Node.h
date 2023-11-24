#ifndef NODE_H
#define NODE_H
#include<vector>
#include<iostream>

class Node {

public:
	std::vector<Node*> children;
	Node* father = nullptr;
	enum class type {
		zero,
		var,
		inp,
		out,
		par,
		abs,
		app,
		cons
	} data_type;
	std::string value;
	int db = 0;
	int ref_count = 0;

	Node(const std::string type, const std::string _value, const int _db) : value{ _value }, db{ _db } {
		if (type == "zero") {
			data_type = type::zero;
		}
		else if (type == "var") {
			data_type = type::var;
		}
		else if (type == "inp") {
			data_type = type::inp;
		}
		else if (type == "out") {
			data_type = type::out;
		}
		else if (type == "par") {
			data_type = type::par;
		}
		else if (type == "abs") {
			data_type = type::abs;
		}
		else if (type == "app") {
			data_type = type::app;
		}
		else if (type == "cons") {
			data_type = type::cons;
		}
	}

	void add_children(const std::string _type, const std::string _value, const int _db) {
		children.push_back(new Node(_type, _value, _db));
		children.back()->father = this;
	}

	void add_children(Node* child) {
		children.push_back(child);
	}

	void change_type(std::string type) {
		if (type == "zero") {
			data_type = type::zero;
		}
		else if (type == "var") {
			data_type = type::var;
		}
		else if (type == "inp") {
			data_type = type::inp;
		}
		else if (type == "out") {
			data_type = type::out;
		}
		else if (type == "par") {
			data_type = type::par;
		}
		else if (type == "abs") {
			data_type = type::abs;
		}
		else if (type == "app") {
			data_type = type::app;
		}
		else if (type == "cons") {
			data_type = type::cons;
		}
	}

	void print() const {
		std::cout << value << std::endl;
	}

	void preorder_print_tree(std::string& str) {
		//this->print();
		str += this->value;
		if (!this->children.empty()) {
			str += "(";
			for (auto temp : children)
				temp->preorder_print_tree(str);
			str += ")";
		}
		else {
			return;
		}
	}


	~Node() = default;
};
#endif