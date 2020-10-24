#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>
#include<vector>
#include<memory>
#include<math.h>

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
        P end_prob;
        Leaf(const std::vector<T> value) : value {value} {}; 
        Leaf(const std::vector<T> value, const P end_prob) : value {value}, end_prob {end_prob} {}; 
        
        Leaf operator=(const Leaf &leaf){//Copy constructor.
            return Leaf(leaf.value,leaf.end_prob);
        };
    };


    struct PossibleState {
        Node<T,P> *current;
        P running_prob; 
        vector<T> trace_of_current_state;
        PossibleState(Node<T,P> *current,const P prob,const vector<T> trace) : current {current}, running_prob {prob}, trace_of_current_state {trace}{};
        PossibleState( Node<T,P> *current) : current {current} {}; 
        PossibleState operator=(const PossibleState &state){
            return PossibleState(state.current,state.running_prob,state.trace_of_current_state);
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


    //partial: word to try matching
    //Pc: probability of typing char correctly
    //Pi: probability of inserting an unneeded char
    //Pm: probability of typing a char wrong in place.
    std::vector<PossibleState> findPossibleChildren(const std::vector<T> partial,const P Pc, const P Pi, const P Pm, const P prob_thresh){   
        std::vector<PossibleState> current_states;
        current_states.push_back(this->root.get());
        for(T c : partial){//For each char in input
            std::cout<<current_states.size()<<std::endl;
            std::unordered_map<Node<T,P>*,unsigned int> counts;
            const int current_state_size = current_states.size();
            for(int i=0;i<current_state_size;i++){//for each of the states currently under consideration...
                const PossibleState state = current_states[i];

                if(counts.count(state.current)){ //next few lines are in preparation for proper state combination
                    counts[state.current]=counts[state.current]+1;
                }
                else{
                    counts.insert({state.current,1});
                }


                if(state.running_prob < prob_thresh)//if the probability of the state is below some threshold, forget it...we should also pop it 
                    continue;


                //beginning of update logic (currently flawed)               
                if(state.current->children.size()==0){//if no child state, must be an insertion error
                    current_states[i].running_prob += std::log(Pi);
                }
                else if(state.current->children.count(c) == 0){//if none of the children match
                auto n_children = state.current->children.size();
                    for(auto &child_pair : state.current->children){
                        auto new_state = state;
                        new_state.current = child_pair.second.get();
                        new_state.running_prob += std::log(Pm/n_children);//we know that the current char can't be correct, so it must be incorrect or a false insertion
                        new_state.trace_of_current_state.push_back(child_pair.first);
                        if(new_state.running_prob>prob_thresh)
                            current_states.push_back(new_state);
                    }
                    current_states[i].running_prob += std::log(Pi);
                }
                else {//case with matching child
                    auto n_other_children = state.current->children.size() + (-1 ? state.current->children.size()>1 : 0);
                    for(auto &child_pair : state.current->children){
                        if(child_pair.first != c){
                            PossibleState new_state = state;
                            new_state.current = child_pair.second.get();
                            new_state.running_prob += std::log(Pm/n_other_children);//this char was typed wrong
                            new_state.trace_of_current_state.push_back(child_pair.first);
                            if(new_state.running_prob>prob_thresh)
                                current_states.push_back(new_state);
                        }
                    }
                    auto new_state = state;
                    new_state.current = (state.current->children[c]).get();
                    new_state.running_prob += std::log(Pc);//Correct char
                    new_state.trace_of_current_state.push_back(c);
                    if(new_state.running_prob>prob_thresh)
                        current_states.push_back(new_state);
                    current_states[i].running_prob += std::log(Pi); //this char shouldn't have been typed at all
                }
            }

            std::cout<<counts.size()<<"========="<<std::endl;//counts.size() gives the *actual* number of states...<<current_states.size()
        }
        return current_states;
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