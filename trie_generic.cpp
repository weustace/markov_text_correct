#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>
#include<vector>
#include<memory>

namespace GenericTrie {
template<typename T,typename P>
struct Node{
        bool terminates=false;
        P end_probability;

        std::unordered_map<T,std::unique_ptr<Node<T,P>>> children;
    };
template<typename T,typename P>
class PrefixTree {
    public:

    struct Leaf {
        std::vector<T> value;
        P end_prob;
        Leaf(const std::vector<T> value) : value {value} {}; 
        Leaf(const std::vector<T> value, const P end_prob) : value {value}, end_prob {end_prob} {}; 
        
        Leaf operator=(const Leaf &leaf){//Copy constructor.
            return Leaf(leaf.value,leaf.end_prob);
        };
    };

    PrefixTree() {
        this->root = std::unique_ptr<Node<T,P>>(new Node<T,P>());
    }


    void insert(const Leaf &item) {
        auto current = &(this->root);
        for(T c : item.value){
            if(!current->get()->children.count(c))
                current->get()->children.insert(std::make_pair(c,std::unique_ptr<Node<T,P>>(new Node<T,P>())));

            current = &(current->get()->children[c]);
        }
        current->get()->terminates = true;
        current->get()->end_probability = item.end_prob;
    }


    std::vector<Leaf> possibleWords(const std::vector<T> &partial){
        std::vector<Leaf> matches;

        auto current = &(this->root);

        for(T c : partial){ //traverse down to the current word
            if(!current->get()->children.count(c))
                return matches; //no matches
            else
                current = &(current->get()->children[c]); //descend to the final node
        }
        if(current->get()->terminates){ //if current partial string is a possibility
            Leaf current_match = Leaf(partial);
            current_match.end_prob = current->get()->end_probability;
            matches.push_back(current_match);
        }

        possibleEndings(Leaf(partial),current,matches);

        return matches;
    }

    
    private:

    std::unique_ptr<Node<T,P>> root;

    void possibleEndings(Leaf root,std::unique_ptr<Node<T,P>> *parent,std::vector<Leaf> &matches){
        //Descend from the given parent node and add root+{all child paths} to the given vector of strings.

        if(!parent->get()->children.size()) return;//end of branch
        // Node *ptr = parent->get();
        // ptr->children['c'];
        // std::unordered_map<char,std::unique_ptr<Node>> new_child = *(parent->get()->children);
        for(auto &child_pair : parent->get()->children){
            Leaf curr_string(root.value);
            curr_string.value.push_back(child_pair.first);

            if(child_pair.second->terminates){
                curr_string.end_prob = child_pair.second->end_probability;
                matches.push_back(curr_string);
            }

            possibleEndings(curr_string,&(child_pair.second),matches);
        }
    }
};
}