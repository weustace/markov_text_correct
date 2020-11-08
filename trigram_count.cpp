using namespace std;
#include<iostream>
#include<unordered_map>
#include<vector>
#include<fstream>
#include<istream>
#include<cctype>
#include<algorithm>
#include<string>

namespace WordCountGenerator {
    const vector<string> dickens_books {"dickens/twist.txt","dickens/twocities.txt","dickens/copperfield.txt","dickens/greatexpectations.txt"};


    vector<string> getDickensWords(){
    //return a vector of all the words appearing in the Dickens works in dickens_books

        vector<string> words;
        for(auto title : dickens_books){
            cout<<"Reading "<<title<<endl;
            ifstream book(title);
            if(book.is_open()){
                book.seekg(0,ios_base::end);
                string input(book.tellg(),'.');
                book.seekg(0,ios_base::beg);
                book.read(&input[0],input.size());
                book.close();
                //now to split up the input into words, replacing all punctuation with spaces
                int last_ind = 0;
                for(int i=0;i<input.size();i++){
                    auto c = input[i];
                    if(ispunct(c)||isspace(c)){
                        if(i>last_ind+1){
                            string str = input.substr(last_ind,i-last_ind);
                            transform(str.begin(),str.end(),str.begin(),[](char letter)-> char{ return tolower(letter);});
                            words.push_back(str);
                        }
                        last_ind = i+1;
                    }
                }

            }
        }

        return words;
    }

    auto str_vec_hash = [](const auto &v){std::string r;for(auto w:v) r+=w+" ";return std::hash<string>{}(r);}; //hash function for trigrams: converts {"foo","bar","baz"} into "foo bar baz " and hashes
    std::unordered_map<std::vector<string>,unsigned int,decltype(str_vec_hash)> countDickensTrigrams(){
        std::unordered_map<std::vector<string>,unsigned int,decltype(str_vec_hash)> trigram_count(1000,str_vec_hash);//arbitrary default bucket size
        auto words = getDickensWords();
        for(int i=0;i<words.size()-2;i++){
            string trigram = words[i];
            vector<string> trigram_vec  {words[i]};

            for(int j=1;j<3;j++){
                trigram+= " "+ words[i+j];
                trigram_vec.push_back(words[i+j]);
            }
            if(trigram_count.count(trigram_vec)){
                trigram_count[trigram_vec] += 1;
            }
            else{
                trigram_count.insert({trigram_vec,1});
            }
        }

        // cout<<trigram_count.size()<<endl;
        const unsigned int thresh = 3;
        std::unordered_map<std::vector<string>,unsigned int,decltype(str_vec_hash)> common(1000,str_vec_hash);//arbitrary default bucket size
        for(auto trigram_with_count : trigram_count){
            if(trigram_with_count.second > thresh){ 
                common.insert(trigram_with_count);
            }

        }
        // for(auto c : common){
        //     cout<<c.first << ": "<<c.second<<endl;
        // }

        // cout<<"Found "<<common.size()<<" trigrams with >"<<thresh<<" occurrences."<<endl;
        return common;

    }



    std::unordered_map<std::string,unsigned int> countDickensWords(){
        
        unordered_map<string,unsigned int> trigram_count;
        unordered_map<string,unsigned int> word_count;
        unsigned int total_unique_words=0;
        
        // cout<<words.size()<<" words found"<<endl;

        auto words=getDickensWords();

        for(int i=0;i<words.size();i++){
            string word = words[i];

            if(word_count.count(word))//add to the word counts dict
                word_count[word]+=1;
            else
                word_count.insert({word,1});
        }

        // cout<<"Found "<<word_count.size()<<" unique words."<<endl;

        // cout<<words.size()<<" total word instances."<<endl;
        

        return word_count;
        // return 0;
    }

}