#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>
#include<vector>
#include<memory>
    struct Node{
        bool terminates=false;
        std::unordered_map<char,std::unique_ptr<Node>> children;

    };


class PrefixTree {
    public:

    PrefixTree() {
        this->root = std::unique_ptr<Node>(new Node());
    }


    void insert(const std::string word) {
        auto current = &(this->root);
        for(char c : word){
            if(!current->get()->children.count(c)){
                current->get()->children.insert(std::make_pair(c,std::unique_ptr<Node>(new Node())));

            }

            current = &(current->get()->children[c]);
        }
        current->get()->terminates = true;
    }


    std::vector<std::string> possibleWords(const std::string partial_word){
        std::vector<std::string> matches;

        auto current = &(this->root);

        for(char c : partial_word){ //traverse down to the current word
            if(!current->get()->children.count(c))
                return matches; //no matches
            else
                current = &(current->get()->children[c]); //descend to the final node
        }
        if(current->get()->terminates) //if current partial string is a possibility
            matches.push_back(partial_word);

        possibleEndings(partial_word,current,matches);

        return matches;
    }
    private:



    std::unique_ptr<Node> root;

    void possibleEndings(std::string root,std::unique_ptr<Node> *parent,std::vector<std::string> &matches){
        //Descend from the given parent node and add root+{all child paths} to the given vector of strings.

        if(!parent->get()->children.size()) return;//end of branch
        // Node *ptr = parent->get();
        // ptr->children['c'];
        // std::unordered_map<char,std::unique_ptr<Node>> new_child = *(parent->get()->children);
        for(auto &child_pair : parent->get()->children){
            auto curr_string = root+child_pair.first;

            if(child_pair.second->terminates)
                matches.push_back(curr_string);

            possibleEndings(curr_string,&(child_pair.second),matches);
        }
    }
};




int main(){
    Node x;
    x.terminates=true;
    std::cout<<x.terminates<<std::endl;
    PrefixTree pt;

    std::ifstream file("english-words/words.txt");
    std::string line;
    while(getline(file,line))
        if(line!="")
            pt.insert(line);
    file.close();


    std::string input = "twitche";
    std::cout<<"With input "<< input<<", generated possibilities:\n";

    for(auto  word : pt.possibleWords(input)){
        std::cout<<word<<std::endl;
    }

    return 0;
}