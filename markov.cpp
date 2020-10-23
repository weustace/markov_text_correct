#include "trigram_count.cpp"
#include "trie_generic.cpp"
#include<unordered_map>
#include<iostream>
#include<algorithm>
#include<float.h>
int main(){
    
    GenericTrie::PrefixTree<char,double> words;
    
    auto dw = WordCountGenerator::countDickensWords();
    unsigned long total_words =0;
    for(auto word_count_pair : dw){
        
        auto leaf = GenericTrie::PrefixTree<char,double>::Leaf(std::vector<char>(word_count_pair.first.begin(),word_count_pair.first.end()),word_count_pair.second);
        words.insert(leaf);
        total_words += word_count_pair.second;
    }

    //now try querying
    std::string query {"cal"};

    auto matches = words.possibleWords(std::vector<char>(query.begin(),query.end()));
    std::cout<<"Querying root "<<query<<std::endl;
    for(auto leaf : matches){
        for(auto c : leaf.value)
            std::cout<<c;
        
        std::cout<<" P="<<(double)leaf.end_prob/total_words<<std::endl;
    }

    //now to explore for a more practical example

    auto query_vec = vector<char>(query.begin(),query.end());

    auto results = words.findPossibleChildren(query_vec,0.95,0.001,0.0499,(double)-100000.0);
    std::cout<<results.size()<<std::endl;
    double min_prob = DBL_MAX;
    unsigned long ind_of_max;
    unsigned long ind_of_min;
    double max_prob = -DBL_MAX;
    for(unsigned long i=0;i<results.size();i++){
        if(results[i].running_prob < min_prob){
            min_prob = results[i].running_prob;
            ind_of_min = i;
        }
        if(results[i].running_prob > max_prob && results[i].trace_of_current_state.size()!=0){
            max_prob = results[i].running_prob;
            ind_of_max = i;
        }
    }    
    std::cout<<min_prob<<std::endl;
    std::cout<<max_prob<<std::endl;
    for (auto c : results[ind_of_max].trace_of_current_state){
        std::cout<<c<<std::endl;
    }
    return 0;
} 


