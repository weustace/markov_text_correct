#include "trigram_count.cpp"
#include "trie_generic.cpp"
#include<unordered_map>
#include<iostream>
int main(){
    
    GenericTrie::PrefixTree<char,unsigned long> words;
    
    auto dw = WordCountGenerator::countDickensWords();
    unsigned long total_words =0;
    for(auto word_count_pair : dw){
        
        auto leaf = GenericTrie::PrefixTree<char,unsigned long>::Leaf(std::vector<char>(word_count_pair.first.begin(),word_count_pair.first.end()),word_count_pair.second);
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

    

    return 0;
} 


