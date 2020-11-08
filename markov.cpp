#include "trigram_count.cpp"
#include "trie_generic.cpp"
#include<unordered_map>
#include<iostream>
#include<algorithm>
#include<float.h>
#include<chrono>
#include<type_traits>

void findPrintWordsFromRoot(GenericTrie::PrefixTree<char,double> &tree, std::string query){
    auto query_vec = vector<char>(query.begin(),query.end());
    auto matches = tree.possibleWords(std::vector<char>(query.begin(),query.end()));
    std::cout<<"Finding all possible words (without errors) starting with "<<query<<"\n";
    for(auto leaf : matches){
        for(auto c : leaf.value)
            std::cout<<c;
        std::cout<<" P="<<(double)leaf.end_prob<<std::endl;
    }
}
template<typename T,typename P>
void findPrintAllWordProbs(GenericTrie::PrefixTree<T,P> &tree, std::vector<T> query_vec, unsigned int n_most_probable_words){
    // auto query_vec = vector<T>(query.begin(),query.end());
    auto results = tree.findPossibleChildren(query_vec,0.8,0.15,0.05,(double)1e-5,true); //hyper-parameters to be tuned...


    //now we sort results by p(c^(i),w_i)
    std::vector<std::pair<P,unsigned int>> hr_probs;

    for(unsigned long i=0;i<results.size();i++){
        if(results[i].running_prob>1e-8){ //threshold by probability--another hyperparameter
            hr_probs.push_back({results[i].running_prob*results[i].word_prob,i});
        }
    }    
    
    std::sort(hr_probs.begin(),hr_probs.end(),[](auto a, auto b){return a.first<b.first;}); 

    std::vector<std::pair<P,unsigned int>> top_ranked_probs;//truncate to most likely items if necessary
    if(hr_probs.size()>n_most_probable_words) 
        top_ranked_probs = std::vector<std::pair<P,unsigned int>>(hr_probs.end()-n_most_probable_words,hr_probs.end());
    else top_ranked_probs = hr_probs;
    
    for(auto prob_ind : top_ranked_probs){//and output with character likelihood and word frequency
        auto result = results[prob_ind.second];
        for(auto c : result.trace_of_current_state){
            std::cout<<c;
        }
        std::cout<<" p(c^(i)|w_i) = "<<result.running_prob << ", p(w(i)) = "<<result.word_prob<<"\n";
    }
}



int main(){//these are just experiments for now...
    
    GenericTrie::PrefixTree<char,double> words;
    
    auto dw = WordCountGenerator::countDickensWords();
    unsigned long total_words =0;
    for(auto word_count_pair : dw)
        total_words+=word_count_pair.second;
    for(auto word_count_pair : dw){
        auto leaf = GenericTrie::PrefixTree<char,double>::Leaf(std::vector<char>(word_count_pair.first.begin(),word_count_pair.first.end()),(double)word_count_pair.second/total_words);
        words.insert(leaf);
    }

    std::string query {"princdple"};

    findPrintWordsFromRoot(words,query); //print the words which could result from the present stem without errors

    std::vector<char> query_vec(query.begin(),query.end());
        
    auto t1 = std::chrono::high_resolution_clock::now();//Time the probabilistic test
    findPrintAllWordProbs(words,query_vec,5);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();//https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c
    std::cout<<"Query took "<<duration<<"us"<<std::endl;




    GenericTrie::PrefixTree<string,double> trigrams;
    auto trigram_counts = WordCountGenerator::countDickensTrigrams();
    unsigned int sum = 0;
    for(auto c : trigram_counts){
        sum+=c.second;
    }

    for(auto trigram_pair : trigram_counts){
        auto leaf = GenericTrie::PrefixTree<std::string,double>::Leaf(trigram_pair.first,trigram_pair.second);
        leaf.end_prob = (double)trigram_pair.second/sum;
        trigrams.insert(leaf);
    }
    std::vector<std::string> query_tri {"Oliver","Twist"};
    findPrintAllWordProbs(trigrams,query_tri,10);









    return 0;
} 


