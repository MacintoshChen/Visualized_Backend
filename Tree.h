#ifndef TREE_H
#define TREE_H
#include"Node.h"
#include<string>
#include<memory>
#include<vector>


class Tree {
	friend int main(); // test only
	friend void test();

public:
	Node* root = nullptr;

	std::shared_ptr<std::string> input_process;

	void reclaim_memory(Node*);

	void NS1(Node*);
	void NS1_enhanced(Node*);

	void NS2(Node*);
	void NS2_enhanced(Node*);

	void remove_par_one_child(Node*);

	void remove_par_par(Node*);

	void set_father(Node*);

	void NS3(Node*);

	void App(Node*, int, Node*);

	void sort_children(Node*);

	bool compare_subtree(const Node*, const Node*) const;

	bool subtree(const Node*, const Node*, const std::string) const;

	std::string to_string();

public:
	Tree() = default;

	~Tree() {
		if (root)
			reclaim_memory(root);
	}

	Tree& simplify();

	Tree& simplify_enhanced();

	Node* get_root() {
		return root;
	}

	void set_root(Node* p) {
		root = p;
	};

	void preorder_print_tree() const;  // test only
};

#endif