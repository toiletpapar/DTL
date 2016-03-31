#include <map>
#include <vector>

#include "Graph.h"

using namespace std;

//Attributes
template <class Domain, class Classification>
class Variable {
public:
	Variable(string name, vector<Domain> domain);

	Classification& get_classification();
	Variable<Domain, Classification>* set_classification(Classification value);

	vector<Domain> &get_domain();
	string get_name();
private:
	vector<Domain> domain;
	Classification value;
	Domain threshold;
	string name;
};

template <typename Domain, typename Classification>
Variable<Domain, Classification>::Variable(string name, vector<Domain, Classification> domain) : name(name), domain(domain) {

}

template <typename Domain, typename Classification>
Classification& Variable<Domain, Classification>::get_classification() {
	return this->value;
}

template <typename Domain, typename Classification>
Variable<Domain, Classification>* Variable<Domain, Classification>::set_classification(Classification value) {
	this->value = value;

	return this;
}

template <typename Domain, typename Classification>
vector<Domain> &Variable<Domain, Classification>::get_domain() {
	return this->domain;
}

template <typename Domain, typename Classification>
string Variable<Domain, Classification>::get_name() {
	return this->name;
}

template <class Key, class Classification>
class Example {
	vector<Variable<Key, Classification>*> attributes;
	vector<Key> attribute_instantiation;
	Classification classification;
};

template <typename Key, typename Classification>
bool is_same_classification(vector<Example<Key, Classification> > examples) {
	Classification klass = examples.at(0).classification;

	for (unsigned int i = 1; i < examples.size(); i++) {
		if (klass != examples.at(i).classification) {
			return false;
		}
	}

	return true;
}

template <typename Key, typename Classification>
Variable<Key, Classification>* most_classified_example(vector<Example<Key, Classification> > examples) {
	//return the classification with the highest number of occurrences in examples
}

template <typename Key, typename Classification>
Variable<Key, Classification>* choose_best_attribute(vector<Variable<Key, Classification>*> attributes) {

}

template <typename Key, typename Classification>
vector<Example<Key, Classification> > filter_examples(vector<Example<Key, Classification> > &examples, Variable<Key, Classification>* attribute, Key value) {
	vector<Example<Key, Classification> > new_examples;

	for (vector<Example<Key, Classification> >::iterator it = examples.begin(); it != examples.end(); ++it) {
		for (unsigned int i = 0; i < it->attributes.size(); ++i) {
			//If the attribute has the value we're filtering for then include it in the list
			if (it->attributes.at(i)->get_name() == attribute->get_name()) {

			}
		}
	}

	return new_examples;
}

template <typename Key, typename Classification>
Node<Key, Variable<Key, Classification>*>* decision_tree_learning(vector<Example<Key, Classification> > examples, vector<Variable<Key, Classification>*> attributes, Node<Key, Variable<Key, Classification>*>* default) {
	if (examples.empty()) {
		return default;
	}
	else if (is_same_classification<Key, Classification>(examples)) {
		//We have to nodeify the classification
		Variable<Key, Classification>* classification = new Variable<Key, Classification>("Classification", vector<Key>());
		classification->set_value(examples.at(0).classification);

		return new Node<Key, Variable<Key, Classification>*>(classification);
	}
	else if (attributes.empty()) {
		return new Node<Key, Variable<Key, Classification>*>(most_classified_example<Key, Classification>(examples));
	}
	else {
		Variable<Key, Classification>* best_attribute = choose_best_attribute<Key, Classification>(attributes);
		Node<Key, Variable<Key, Classification>*>* tree = new Node<Key, Variable<Key, Classification>*>(best_attribute);

		for (vector<Key>::iterator it = best_attribute->get_domain().begin(); it != best_attribute->get_domain().end(); ++it) {
			vector<Example<Key, Classification> > filtered_examples = filter_examples<Key, Classification>(examples, best_attribute, *it);
			Node<Key, Variable<Key, Classification>*> default_node = new Node<Key, Variable<Key, Classification>*>(most_classified_example<Key, Classification>(examples));
			
			Node<Key, Variable<Key, Classification>*> subtree = decision_tree_learning(filtered_examples, attributes, default_node);

			tree->add_neighbour(*it, subtree);
		}

		return tree;
	}
}