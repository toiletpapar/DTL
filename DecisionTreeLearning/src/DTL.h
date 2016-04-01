#ifndef DTL_H
#define DTL_H

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "Graph.h"

using namespace std;

template <class Key>
class Example {
public:
	vector<Variable<Key>*> attributes;
	vector<double> attribute_instantiation;	//We'll use double here since it differs from the domain of the actual attributes (which is bool representing below or above threshold)
	bool classification;
};

template <typename Key>
bool is_same_classification(vector<Example<Key> > examples) {
	if (!examples.empty()) {
		bool klass = examples.at(0).classification;

		for (unsigned int i = 1; i < examples.size(); i++) {
			if (klass != examples.at(i).classification) {
				return false;
			}
		}
	}
	
	return true;
}

template <typename Key>
bool most_classified(vector<Example<Key> > examples) {
	//return the classification with the highest number of occurrences in examples
	map<bool, int> classification_occurrences;

	for (unsigned int i = 0; i < examples.size(); ++i) {
		bool classification = examples.at(i).classification;
		classification_occurrences[classification] = classification_occurrences[classification] + 1;
	}

	pair<bool, int> max = *classification_occurrences.begin();
	for (map<bool, int>::iterator it = next(classification_occurrences.begin(), 1); it != classification_occurrences.end(); it++) {
		if (max.second < it->second) {
			max = *it;
		}
	}

	return max.first;
}

template <class Key>
class ExampleSort {
public:
	ExampleSort(int index_of_attribute_to_sort_by) : index_of_attribute_to_sort_by(index_of_attribute_to_sort_by) {}
	bool operator() (Example<Key> e1, Example<Key> e2) {
		return e1.attribute_instantiation.at(this->index_of_attribute_to_sort_by) < e2.attribute_instantiation.at(this->index_of_attribute_to_sort_by);
	}
private:
	int index_of_attribute_to_sort_by;
};

template <typename Key>
int index_of_attribute(Example<Key> example, Variable<Key>* attribute) {
	int index = -1;

	//Breaks when the number of attributes is greater than what int can hold
	for (int i = 0; i < example.attributes.size(); ++i) {
		if (example.attributes.at(i)->get_name() == attribute->get_name()) {
			index = i;
		}
	}

	return index;
}

//Can definitely be broken up into more logical parts
template <typename Key>
pair<Variable<Key>*, double> choose_best_attribute(vector<Variable<Key>*> attributes, vector<Example<Key> > examples) {
	//Calculate the number of positive/negative examples in the entire training set
	double positive_examples = 0;
	double negative_examples = 0;
	
	for (unsigned long j = 0; j < examples.size(); j++) {
		if (examples.at(j).classification) {
			++positive_examples;
		}
		else {
			++negative_examples;
		}
	}

	//Calculate the entropy of the entire training set
	unsigned long all_examples = positive_examples + negative_examples;
	double entropy;
	if (positive_examples != 0 && negative_examples != 0) {
		entropy = ((-1.0 * positive_examples / all_examples) * log2(positive_examples / all_examples)) - ((negative_examples / all_examples) * log2(negative_examples / all_examples));
	}
	else {
		entropy = 0;
	}

	Variable<Key>* best_attribute = attributes.at(0);
	double best_attribute_threshold = 0.0;
	double best_attribute_information_gain = 0.0;

	//Pick an attribute
	for (vector<Variable<Key>*>::iterator attribute = attributes.begin(); attribute != attributes.end(); attribute++) {
		//Pick a threshold
		//1. Order the examples based on the selected attribute
		//Find the index of the selected attribute
		int index = index_of_attribute(examples.at(0), *attribute); //Assume that each example has the same ordering for attributes;

		//Assume that you've found attribute in the example (Otherwise something has been formatted wrong)
		if (index == -1) {
			//Something went horribly wrong
			cout << "Bad formatting" << endl;
		}
		else {
			ExampleSort<Key> example_sort = ExampleSort<Key>(index);

			//Order the examples by the index
			sort(examples.begin(), examples.end(), example_sort);
		}

		//Going to assume we sorted and we didn't have bad formatting
		//2. Pick threshold values that are halfway between each example with respect to the selected attribute
		vector<double> thresholds;
		//For each example calculate the threshold
		for (vector<Example<Key> >::iterator example = examples.begin() + 1; example != examples.end(); example++) {
			Example<Key> prev_example = *(example - 1);
			Example<Key> curr_example = *example;

			//The threshold is the midpoint between these two examples with respect to the attribute
			thresholds.push_back(prev_example.attribute_instantiation.at(index) / 2 + curr_example.attribute_instantiation.at(index) / 2);
		}

		//3. Pick the threshold that provides the highest information gain
		double max_information_gain = 0;
		double best_threshold = 0;
		for (unsigned int i = 0; i < thresholds.size(); i++) {
			double threshold = thresholds.at(i);

			//Divide the training set into two example sets based on the threshold value (instead of using the attribute domain)
			vector<Example<Key> > examples_below_threshold;
			vector<Example<Key> > examples_above_threshold;

			for (unsigned long j = 0; j < examples.size(); j++) {
				if (examples.at(j).attribute_instantiation.at(index) < threshold) {
					examples_below_threshold.push_back(examples.at(j));
				}
				else {
					examples_above_threshold.push_back(examples.at(j));
				}
			}
			
			//For each example set categorized by the attribute value (in this case above or below threshold value)
			//Count the number of positive and negative examples for each example set
			unsigned long positive_examples_below = 0;
			unsigned long negative_examples_below = 0;

			for (unsigned long j = 0; j < examples_below_threshold.size(); j++) {
				if (examples_below_threshold.at(j).classification) {
					++positive_examples_below;
				}
				else {
					++negative_examples_below;
				}
			}

			unsigned long positive_examples_above = 0;
			unsigned long negative_examples_above = 0;

			for (unsigned long j = 0; j < examples_above_threshold.size(); j++) {
				if (examples_above_threshold.at(j).classification) {
					++positive_examples_above;
				}
				else {
					++negative_examples_above;
				}
			}

			//Calculate the information gain for this threshold
			double all_examples_below = positive_examples_below + negative_examples_below;
			double all_examples_above = positive_examples_above + negative_examples_above;

			double entropy_below;
			if (positive_examples_below != 0 && negative_examples_below != 0) {
				entropy_below = ((-1.0 * positive_examples_below / all_examples_below) * log2(positive_examples_below / all_examples_below)) - ((negative_examples_below / all_examples_below) * log2(negative_examples_below / all_examples_below));	//The entropy for values below the threshold
			}
			else {
				entropy_below = 0;
			}

			double entropy_above;
			if (positive_examples_above != 0 && negative_examples_above != 0) {
				entropy_above = ((-1.0 * positive_examples_above / all_examples_above) * log2(positive_examples_above / all_examples_above)) - ((negative_examples_above / all_examples_above) * log2(negative_examples_above / all_examples_above));	//The entropy for values above the threshold
			}
			else {
				entropy_above = 0;
			}

			double remainder = ((positive_examples_below + negative_examples_below) / (positive_examples + negative_examples)) * entropy_below +
				((positive_examples_above + negative_examples_above) / (positive_examples + negative_examples)) * entropy_above; //Sum of proportional entropy
			double information_gain = entropy - remainder;

			if (max_information_gain < information_gain) {
				max_information_gain = information_gain;
				best_threshold = threshold;
			}
		}

		//Test whether the best_threshold for this attribute is better than the best_attribute's best_threshold with respect to information gain;
		if (best_attribute_information_gain < max_information_gain) {
			best_attribute = *attribute;
			best_attribute_information_gain = max_information_gain;
			best_attribute_threshold = best_threshold;
		}
	}

	//Return the attribute and threshold that provides the highest information gain
	return pair<Variable<Key>*, double>(best_attribute, best_attribute_threshold);
}

template <typename Key>
vector<Example<Key> > filter_examples(vector<Example<Key> > examples, pair<Variable<Key>*, double> attribute, Key value) {
	//Return examples that are above or below (given) the threshold
	//We require that the domain of all variables (i.e. the keys) will be {"above", "below"} to indicate whether we should divide the training set above or below the threshold
	vector<Example<Key> > new_examples;

	//Find the index of the attribute
	int index = index_of_attribute(examples.at(0), attribute.first);

	//Assume index isn't -1 or something went horribly wrong because we can't find attribute in the example
	if (index != -1) {
		for (vector<Example<Key> >::iterator example = examples.begin(); example != examples.end(); example++) {
			if ((value == "above" && example->attribute_instantiation.at(index) >= attribute.second) ||
				(value == "below" && example->attribute_instantiation.at(index) < attribute.second)) {
				new_examples.push_back(*example);
			}
		}
	}
	else {
		//or else something went horribly wrong...
		cout << "Variables didn't just contain above and below, my assumption is faulty :(" << endl;
		return examples;
	}
	
	return new_examples;
}

//A modified version of DTL for continuous variables
template <typename Key>
Node<Key, bool>* decision_tree_learning(vector<Example<Key> > examples, vector<Variable<Key>*> attributes, Node<Key, bool>* default) {
	if (examples.empty()) {
		return default;
	}
	else if (is_same_classification<Key>(examples)) {
		return new Node<Key, bool>(examples.at(0).classification);
	}
	else if (attributes.empty()) {
		return new Node<Key, bool>(most_classified<Key>(examples));
	}
	else {
		pair<Variable<Key>*, double> best_attribute = choose_best_attribute<Key>(attributes, examples);
		Node<Key, bool>* tree = new Node<Key, bool>(false, best_attribute.first);
		tree->set_threshold(best_attribute.second);

		for (vector<Key>::iterator it = best_attribute.first->get_domain().begin(); it != best_attribute.first->get_domain().end(); ++it) {
			vector<Example<Key> > filtered_examples = filter_examples<Key>(examples, best_attribute, *it);
			Node<Key, bool>* default_node = new Node<Key, bool>(most_classified<Key>(examples));
			
			Node<Key, bool>* subtree = decision_tree_learning(filtered_examples, attributes, default_node);

			tree->add_neighbour(*it, subtree);
		}

		return tree;
	}
}

#endif