#pragma once

#include<iostream>
#include<map>
#include <utility>
#include<vector>
#include <array>

using namespace std;

typedef std::pair<int,char> tpair;

/**
 * Abstract class for Deterministic Finite Automata.
 */
class AbstractDFA {
    // TODO: use data structure for representing
    // - states
    // - final states (and sink states)
    // - transitions of the form (state, input) -> state
    // - current state
private:
    // stato corrente
    int current_state;
    // vettore di stati finali
    vector<int> final_states;
    // id dello stato trappola (a parte casi speciali è states.size()+1)
    int trap_id;
    // mappa dell'automa (funzione di transizione (q,a)->q)
    map<tpair, int> states; 

public:
    /**
     * Constructor for Abstract DFA.
     *
     * @param noStates
     *            Number of states in the DFA.
     */
    AbstractDFA(int noStates);
    /**
     * Reset the automaton to the initial state.
     */
    void reset();
    /**
     * Performs one step of the DFA for a given letter. If there is a transition
     * for the given letter, then the automaton proceeds to the successor state.
     * Otherwise it goes to the sink state. By construction it will stay in the
     * sink for every input letter.
     *
     * @param letter
     *            The current input.
     */
    virtual void doStep(char letter);
    /**
     * Check if the automaton is currently accepting.
     *
     * @return True, if the automaton is currently in the accepting state.
     */
    bool isAccepting();
    /**
     * Run the DFA on the input.
     *
     * @param inputWord
     *            stream that contains the input word
     * @return True, if if the word is accepted by this automaton
     */
    bool run(const string &inputWord);
	/**
		aggiungere (o sovrascrivere se già esistente) una transizione alla mappa delle transizioni
		@param from  : stato del DFA da dove parte la transizione
		@param input : carattere che identifica il carattere per effettuare la transizione
		@param to    : stato del DFA dove finisce la transizione
	**/
protected:
	void manageTransition(const int& from, const char& input, int to);
	/**
		imposta l'id della trappola
		@param id  : id della trappola
	**/
	void setTrap(const int& id);
	/**
		@return : id della trappola
	**/
	int getTrap();
	/**
	    Aggiunge id alla lista di stati finali
		@param id : id dello stato finale
	**/
	void addFinalState(const int& id);
	/**
	    Imposta gli stati finali a new_final_states
		@param new_final_states : vector di stati finali
	**/
	void setFinalStates(const vector<int>& new_final_states= vector<int>());
	/**
		@return : vector di stati finali
	**/
	vector<int> getFinalStates();
};

/**
 * DFA recognizing a given word.
 */
class WordDFA : public AbstractDFA {

public:
    /**
     * Construct a new DFA that recognizes exactly the given word. Given a word
     * "foo" the constructed automaton looks like: -> () -f-> () -o-> () -o-> []
     * from every state (including the final one) every other input letter leads
     * to a distinguished sink state in which the automaton then remains
     *
     * @param word
     *            A String that the automaton should recognize
     */
    WordDFA(const string &word);
};

/**
 * DFA recognizing comments.
 */
class CommentDFA : public AbstractDFA {
protected:
    inline static const char ANY = 'c';
    inline static const vector<char> ALPHABET = {
            '/',
            '\n',
            '*',
            CommentDFA::ANY // rappresenta qualunque altro carattere
    };
public:
    /**
     * Construct a new DFA that recognizes comments within source code. There
     * are two kinds of comments: A single line comment starts with // and ends
     * with a newline and a multiline comment that starts with / * and ends with
     * * / (without the spaces)
     */
    CommentDFA();

    /**
     * Performs one step of the DFA for a given letter. This method works
     * differently than in the superclass AbstractDFA.
     *
     * @param letter
     *            The current input.
     */
    virtual void doStep(char letter);
};


