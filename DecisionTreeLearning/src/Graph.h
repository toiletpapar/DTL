#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;

//Attributes
template <class Domain>
class Variable {
public:
	Variable(string name, vector<Domain> domain);

	vector<Domain> &get_domain();
	string get_name();
private:
	vector<Domain> domain;
	string name;
};

template <typename Domain>
Variable<Domain>::Variable(string name, vector<Domain> domain) : name(name), domain(domain) {

}

template <typename Domain>
vector<Domain> &Variable<Domain>::get_domain() {
	return this->domain;
}

template <typename Domain>
string Variable<Domain>::get_name() {
	return this->name;
}

//Decision Tree
template <class K, class T>
class Node {
public:
	Node();
	Node(T data);
	Node(T data, Variable<K>* variable);
	Node(T data, Variable<K>* variable, Node<K, T>* parent);

	Node<K, T>* add_neighbour(K transition, Node<K, T>* node);
	Node<K, T>* get_neighbour(K transition);
	Node<K, T>* set_neighbour(K transition, Node<K, T>* node);
	Node<K, T>* remove_neighbour(K transition);
	map<K, Node<K, T>*> &get_neighbours();
	bool is_leaf();

	T get_data();
	Node<K, T>* set_data(T data);

	Node<K, T>* get_parent();
	Node<K, T>* set_parent(Node<K, T>* parent);

	Variable<K>* get_variable();
	Node<K, T>* set_variable(Variable<K>* variable);

	double get_threshold();
	Node<K, T>* set_threshold(double threshold);

	static void print_graph(Node<K, T>* node, long count);
private:
	Node<K, T>* parent;
	Variable<K>* variable;
	double threshold;
	map<K, Node<K, T>*> neighbours;
	T data;
};

//Implementation
template <typename K, typename T>
Node<K, T>::Node(T data, Variable<K>* variable, Node<K, T>* parent) : data(data), variable(variable), parent(parent), neighbours(map<K, Node<K, T>*>()) {

}

template <typename K, typename T>
Node<K, T>::Node(T data, Variable<K>* variable) : Node<K, T>(data, variable, NULL) {

}

template<class K, class T>
Node<K, T>::Node(T data) : Node<K, T>(data, NULL) {

}

template<class K, class T>
Node<K, T>::Node() : Node<K, T>(NULL) {

}

template <typename K, typename T>
Node<K, T>* Node<K, T>::add_neighbour(K transition, Node<K, T>* node) {
	this->neighbours.insert(pair<K, Node<K, T>*>(transition, node));
	return this;
}

template<class K, class T>
Node<K, T>* Node<K, T>::get_neighbour(K transition)
{
	return this->get_neighbours().find(transition)->second;
}

template<class K, class T>
Node<K, T>* Node<K, T>::set_neighbour(K transition, Node<K, T>* node)
{
	this->get_neighbours().at(transition) = node;
	return this;
}

template<class K, class T>
Node<K, T>* Node<K, T>::remove_neighbour(K transition)
{
	this->get_neighbours().erase(transition);
	return this;
}

template <typename K, typename T>
map<K, Node<K, T>*> &Node<K, T>::get_neighbours() {
	return this->neighbours;
}

template<class K, class T>
bool Node<K, T>::is_leaf()
{
	return this->get_neighbours().empty();
}

template <typename K, typename T>
T Node<K, T>::get_data() {
	return this->data;
}

template <typename K, typename T>
Node<K, T>* Node<K, T>::set_data(T data) {
	this->data = data;
	return this;
}

template<class K, class T>
Node<K, T>* Node<K, T>::get_parent()
{
	return this->parent;
}

template<class K, class T>
Node<K, T>* Node<K, T>::set_parent(Node<K, T>* parent)
{
	this->parent = parent;
	return this;
}

template<class K, class T>
Variable<K>* Node<K, T>::get_variable()
{
	return this->variable;
}

template<class K, class T>
Node<K, T>* Node<K, T>::set_variable(Variable<K>* variable)
{
	this->variable = variable;
	return this;
}

template<class K, class T>
double Node<K, T>::get_threshold()
{
	return this->threshold;
}

template<class K, class T>
Node<K, T>* Node<K, T>::set_threshold(double threshold)
{
	this->threshold = threshold;
	return this;
}

//Assume acyclic
template<class K, class T>
void Node<K, T>::print_graph(Node<K, T>* node, long count)
{
	for (long i = 0; i < count; i++) {
		cout << "  ";
	}

	if (!node->is_leaf()) {
		//non-leaf node
		cout << "Name: " << node->get_variable()->get_name() << " Threshold: " << node->get_threshold() << endl;
		map<K, Node<K, T>*> neighbours = node->get_neighbours();
		count++;
		for (map<K, Node<K, T>*>::iterator it = neighbours.begin(); it != neighbours.end(); it++) {
			print_graph(it->second, count);
		}
	}
	else {
		cout << "Classification: " << node->get_data() << endl;
	}
}

#endif