#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>
#include<vector>
#include<memory>
#include<math.h>
#include<assert.h>

namespace GenericTrie {
template<typename T,typename P>
struct Node{
        bool terminates=false;
        P end_probability;

        std::unordered_map<T,std::unique_ptr<Node<T,P>>> children;
    };
template<typename T,typename P>
class PrefixTree {
    //NOTE on P:
    //We use value initialisation implicitly for P. If using a custom distribution object, P() should produce whatever the equivalent of zero for log-prob or 1 for normal prob is...
    //(i.e. something which can be multiplied by other probs). 
    
    public:

    struct Leaf {
        std::vector<T> value;
        P end_prob=1;
        Leaf(const std::vector<T> value) : value {value} {}; 
        Leaf(const std::vector<T> value, const P end_prob) : value {value}, end_prob {end_prob} {}; 
        
        Leaf operator=(const Leaf &leaf){//Copy constructor.
            return Leaf(leaf.value,leaf.end_prob);
        };
    };


    struct PossibleState {
        P running_prob=1; 
        P word_prob=1;
        vector<T> trace_of_current_state;
        PossibleState(const P prob,const vector<T> trace,const P word_probability) :  running_prob {prob}, trace_of_current_state {trace}, word_prob {word_probability} {};

        PossibleState() {}; 
        PossibleState operator=(const PossibleState &state){
            return PossibleState(state.running_prob,state.trace_of_current_state,state.word_prob);
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


    Node<T,P>* traverseTo(const std::vector<T> &word){
        auto current = &(this->root);
        for(T c : word){
            if(!current->get()->children.count(c))
                return nullptr;
            else current = &(current->get()->children[c]);
        }
        return current->get();
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
    

    //partial: word to try matching
    //Pc: probability of typing char correctly
    //Pi: probability of inserting an unneeded char
    //Pm: probability of typing a char wrong in place.
    std::vector<PossibleState> findPossibleChildren(const std::vector<T> partial,const P Pc, const P Pi, const P Pm, const P prob_thresh,const bool space_inserted,const unsigned int max_missing_chars=3){   
        std::unordered_map<Node<T,P>*,PossibleState> current_states;

        current_states.insert({this->root.get(),PossibleState()});
        insertPossibleChildStates(current_states,this->root.get(),Pi,max_missing_chars);


        for(T c : partial){//For each char in input
            std::vector<Node<T,P>*> to_process;
            for(auto pair : current_states)//need to find a better way of doing this...expensive!
                to_process.push_back(pair.first);

            for(auto key : to_process){//(this is  a hack to get around iterator invalidation on current_states...)
                auto state = current_states[key];
                if(state.running_prob < prob_thresh){
                    current_states.erase(key);
                    continue;
                }

                if(key->children.count(c)) {//case with matching child  
                    auto child_elem =(key->children[c]).get();

                    if(current_states.count(child_elem)){
                        current_states[child_elem].running_prob += state.running_prob*Pc;
                    }
                    else{
                        auto new_state = state;
                        new_state.running_prob *= Pc;//Correct char
                        new_state.trace_of_current_state.push_back(c);
                        if(new_state.running_prob>prob_thresh)
                            current_states.insert({child_elem,new_state});
                    }
                    
                }   
                addChildrenOfState(current_states,key,Pm,prob_thresh,c);
                current_states[key].running_prob *= Pi; //this char shouldn't have been typed at all  
                insertPossibleChildStates(current_states,key,Pi,max_missing_chars);      
            }                
        }

        // We only want to return complete words. We should have added all possible child states max_missing_chars ahead during the last pass,
        // so if the word is complete, our job is done, otherwise we look a further 5 chars ahead from current states.
        if(!space_inserted){
            std::vector<Node<T,P>*> to_process;
            for(auto pair : current_states)//need to find a better way of doing this...expensive!
                to_process.push_back(pair.first);
            for(auto key : to_process){
                insertPossibleChildStates(current_states,key,1,5);
            }
        }

        
        std::vector<PossibleState> output;
        
        for(auto pair : current_states){
            if(pair.first->terminates && pair.second.running_prob > prob_thresh){
                auto output_state = pair.second;
                output_state.word_prob = pair.first->end_probability;
                // output_state.running_prob *= pair.first->end_probability;
                output.push_back(output_state);
            }
        }
        // for(auto pair : current_states)
        //     output.push_back(pair.second);
        return output;

    }
    
    private:

    std::unique_ptr<Node<T,P>> root;


    void insertPossibleChildStates(std::unordered_map<Node<T,P>*,PossibleState> &current_states, Node<T,P>* root,const P Pi,const unsigned int max_depth,const unsigned int current_depth=0) {
        assert(current_states.count(root)>0); //root must have an entry in current_states already
        if(current_depth < max_depth){
            auto state = current_states[root];
            for(auto &child_pair : root->children){
                Node<T,P> *current_item = child_pair.second.get();
                if(current_states.count(current_item)==0){//if the state is not already under consideration
                    auto new_state = state;
                    new_state.trace_of_current_state.push_back(child_pair.first);
                    new_state.running_prob = Pi* new_state.running_prob;
                    current_states.insert({current_item,new_state});
                }
                else{
                    current_states[current_item].running_prob = current_states[current_item].running_prob + Pi*state.running_prob;
                }
                insertPossibleChildStates(current_states,current_item,Pi,max_depth,current_depth+1);
            }
        }
    }

    void addChildrenOfState(std::unordered_map<Node<T,P>*,PossibleState> &current_states,Node<T,P>*key,const P Pm, const P prob_thresh,const T current_char){
        //Consider the probability that each of the children of the current node is the intended next character. Add each of these states to the stack.
        auto n_children = key->children.size() - key->children.count(current_char);
        auto state = current_states[key];
        for(auto &child_pair : key->children){//for each child of root
                        if(child_pair.first != current_char){//if child is not the current char
                            auto child_elem = child_pair.second.get();
                            if(current_states.count(child_elem)){ //Consider the probability of moving to the child state via the current state
                                current_states[child_elem].running_prob += state.running_prob*(Pm/n_children); //(if the next state is already in the map)
                            }
                            else{
                                PossibleState new_state = state;
                                
                                new_state.running_prob *= (Pm/n_children);//this char was typed wrong
                                new_state.trace_of_current_state.push_back(child_pair.first);
                                if(new_state.running_prob>prob_thresh)//(if it's not _very_ unlikely)
                                    current_states.insert({child_elem,new_state}); //(if the child state isn't already in the map, insert it)
                            }
                        }
                    }
    }

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