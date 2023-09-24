#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STATES 10

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
//     for (int i = 0; i < numDFAStates; i++) {
//         if (memcmp(dfaStates[i]->nfaStateSet.states, nfaSet->states, sizeof(int) * nfaSet->count) == 0) {
//             return i;  // Found the same NFA state set in the DFA
//         }
//     }
//     return -1;  // Not found
// }

// // Add a new DFA state to the queue for processing
// void enqueueDFAState(Queue* queue, DFAState* state) {
//     if (queue->size == queue->capacity) return;  // Queue is full, should not happen in this implementation

//     queue->rear = (queue->rear + 1) % queue->capacity;
//     queue->array[queue->rear] = state;
//     queue->size++;
// }

// // Create a mapping from sets of NFA states to DFA states
// typedef struct {
//     NFAStateSet* nfaSet;
//     int dfaState;
// } NFAToDFA;

// // Function to convert an NFA to a DFA
// DFAState* NFA_to_DFA(NFA* nfa) {
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

//     // Create an array to store DFA states
//     int numDFAStates = 0;
//     int maxDFAStates = MAX_STATES;  // Assuming a maximum number of DFA states
//     DFAState** dfaStates = (DFAState**)malloc(sizeof(DFAState*) * maxDFAStates);

//     // Create an array to store the mapping of NFA states to DFA states
//     NFAToDFA* nfaToDFA = (NFAToDFA*)malloc(sizeof(NFAToDFA) * maxDFAStates);

//     // Map the initial DFA state to its corresponding NFA state set
//     nfaToDFA[initialDFAState->dfaState].nfaSet = &initialDFAState->nfaStateSet;
//     nfaToDFA[initialDFAState->dfaState].dfaState = initialDFAState->dfaState;

//     // Process DFA states to build the DFA
//     while (queueNotEmpty(dfaStatesQueue)) {
//         DFAState* currentDFAState = dequeueDFAState(dfaStatesQueue);

//         // Compute transitions for each symbol and the ε-closure of the resulting NFA states
//         for (char inputSymbol = 'a'; inputSymbol <= 'z'; inputSymbol++) {
//             NFAStateSet nextStateSet;
//             computeTransition(nfa, &currentDFAState->nfaStateSet, inputSymbol, &nextStateSet);

//             // Calculate ε-closure of the resulting NFA state set
//             NFAStateSet* epsilonClosureSet = createNFAStateSet();
//             epsilonClosure(nfa, nextStateSet.states, nextStateSet.count, epsilonClosureSet);

//             // Check if this set of NFA states is already a DFA state
//             int existingDFAState = findDFAState(dfaStates, numDFAStates, epsilonClosureSet);

//             if (existingDFAState == -1) {
//                 // Process a new DFA state and enqueue it for further processing
//                 processDFANewState(dfaStatesQueue, dfaStates, &numDFAStates, epsilonClosureSet, nfaToDFA);
//             }
//             else {
//                 // Add a transition from the current DFA state to the existing DFA state
//                 addDFATransition(currentDFAState, inputSymbol, nfaToDFA[existingDFAState].dfaState);
//             }

//             free(epsilonClosureSet);
//         }
//     }

//     // TODO: Mark accept states in DFA based on your accept state marking logic

//     // Clean up
//     free(epsilonClosureSet);
//     for (int i = 0; i < numDFAStates; i++) {
//         free(dfaStates[i]);
//     }
//     free(dfaStates);
//     free(nfaToDFA);

//     return initialDFAState;
// }

// // Helper function to add a transition from a DFA state to another DFA state
// void addDFATransition(DFAState* dfaState, char inputSymbol, int targetDFAState) {
//     // TODO: Implement this based on your DFA transition function and data structures
// }

// // Helper function to process a new DFA state and enqueue it for further processing
// void processDFANewState(Queue* dfaStatesQueue, DFAState** dfaStates, int* numDFAStates, NFAStateSet* nfaStateSet, NFAToDFA* nfaToDFA) {
//     // TODO: Implement this based on your DFA state processing logic

//     // Create a new DFA state
//     DFAState* newDFAState = (DFAState*)malloc(sizeof(DFAState));
//     newDFAState->nfaStateSet = *nfaStateSet;
//     newDFAState->dfaState = *numDFAStates;

//     // TODO: Mark accept state in the DFA if it corresponds to an accept state in the NFA

//     // Enqueue the new DFA state for further processing
//     enqueueDFAState(dfaStatesQueue, newDFAState);

//     // Update the mapping of NFA states to DFA states
//     nfaToDFA[newDFAState->dfaState].nfaSet = nfaStateSet;
//     nfaToDFA[newDFAState->dfaState].dfaState = newDFAState->dfaState;

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

    // // Create an NFA
    // struct NFA nfaForEndInAt = {0, 0, {2}, 1, transitionForEndInAt};

    // // Convert the NFA to a DFA
    // DFAState* initialDFAState = NFA_to_DFA(&nfaForEndInAt);

    // // Print the DFA state for debugging
    // printf("Initial DFA State: %d\n", initialDFAState->dfaState);

    // // Test the DFA
    // DFA_repl(initialDFAState, "strings ending in \"at\"");

    // // Clean up the dynamically allocated memory for DFA
    // free(initialDFAState);

    return 0;
}
