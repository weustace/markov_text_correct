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
std::unordered_map<std::string,unsigned int> countDickensWords(){
    vector<string> books {"dickens/twist.txt","dickens/twocities.txt","dickens/copperfield.txt","dickens/greatexpectations.txt"};
    vector<string> words;
    unordered_map<string,unsigned int> trigram_count;
    unordered_map<string,unsigned int> word_count;
    unsigned int total_unique_words=0;
    for(auto title : books){
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

    // cout<<words.size()<<" words found"<<endl;


    for(int i=0;i<words.size()-2;i++){
        string trigram = words[i];

        if(word_count.count(trigram))//add to the word counts dict
            word_count[trigram]+=1;
        else
            word_count.insert({trigram,1});

        // for(int j=1;j<3;j++){
        //     trigram+= " "+ words[i+j];
        // }
        // if(trigram_count.count(trigram)){
        //     trigram_count[trigram] += 1;
        // }
        // else{
        //     trigram_count.insert({trigram,1});
        // }
    }

    for(int i=words.size()-3;i<words.size();i++){//last three words into the word count dict...
        if(word_count.count(words[i]))
            word_count[words[i]]+=1;
        else
            word_count.insert({words[i],1});

    }

    // cout<<trigram_count.size()<<endl;
    // const unsigned int thresh = 10;
    // unordered_map<string,unsigned int> common;
    // for(auto trigram_with_count : trigram_count){
    //     if(trigram_with_count.second > thresh){ 
    //         common.insert(trigram_with_count);
    //     }

    // }
    // for(auto c : common){
    //     cout<<c.first << ": "<<c.second<<endl;
    // }

    // cout<<"Found "<<common.size()<<" trigrams with >"<<thresh<<" occurrences."<<endl;
    // cout<<"Found "<<word_count.size()<<" unique words."<<endl;

    // cout<<words.size()<<" total word instances."<<endl;
    

    return word_count;
    // return 0;
}

}