#include "trigram_count.cpp"
#include "trie_generic.cpp"
#include<unordered_map>
#include<iostream>
#include<algorithm>
#include<float.h>
#include<chrono>
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
    std::string query {"clculate"};

    auto matches = words.possibleWords(std::vector<char>(query.begin(),query.end()));
    std::cout<<"Querying root "<<query<<std::endl;
    for(auto leaf : matches){
        for(auto c : leaf.value)
            std::cout<<c;
        
        std::cout<<" P="<<(double)leaf.end_prob/total_words<<std::endl;
    }

    //now to explore for a more practical example

    auto query_vec = vector<char>(query.begin(),query.end());
    auto t1 = std::chrono::high_resolution_clock::now();
    auto results = words.findPossibleChildren(query_vec,0.9,0.05,0.05,(double)1e-5);
    std::cout<<results.size()<<std::endl;
    std::vector<std::pair<std::vector<char>,double>> high_results;
    for(unsigned long i=0;i<results.size();i++){
        if(results[i].running_prob > -10 ){
            high_results.push_back({results[i].trace_of_current_state,results[i].running_prob});
        }
    }    

    std::sort(high_results.begin(),high_results.end(),[](std::pair<std::vector<char>,double> a, std::pair<std::vector<char>,double> b){return a.second<b.second;});
    auto t2 = std::chrono::high_resolution_clock::now();
    for(auto result_pair : high_results){
    for (auto c : result_pair.first){
        std::cout<<c;
    }
    std::cout<<" p="<<result_pair.second<<std::endl;
    }
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();//https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c

    std::cout<<"Query took "<<duration<<"us"<<std::endl;
    return 0;
} 


