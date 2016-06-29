/*

Author: Kulunchakov Andrei
*/

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <fstream> 
#include <stdlib.h>
#include "tree_operations.h"
#include "create_data_by_handle.h"

using namespace std;


#include <boost/python/def.hpp>
#include <boost/python/module.hpp>

namespace bp = boost::python;


const int UNFILLED_SIBSTITUTION = -1;
int MINIMUM_CODE_OF_VARIABLES;


vector<int> find_candidates_for_search(vector<int>& encodings, int pattern_root_label) {
  vector<int> canditates_for_search;
  for (int i = 0; i < encodings.size(); ++i) {
    if (encodings[i] == pattern_root_label) {
      canditates_for_search.push_back(i);
    }
  }
  return canditates_for_search;
}

bool check_for_matching(vector<vector<int> >& pattern_matr, vector<int>& pattern_encodings, vector<int>& substitutions, vector<int>& equivalence_classes,
  const vector<vector<int> >& matr, const vector<int>& encodings,  int root, int pattern_root) {
  if (pattern_matr[pattern_root].size() == 0) {  
    if (substitutions[pattern_encodings[pattern_root] - MINIMUM_CODE_OF_VARIABLES] ==  UNFILLED_SIBSTITUTION) {
      substitutions[pattern_encodings[pattern_root] - MINIMUM_CODE_OF_VARIABLES] = equivalence_classes[root];    
    } else {
      if (substitutions[pattern_encodings[pattern_root] - MINIMUM_CODE_OF_VARIABLES] !=  equivalence_classes[root]) {
        return false;
      }
    }
    return true;
  }
  if (encodings[root] != pattern_encodings[pattern_root]) {
    return false;
  }
  // if the roots have the same label, then the corresponding primitives have the same arity
  for (int i = 0; i < matr[root].size(); ++i) {
    if (!check_for_matching(pattern_matr, pattern_encodings, substitutions, equivalence_classes, matr, encodings, matr[root][i], pattern_matr[pattern_root][i])) {
      return false;
    }
  }
return true;
}

vector<int> find_representatives_of_equivalence_classes(vector<int>& equivalence_classes) {
  vector<int> representatives(max_in_vector(equivalence_classes) + 1, -1);
  for (int i = 0; i < equivalence_classes.size(); ++i) {
    if (representatives[equivalence_classes[i]] == -1) {
      representatives[equivalence_classes[i]] = i;
    }
  }
  return representatives;
}



void replaceAll( string &s, const string &search, const string &replace ) {
    for( int pos = 0; ; pos += replace.length() ) {
        // Locate the substring to replace
        pos = s.find( search, pos );
        if( pos == string::npos ) {
          break;
        }
        // Replace by erasing and inserting
        s = eraser(s, pos, search.length() );
        s.insert( pos, replace );
    }
}



string launch_substitute(vector<vector<int> >& model_matr, vector<int>& model_encoding, string replace_handle, vector<int>& substitutions, vector<int>& representatives, 
                        map<string, int>& map_tokens, vector<string>& tokens) {
  pair<int, int> token_counters = find_number_of_tokens(replace_handle);
  pair<vector<vector<int> >, vector<int> > replace_data  = create_incid_matrix_tokens(map_tokens, replace_handle);
  vector<vector<int> > replace_matr = replace_data.first;  
  vector<int> replace_encodings = replace_data.second;
  
  for (int i = 0; i < substitutions.size(); ++i) {
    if (substitutions[i] == UNFILLED_SIBSTITUTION) {
      continue;
    }
    string substring_in_model = string_constructor_unparametred(model_matr, tokens, model_encoding, representatives[substitutions[i]]);
    string var = "x" + SSTR( i );
    cout << "replaceAll = " << replace_handle << " " <<  var << " " << substring_in_model << '\n';
    replaceAll(replace_handle, var, substring_in_model);    
  }  
  return replace_handle;
}

string Simplifier(pair<vector<string>, vector<string> >& rules, string modelhandle) {
	
  pair<map<string, int>, int> tokens_info = read_info_primitives ();
  map<string, int> map_tokens = tokens_info.first;
  MINIMUM_CODE_OF_VARIABLES = tokens_info.second;

  vector<string> rules_pattern = rules.first;
  vector<string> rules_replace = rules.second;

  int number_of_rules = rules_pattern.size();
  
  pair<vector<vector<int> >, vector<int> > model_data;
  model_data = create_incid_matrix_tokens(map_tokens, modelhandle);
  
  vector<vector<int> > model_matr = model_data.first;  
  vector<int> model_encoding = model_data.second;
  vector<int> equivalence_classes = fill_equivalence_classes (model_matr, model_encoding);
  cout << "equivalence_classes == ";
  for (int y = 0; y < equivalence_classes.size(); ++y)  {
    cout << equivalence_classes[y] << ' ';
  }
  vector<int> representatives = find_representatives_of_equivalence_classes(equivalence_classes);
  pair<vector<string>, vector<int> > tokens_data = retrieve_tokens();
  vector<string> tokens = tokens_data.first;
  cout << "number_of_rules = " << number_of_rules << '\n';
  // vector of substitutions for variables from patter trees
  for (int i = 0; i < number_of_rules; ++i) {
    pair<int, int> token_counters = find_number_of_tokens(rules_pattern[i]);
    
    pair<vector<vector<int> >, vector<int> > pattern_data  = create_incid_matrix_tokens(map_tokens, rules_pattern[i]);
    vector<vector<int> > pattern_matr = pattern_data.first;  
    vector<int> pattern_encodings = pattern_data.second;
    vector<int> canditates_for_search = find_candidates_for_search(model_encoding, pattern_encodings[0]);
    
    for (int j = 0; j < canditates_for_search.size(); ++j) {
      // the size of substitutions is equal to the size of variables

      vector<int> substitutions(token_counters.second, UNFILLED_SIBSTITUTION);
      cout << "token_counters= " << token_counters.second <<'\n';
      bool matches = check_for_matching(pattern_matr, pattern_encodings, substitutions, equivalence_classes, model_matr, model_encoding, canditates_for_search[j], 0);
      cout << "substitutions == ";
      for (int y = 0; y < substitutions.size(); ++y)  {
        cout << substitutions[y] << ' ';
      }
      cout << '\n';
      if (matches) {
        cout << "model = " << modelhandle << "; ";
        cout << rules_pattern[i] << " " << rules_replace[i] <<'\n';
        cout << "candidate " << j << "; " << matches <<'\n';

        string inserted_submodel = launch_substitute(model_matr, model_encoding, rules_replace[i], substitutions, representatives, map_tokens, tokens);
        string substituted_string = string_constructor_unparametred(model_matr, tokens, model_encoding, canditates_for_search[j]);
        cout << "inserted_submodel = " << inserted_submodel << "; substituted_string = " << substituted_string <<'\n';
        replaceAll(modelhandle, substituted_string, inserted_submodel);
        model_data = create_incid_matrix_tokens(map_tokens, modelhandle);            
        model_matr = model_data.first;  
        model_encoding = model_data.second;
        equivalence_classes = fill_equivalence_classes (model_matr, model_encoding);
        representatives = find_representatives_of_equivalence_classes(equivalence_classes);     
        // if the model is simplified, the rules rejected before could be applicable now
        // therefore, we check each rule again
        i = 0;        
      }    
    }
    
  }  

  return modelhandle;
}



string simplify_by_rules(const string handle){
	ifstream file_rules_txt;
  	file_rules_txt.open ("data/rules.txt");
  	
  	vector<string> rules_pattern;
  	vector<string> rules_replace;

  	// extract rules from text file
  	string giglet_string;
  	while( file_rules_txt) {
  		file_rules_txt >> giglet_string;
		rules_pattern.push_back(giglet_string);
		file_rules_txt >> giglet_string;
		rules_replace.push_back(giglet_string);
  	}
  	pair<vector<string>, vector<string> > rules = make_pair(rules_pattern, rules_replace);
  	return Simplifier(rules, handle);  	
}

BOOST_PYTHON_MODULE(model_simplifier_by_rules) {
	bp::def("simplify_by_rules", simplify_by_rules);
    	
}

/*
int main(){
  //string s = "plus2_(minus2_(x0,x0),hyperbola_(linear_(parabola_(x0))))";
  //string s = "inv_(hyperbola_(hyperbola_(linear_(parabola_(x0))))))";
  
  cout << s << "-->" << simplify_by_rules(s) << '\n';
  return 0;
}

*/
