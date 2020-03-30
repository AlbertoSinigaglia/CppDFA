#include <iostream>
#include "automata.h"

using namespace std;

/**
 * Constructor for Abstract DFA.
 *
 * @param noStates
 *            Number of states in the DFA.
 */
AbstractDFA::AbstractDFA(int noStates) : current_state(0), trap_id(-1){}

/**
 * Reset the automaton to the initial state.
 */
void AbstractDFA::reset() {
    // reimposto lo stato corrente a 0
    current_state = 0;
}

/**
 * Performs one step of the DFA for a given letter. If there is a transition
 * for the given letter, then the automaton proceeds to the successor state.
 * Otherwise it goes to the sink state. By construction it will stay in the
 * sink for every input letter.
 *
 * @param letter
 *            The current input.
 */
void AbstractDFA::doStep(char letter) {
    /*
        vado a cercare nella mappa il percorso definito
        -> essendo DFA non mi preoccupo del caso "percorso non trovato" per definizione di DFA
    */
    current_state = states[tpair(current_state, letter)];
}

/**
 * Check if the automaton is currently accepting.
 *
 * @return True, if the automaton is currently in the accepting state.
 */
bool AbstractDFA::isAccepting() {
    /*
        controllo se lo stato in cui mi trovo è dentro all'insieme
        degli stati che son segnati come finali
    */ 
    return std::count(final_states.begin(), final_states.end(), current_state);
}

/**
 * Run the DFA on the input.
 *
 * @param inputWord
 *            stream that contains the input word
 * @return True, if if the word is accepted by this automaton
 */
bool AbstractDFA::run(const string &inputWord) {
    this->reset();
    for(int i = 0; i < inputWord.length(); i++) {
        doStep(inputWord[i]);
    }
    return isAccepting();
}

//aggiunge o modifica la transizione da  (FROM, INPUT) a TO
void AbstractDFA::manageTransition(const int& from, const char& input, int to){ states[tpair(from, input)] = to; }

//getter e setter vari
void AbstractDFA::setTrap(const int& id){ trap_id = id; }
int AbstractDFA::getTrap(){ return trap_id; }
void AbstractDFA::addFinalState(const int& id){ final_states.push_back(id); }
void AbstractDFA::setFinalStates(const vector<int>& new_final_states){ final_states = new_final_states; }
vector<int> AbstractDFA::getFinalStates(){ return final_states; }








/**
 * Construct a new DFA that recognizes exactly the given word. Given a word
 * "foo" the constructed automaton looks like: -> () -f-> () -o-> () -o-> []
 * from every state (including the final one) every other input letter leads
 * to a distinguished sink state in which the automaton then remains
 *
 * @param word
 *            A String that the automaton should recognize
 */
/*

|\
| \       
|  (0) -- 1st letter --> (1) -- 2nd letter --> (2)  -- 3rd letter --> (3) -- .... --> (n-1) -- nth letter --> ((n))
| /
|/

*/
WordDFA::WordDFA(const string &word) : AbstractDFA(0) {
    assert (word.length() > 0);
    // imposto come stato trappola l'n-esimo + 1 dove n = numero di stati dell'automa
    AbstractDFA::setTrap(word.length()+1);
    // preparo la mappa del DFA passando per ogni lettera dell'alfabeto (dove alfabeto = set(chars(word)))
    for(auto& l : word) 
        // per ogni stato che poi il DFA avrà == length(word)+stato finale e lo stato trappola ( +1 dopo il .length())
        for(unsigned int i = 0; i<= word.length() + 1; ++i)
            // inizializzo per questa coppia (*lettera l dell'alfabeto* + *stato i del DFA*) il valore allo stato trap
            // (poi i percorsi che mi interesseranno li andrò a sovrascrivere)
            AbstractDFA::manageTransition(i, l, AbstractDFA::getTrap());
    // per ogni lettera della mia parola (e quindi stato del mio DFA eccetto l'ultimo) sovrascrivo il valore precedente
    // (che era quello che portava a trap) e metto come valore l'identificativo del successivo stato (dato stato i e word[i], 
    // il successivo è i+1)
    for(unsigned int i = 0; i < word.length(); ++i)
        AbstractDFA::manageTransition(i, word[i], i+1);
    //come stato finale metto solo l'n-esimo (ultima lettera della mia parola)
    AbstractDFA::addFinalState(static_cast<int>(word.length()));
}

/**
 * Construct a new DFA that recognizes comments within source code. There
 * are two kinds of comments: A single line comment starts with // and ends
 * with a newline and a multiline comment that starts with / * and ends with
 * * / (without the spaces)
 */

/* 
                        "ANY, /, *"        
                           /   \            
                           \   /            
                       -->  (2)  -- "\n" --> ((6))
                     /                                      
|\                 "/"                                                
| \                /                                                
|  (0) -- "/" --> (1)                                            
| /                \   "ANY, /, \n"        "*"              
|/                 "*"    /   \           /   \         
                     \    \   /           \   /      
                       --> (3) -- "*" -->  (4) -- "/" --> ((5))
                            ^               /
                             \             /
                                "ANY, \n"
*/
CommentDFA::CommentDFA() : AbstractDFA(0) {
    // stati finali del DFA
    AbstractDFA::setFinalStates({5, 6});
    AbstractDFA::setTrap(7);
    // per ogni stato e per ogni lettera dell'alfabeto, creo una transizione a trap_id
    // di cui poi andrò a sovrascrivere solo quelle che mi servono
    for(auto& i : {0,1,2,3,4,5,6, AbstractDFA::getTrap()})
        for(auto& c : CommentDFA::ALPHABET)
            AbstractDFA::manageTransition(i, c, AbstractDFA::getTrap());

    //tutto fino alla biforcazione
    AbstractDFA::manageTransition(0, '/', 1);
    AbstractDFA::manageTransition(1, '/', 2);
    AbstractDFA::manageTransition(1, '*', 3);
    
    // ramo superiore
    AbstractDFA::manageTransition(2, '*' , 2);
    AbstractDFA::manageTransition(2, '/' , 2);
    AbstractDFA::manageTransition(2, CommentDFA::ANY , 2);
    AbstractDFA::manageTransition(2, '\n', 6);

    // ramo inferiore
    AbstractDFA::manageTransition(3, '/'  , 3);
    AbstractDFA::manageTransition(3, '\n' , 3);
    AbstractDFA::manageTransition(3, CommentDFA::ANY  , 3);
    AbstractDFA::manageTransition(3, '*'  , 4);
    AbstractDFA::manageTransition(4, CommentDFA::ANY  , 3);
    AbstractDFA::manageTransition(4, '\n' , 3);
    AbstractDFA::manageTransition(4, '*'  , 4);
    AbstractDFA::manageTransition(4, '/'  , 5);
}

/**
 * Performs one step of the DFA for a given letter. This method works
 * differently than in the superclass AbstractDFA.
 *
 * @param letter
 *            The current input.
 */
void CommentDFA::doStep(char letter) {
    // chiamo il doStep di default
    AbstractDFA::doStep(
            // se trovo il carattere nel mio alfabeto, uso quel carattere, 
            // altrimenti uso ANY per rappresentarlo
            std::count(CommentDFA::ALPHABET.begin(), CommentDFA::ALPHABET.end(), letter) ?
                letter : CommentDFA::ANY
    );
}


