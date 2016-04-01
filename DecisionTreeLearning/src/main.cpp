#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Graph.h"
#include "DTL.h"

using namespace std;

//Examples with classification
vector<Example<string> > read_file(vector<Variable<string>*> variables) {
	const string filename = "horseTrain.txt";
	fstream fs = fstream(filename, fstream::in);

	vector<Example<string> > examples;

	while (!fs.eof()) {
		Example<string> example;
		example.attributes = variables;
		example.attribute_instantiation = vector<double>(16);

		for (unsigned int i = 0; i < 16; i++) {
			fs >> example.attribute_instantiation.at(i);
		}

		string string_classification;
		fs >> string_classification;

		//Map classification to true or false
		if (string_classification == "healthy") {
			example.classification = true;
		}
		else {
			example.classification = false;
		}

		examples.push_back(example);
	}

	return examples;
}

//Examples without classification
vector<Example<string> > read_file_test(vector<Variable<string>*> variables) {
	const string filename = "horseTest.txt";
	fstream fs = fstream(filename, fstream::in);

	vector<Example<string> > examples;

	while (!fs.eof()) {
		Example<string> example;
		example.attributes = variables;
		example.attribute_instantiation = vector<double>(16);

		for (unsigned int i = 0; i < 16; i++) {
			fs >> example.attribute_instantiation.at(i);
		}

		string string_classification;
		fs >> string_classification;

		examples.push_back(example);
	}

	return examples;
}

vector<Example<string> > classify(Node<string, bool>* decision_tree, vector<Example<string> > examples_to_classify) {
	if (!decision_tree->is_leaf()) {
		if (!examples_to_classify.empty()) {
			int index = index_of_attribute(examples_to_classify.at(0), decision_tree->get_variable());
			if (index != -1) {
				vector<Example<string> > above_examples = classify(decision_tree->get_neighbour("above"), filter_examples<string>(examples_to_classify, pair<Variable<string>*, double>(decision_tree->get_variable(), decision_tree->get_threshold()), "above"));
				vector<Example<string> > below_examples = classify(decision_tree->get_neighbour("below"), filter_examples<string>(examples_to_classify, pair<Variable<string>*, double>(decision_tree->get_variable(), decision_tree->get_threshold()), "below"));

				above_examples.insert(above_examples.end(), below_examples.begin(), below_examples.end());

				return above_examples;
			}
			else {
				cout << "Blow up" << endl;
				return vector<Example<string> >();
			}
		}
	}
	else {
		for (unsigned int i = 0; i < examples_to_classify.size(); i++) {
			examples_to_classify.at(i).classification = decision_tree->get_data();
		}

		return examples_to_classify;
	}
}

int main() {
	vector<string> domain = vector<string>({ "above", "below" });
	vector<Variable<string>*> variables = vector<Variable<string>*>({
		new Variable<string>("K", domain),
		new Variable<string>("Na", domain),
		new Variable<string>("CL", domain),
		new Variable<string>("HCO3", domain),
		new Variable<string>("Endotoxin", domain),
		new Variable<string>("Aniongap", domain),
		new Variable<string>("PLA2", domain),
		new Variable<string>("SDH", domain),
		new Variable<string>("GLDH", domain),
		new Variable<string>("TPP", domain),
		new Variable<string>("Breath rate", domain),
		new Variable<string>("PCV", domain),
		new Variable<string>("Pulse Rate", domain),
		new Variable<string>("Fibrinogen", domain),
		new Variable<string>("Dimer", domain),
		new Variable<string>("FibPerDim", domain)
	});

	vector<Example<string> > examples = read_file(variables);
	Node<string, bool>* default_node = new Node<string, bool>(false);

	Node<string, bool>* decision_tree = decision_tree_learning(examples, variables, default_node);

	Node<string, bool>::print_graph(decision_tree, 0);

	cout << "Classifying HorseTest" << endl;

	vector<Example<string> > examples_test = read_file_test(variables);
	vector<Example<string> > classified_test = classify(decision_tree, examples_test);

	//print classified examples
	for (unsigned int i = 0; i < classified_test.size(); ++i) {
		Example<string> example = classified_test.at(i);

		for (unsigned int j = 0; j < example.attributes.size(); ++j) {
			cout << example.attribute_instantiation.at(j) << " ";
		}
		cout << example.classification << endl;
	}
	return 0;
}