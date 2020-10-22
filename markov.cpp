#include "trigram_count.cpp"
#include "trie_generic.cpp"
#include<unordered_map>
#include<iostream>
int main(){
    GenericTrie::PrefixTree<std::string,unsigned long> words;
    auto dw = WordCountGenerator::countDickensWords();
    unsigned long total_words =0;
    for(auto word_count_pair : dw){
        auto leaf = GenericTrie::PrefixTree<std::string,unsigned long>::Leaf(GenericTrie::stringToStringArr(word_count_pair.first),word_count_pair.second);
        words.insert(leaf);
        total_words += word_count_pair.second;
    }

    //now try querying
    std::string query {"try"};

    auto matches = words.possibleWords(GenericTrie::stringToStringArr(query));
    std::cout<<"Querying root "<<query<<std::endl;
    for(auto leaf : matches){
        for(auto c : leaf.value)
            std::cout<<c;
        
        std::cout<<" P="<<(double)leaf.end_prob/total_words<<std::endl;
    }


    return 0;
} 


