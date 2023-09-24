#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//#define MAX_STATES 10

struct DFA {
    int startState;
    int currentState;
    int acceptState;
    int (*transitionFunction)(int, char);
};

struct NFA {
    int startState;
    int currentState;
    int acceptStates[10];  // Array to hold multiple accept states
    int acceptStateCount;  
    int (*transitionFunction)(int, char);
};

int characterCounts[7] = {0};  
char characters[7] = {'a', 'e', 'h', 'i', 'g', 'n', 'p'};

void countCharacters(char input) {
    for (int i = 0; i < 7; i++) {
        if (input == characters[i]) {
            characterCounts[i]++;
            break;  
        }
    }
}

// Transition function for the string "CSC"
int transitionForCSC(int state, char input) {
    if (state == 0 && input == 'C') return 1;
    if (state == 1 && input == 'S') return 2;
    if (state == 2 && input == 'C') return 3;
    return 0;
}

// Transition function for the string containing "end"
int transitionForContainsEnd(int state, char input) {
    if (state == 0 && input == 'e') return 1;
    if (state == 1 && input == 'n') return 2;
    if (state == 2 && input == 'd') return 3;
    return state;
}

// Transition function for string starting with a vowel
int transitionForStartsWithVowel(int state, char input) {
    if (state == 0 && (input == 'a' || input == 'e' || input == 'i' || input == 'o' || input == 'u')) {
        return 1;
    }
    if(state==0){
        return 2; //Move to a non-accept state if first char is not a vowel
    }
    return state; //stay in same state for remaining states

}

// Transition function for even 0's and 1's
int transitionForEven01(int state, char input) {
    if (input == '0') return state ^ 1;
    if (input == '1') return state ^ 2;
    return state;
}

// Transition function for the NFA recognizing strings that end in "at"
int transitionForEndInAt(int state, char input) {
    if (state == 0 && input == 'a') {
        return 1;
    } else if (state == 1 && input == 't') {
        return 2;  
    } else if (state == 0 && input != 'a') {
        return 0;  // If 'at' doesn't start at the beginning, stay in the start state
    }
    return -1; 
}

// Transition function for the NFA recognizing strings containing "got"
int transitionForContainsGot(int state, char input) {
    if (state == 0 && input == 'g') {
        return 1;
    } else if (state == 1 && input == 'o') {
        return 2;
    } else if (state == 2 && input == 't') {
        return 3;
    }
    return state;  // Stay in the same state if no valid transition
}

// Transition function for the NFA recognizing strings that have more than one a, e, h, i, or g, or more than two n’s or p’s
bool transitionForCharacterCounts(const char* input) {
    // Reset character counts for each new input string
    for (int i = 0; i < 7; i++) {
        characterCounts[i] = 0;
    }

    // Count the occurrences of each character in the input string
    for (int i = 0; i < strlen(input); i++) {
        countCharacters(input[i]);
    }

    // Check conditions for 'a', 'e', 'h', 'i', 'g', 'n', and 'p'
    for (int i = 0; i < 5; i++) {
        if (characterCounts[i] > 1) {
            return true;  
        }
    }

    // Check conditions for 'n' and 'p'
    for (int i = 5; i < 7; i++) {
        if (characterCounts[i] > 2) {
            return true;  
        }
    }

    return false; 
}


bool DFA_run(struct DFA* dfa, const char* input) {
    dfa->currentState = dfa->startState;
    for (int i = 0; i < strlen(input); ++i) {
        dfa->currentState = dfa->transitionFunction(dfa->currentState, input[i]);
    }
    return dfa->currentState == dfa->acceptState;
}

bool NFA_run(struct NFA* nfa, const char* input) {
    int i, j;
    nfa->currentState = nfa->startState;
    
    for (i = 0; i < strlen(input); ++i) {
        int nextState = nfa->transitionFunction(nfa->currentState, input[i]);
        
        if (nextState == -1) {
            return false;
        }
        
        nfa->currentState = nextState;
    }
    
    // Check if the final state is an accept state
    for (j = 0; j < nfa->acceptStateCount; ++j) {
        if (nfa->currentState == nfa->acceptStates[j]) {
            return true;
        }
    }
    
    return false;
}

void DFA_repl(struct DFA* dfa, const char* description) {
    char input[128];
    printf("Testing DFA that %s...\n", description);

    while (1) {
        printf("Enter an input (\"quit\" to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; 

        if (strcmp(input, "quit") == 0) break;

        printf("Result for input \"%s\": %s\n", input, DFA_run(dfa, input) ? "true" : "false");
    }
}

void NFA_repl(struct NFA* nfa, const char* description) {
    char input[128];
    printf("Testing NFA that %s...\n", description);

    while (1) {
        printf("Enter an input (\"quit\" to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; 

        if (strcmp(input, "quit") == 0) break;

        // Reset the NFA's current state to the start state before processing each input
        nfa->currentState = nfa->startState;

        printf("Result for input \"%s\": %s\n", input, NFA_run(nfa, input) ? "true" : "false");
    }
}

void NFA_repl_characterCounts() {
    char input[128];

    printf("Testing NFA that with strings that have more than one a, e, h, i, or g, or more than two n’s or p’s...\n");

    while (1) {
        printf("Enter an input (\"quit\" to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "quit") == 0) {
            break;
        }

        printf("Result for input \"%s\": %s\n", input, transitionForCharacterCounts(input) ? "true" : "false");
    }
}

//
//THIS IS FOR NFA TO DFA SECTION
//

// // Structure to represent a set of NFA states
// typedef struct {
//     int states[MAX_STATES];
//     int count;
// } NFAStateSet;

// // Structure to represent a DFA state
// typedef struct {
//     NFAStateSet nfaStateSet;  // Set of NFA states represented by this DFA state
//     int dfaState;             // Corresponding DFA state number
// } DFAState;

// // Queue for processing DFA states during conversion
// typedef struct {
//     int front, rear, size;
//     unsigned capacity;
//     DFAState** array;
// } Queue;

// // Create a new NFA state set
// NFAStateSet* createNFAStateSet() {
//     NFAStateSet* set = (NFAStateSet*)malloc(sizeof(NFAStateSet));
//     set->count = 0;
//     return set;
// }

// // Helper function to check if a state set is already in the DFA
// int findDFAState(DFAState** dfaStates, int numDFAStates, NFAStateSet* nfaSet) {
//     // TODO: Implement this function to find a matching DFA state, return -1 if not found
//     // You'll compare the NFAStateSet of each DFAState with nfaSet
// }

// // Add a new DFA state to the queue for processing
// void enqueueDFAState(Queue* queue, DFAState* state) {
//     // TODO: Implement this function to enqueue a DFA state for processing
// }

// // Create a mapping from sets of NFA states to DFA states
// Map* nfaToDfaMap = createMap();
// addToMap(nfaToDfaMap, epsilonClosureSet, initialDFAState);

// while (!isEmpty(dfaStatesQueue)) {
//     DFAState* currentDFAState = dequeueDFAState(dfaStatesQueue);

//     // Process each input symbol
//     for (char inputSymbol = 'a'; inputSymbol <= 'z'; inputSymbol++) {
//         // Compute transitions based on the input symbol

//         // TODO: Implement transition logic for the current DFA state and input symbol

//         // Calculate ε-closure of the resulting NFA state set

//         // TODO: Implement epsilon closure calculation
//     }
// }

// // Inside the inner loop for processing input symbols
// NFAStateSet nextStateSet;
// computeTransition(nfa, &currentDFAState->nfaStateSet, inputSymbol, &nextStateSet);

// // Calculate ε-closure of the resulting NFA state set
// NFAStateSet* epsilonClosureSet = createNFAStateSet();
// epsilonClosure(nfa, nextStateSet.states, nextStateSet.count, epsilonClosureSet);

// // Check if this set of NFA states is already a DFA state
// int existingDFAState = findDFAState(dfaStates, numDFAStates, epsilonClosureSet);

// // TODO: Implement adding transitions to the DFA based on existing or new DFA state

// void computeTransition(NFA* nfa, NFAStateSet* nfaStateSet, char inputSymbol, NFAStateSet* nextStateSet) {
//     // TODO: Implement transition calculation based on input symbol and current NFA state set
// }

// void processDFANewState(Queue* dfaStatesQueue, DFAState** dfaStates, int* numDFAStates, NFAStateSet* nfaStateSet) {
//     // TODO: Implement processing of a new DFA state and enqueueing it for further processing
// }

// // Mark accept states in DFA
// // TODO: Modify this based on your accept state marking logic

// // Function to convert an NFA to a DFA
// DFA* NFA_to_DFA(NFA* nfa) {
//     // Initialize the DFA and other necessary data structures

//     // Step 1: Compute ε-closure of the start state of NFA
//     NFAStateSet* epsilonClosureSet = createNFAStateSet();
//     epsilonClosure(nfa, &nfa->startState, 1, epsilonClosureSet);

//     // Initialize a queue for processing DFA states
//     Queue* dfaStatesQueue = createQueue();

//     // Create the initial DFA state
//     DFAState* initialDFAState = (DFAState*)malloc(sizeof(DFAState));
//     initialDFAState->nfaStateSet = *epsilonClosureSet;
//     initialDFAState->dfaState = 0;
//     enqueueDFAState(dfaStatesQueue, initialDFAState);

//     // Create a mapping from sets of NFA states to DFA states
//     // This will help in avoiding duplicates and mapping transitions
//     Map* nfaToDfaMap = createMap();
//     addToMap(nfaToDfaMap, epsilonClosureSet, initialDFAState);

//     // Process DFA states to build the DFA
//     while (!isEmpty(dfaStatesQueue)) {
//         DFAState* currentDFAState = dequeueDFAState(dfaStatesQueue);
        
//         // TODO: Implement transitions and processing of NFA states to build the DFA
//         // Compute transitions for each symbol and the ε-closure of the resulting NFA states
//     }

//     // Mark accept states in DFA
//     // TODO: Modify this based on your accept state marking logic

//     return dfa;
// }

//
//
//

int main() {
    struct DFA dfaForCSC = {0, 0, 3, transitionForCSC};
    struct DFA dfaForContainsEnd = {0, 0, 3, transitionForContainsEnd};
    struct DFA dfaForStartsWithVowel = {0, 0, 1, transitionForStartsWithVowel};
    struct DFA dfaForEven01 = {0, 0, 0, transitionForEven01};

    struct NFA nfaForEndInAt = {0, 0, {2}, 1, transitionForEndInAt};
    struct NFA nfaForContainsGot = {0, 0, {3}, 1, transitionForContainsGot};

    printf("CSC173 Project 1 by Gem and Tamuda \n");

    DFA_repl(&dfaForCSC, "recognizes exactly \"CSC\"");
    DFA_repl(&dfaForContainsEnd, "contains end");
    DFA_repl(&dfaForStartsWithVowel, "recognizes string starting with a vowel");
    DFA_repl(&dfaForEven01, "recognizes even number of 0's and 1's");

    NFA_repl(&nfaForEndInAt, "strings ending in \"at\"");
    NFA_repl(&nfaForContainsGot, "strings contains got");
    NFA_repl_characterCounts();


    return 0;
}
