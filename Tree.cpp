#include"Tree.h"
#include"Parser.h"
#include<algorithm>
#include<iostream>
/*#include<boost/asio.hpp>
#include<boost/array.hpp>
#include<boost/beast/core.hpp>
#include<boost/beast/http.hpp>
#include<boost/beast/version.hpp>*/

enum {
	BUF_SIZE = 1024
};


bool is_number(std::string& s) {
	for (auto ch : s) {
		if (!(ch >= '0' && ch <= '9')) {
			return false;
		}
	}
	return true;
}

Tree& Tree::simplify() {
	remove_par_par(root);
	set_father(root);
	NS1(root);
	NS2(root);
	remove_par_one_child(root);
	set_father(root);
	NS3(root);
	return *this;
}

/*
* @Param node
* This method reclaims the memory of the node
*/
void Tree::reclaim_memory(Node* node) {
	if (!node->children.empty()) {
		for (auto it : node->children) {
			reclaim_memory(it);
		}
	}

	if (!node->ref_count) {
		delete node;
	}
	else {
		--node->ref_count;
	}
}


/*
* The first step deals with parallel composition nodes:
* it removes all unnecessary 0 nodes,
* and relabels the nodes when the parallel composition has only one or no descendants
*/
void Tree::NS1(Node* n) {
	if (n->data_type == Node::type::par) {
		auto num_children = n->children.size();

		if (num_children == 0) {
			n->data_type = Node::type::zero;
			n->value = "0";
		}
		else if (num_children == 1) {
			auto temp = n->children[0],
				par_father = n->father;
			*n = *temp;
			n->father = par_father;
			delete temp;
			NS1(n);
		}
		else {
			size_t j = 0;

			for (size_t i = 0; i < num_children; ++i) {
				auto child = n->children[i];
				NS1(child);
				if (child->data_type == Node::type::zero) {
					delete child;
				}
				else {
					n->children[j] = n->children[i];
					++j;
				}
			}
			n->children.resize(j);
		}
	}
	else {
		for (auto child : n->children) {
			NS1(child);
		}
	}
}

/*
* If the node is a parallel composition,
* sort all the children lexicographically.
*
* If n children are equal,
* leave just one and make n references to it.
*/
void Tree::NS2(Node* n) {
	if (n->children.empty()) {
		return;
	}
	for (auto child : n->children) {
		NS2(child);
	}

	if (n->data_type == Node::type::par) {
		sort_children(n);
		for (size_t i = 0; i < n->children.size() - 1; ++i) {
			if (compare_subtree(n->children[i], n->children[i + 1])) {
				delete n->children[i + 1];
				n->children[i + 1] = n->children[i];
				++n->children[i]->ref_count;
			}
		}
	}
}

void Tree::remove_par_one_child(Node* n) {
	if (n->data_type == Node::type::par && n->children.size() == 1) {
		auto par_father = n->father,
			temp = n->children[0];
		*n = *n->children[0];
		delete temp;
	}

	for (auto child : n->children) {
		remove_par_one_child(child);
	}
}

void Tree::remove_par_par(Node* n){
	std::vector<Node*> tmp;
	if(n->data_type == Node::type::par && n->children.size() > 1){
		for(auto it = n->children.begin(); it != n->children.end();){
			if((*it)->data_type == Node::type::par){
				tmp.push_back(*it);
				it = n->children.erase(it);
			}
			++it;
		}
	}
	if(tmp.empty()){
		for (auto child : n->children) {
			remove_par_par(child);
		}
	}else{
		for(auto t : tmp){
			for(auto c : t->children)
				n->children.push_back(c);
			t->children.clear();
			delete t;
		}
		remove_par_par(n);
	}
}

void Tree::set_father(Node* n) {
	for (auto child : n->children) {
		child->father = n;
		set_father(child);
	}
}

/*
* Normalization Step 3 exploits an auxiliary operation.
*/
void Tree::NS3(Node* n) {
	if (n->children.empty()) {
		return;
	}

	for (auto child : n->children) {
		NS3(child);
	}

	if (n->data_type == Node::type::inp) {
		auto s = n->children[0];
		if (s->data_type == Node::type::par) {
			/*if (s->children.size() == 1) {
				auto par_father = s->father;
				auto temp = s->children[0];
				*s = *s->children[0];
				delete temp;
				s->father = par_father;
				return;
			}*/ // �Լ�����ȥ��, ��Ҳ��֪���Բ���

			auto c = n->value;
			auto ok = false;
			auto small = -1,
				big = -1;

			if (subtree(s->children[0], s->children[1], c)) {
				small = 0;
				big = 1;
				ok = true;
			}
			else if (subtree(s->children[s->children.size() - 1], s->children[0], c)) {
				small = s->children.size() - 1;
				big = 0;
				ok = true;
			}

			for (size_t i = 1; i < n->children.size() - 1; ++i) {
				if (big == -1) {
					ok = false;
					break;
				}
				if (s->children[big] != s->children[i]) {
					ok = false;
				}
			}

			if (ok) {
				auto father = n->father;
				*n = *s;
				n->father = father;
				delete s;
				auto aux = n->children[small];
				n->children[small] = n->children[big];
				reclaim_memory((n->children[big])->children[0]);
				n->children[big]->children[0] = aux;
				n->children[big]->children[0]->father = n->children[big];
			}
		}
	}
}

/*
* This method checks if two input tree are equal.
*/
bool Tree::compare_subtree(const Node* subtree1, const Node* subtree2) const {
	if (!subtree1 || !subtree2) {
		return false;
	}

	bool result = true;

	if (subtree1->children.size() != subtree2->children.size() ||
		subtree1->data_type != subtree2->data_type ||
		subtree1->db != subtree2->db ||
		subtree1->value != subtree2->value) {
		return false;
	}
	else {
		if (subtree1->children.empty() && subtree2->children.empty()) {
			return true;
		}
		else {
			for (size_t i = 0; i < subtree1->children.size(); ++i) {
				result &= compare_subtree(subtree1->children[i], subtree2->children[i]);
			}
		}
	}

	return result;
}

/*
*  Operation subtree(p1, p2, c) takes as arguments two pointers to nodes p1 and p2 and a name c.
*  It checks whether p2 points to an input node with label c and with a subtree equal to p1.
*/
bool Tree::subtree(const Node* p1, const Node* p2, const std::string c) const {
	if (!p1 || !p2) {
		return false;
	}

	const Node* tmp = p2;

	int db = p2->db;

	while (db > 0) {
		tmp = tmp->father;
		if (tmp->data_type == Node::type::inp /*|| tmp->data_type == Node::type::out*/) {
			--db;
		}
	}

	if (!p2->children.empty() && tmp->value == c && compare_subtree(p1, p2->children[0])) {
		return true;
	}

	return false;
}

/*
* This method sort children by lexicographic order.
*/
void Tree::sort_children(Node* n) {
	std::sort(n->children.begin(),
		n->children.end(),
		[](Node* first, Node* end) {
			return first->value < end->value;
		}
	);
}

void Tree::preorder_print_tree() const {
	//root->preorder_print_tree();
}

void Tree::NS1_enhanced(Node* n) {
	for (auto i : n->children) {
		NS1_enhanced(i);
	}

	if (n->data_type == Node::type::app) {
		if (n->children[0]->data_type == Node::type::abs) {
			App(n->children[0]->children[0], 1, n->children[1]);
		}
	}
}

void Tree::NS2_enhanced(Node* n) {
	if (n->data_type == Node::type::par) {
		auto num_children = n->children.size();

		if (num_children == 0) {
			n->data_type = Node::type::zero;
			n->value = "0";
		}
		else if (num_children == 1) {
			auto temp = n->children[0],
				par_father = n->father;
			n->children.clear();
			delete n;
			n = temp;
			n->father = par_father;
			NS2_enhanced(n);
		}
		else {
			size_t j = 0;

			for (size_t i = 0; i < num_children; ++i) {
				auto child = n->children[i];
				NS1(child);
				if (child->data_type == Node::type::zero) {
					delete child;
				}
				else {
					n->children[j] = n->children[i];
					++j;
				}
			}
			n->children.resize(j);
		}
	}
	else {
		for (auto child : n->children) {
			NS2_enhanced(child);
		}
	}
	
	if (n->children.size() == 0) {
		n->data_type = Node::type::zero;
		n->value = "0";
	}
	sort_children(n);
}

void Tree::App(Node* n_raw, int ind, Node* n_eval) {
	if ((n_raw->data_type == Node::type::var || n_raw->data_type == Node::type::inp)
		&& n_raw->db == ind) {
		delete n_raw;
		n_raw = n_eval;
	}

	if (n_raw->data_type == Node::type::out && ::is_number(n_raw->value) && n_raw->db == ind ) {
		delete n_raw;
		n_raw = n_eval;
		n_raw->data_type = Node::type::out;
	}
	if (n_raw->data_type == Node::type::inp || n_raw->data_type == Node::type::abs) {
		ind++;
	}
	for (auto child : n_raw->children) {
		App(child, ind, n_eval);
	}
}



Tree& Tree::simplify_enhanced() {
	NS1_enhanced(root);
	NS2_enhanced(root);
	remove_par_one_child(root);
	set_father(root);
	NS3(root);
	return *this;
}

std::string Tree::to_string() {
	std::string res{""};
	root->preorder_print_tree(res);
	return res;
}
