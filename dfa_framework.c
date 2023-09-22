#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    int acceptStateCount;  // Number of accept states
    int (*transitionFunction)(int, char);
};

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
        return 2;  // Transition to the final state if 'a' is followed by 't'
    } else if (state == 0 && input != 'a') {
        return 0;  // If 'at' doesn't start at the beginning, stay in the start state
    }
    return -1;  // Invalid transition
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
int transitionForCharacterCounts(int state, char input) {
    if (input == 'a' || input == 'e' || input == 'h' || input == 'i' || input == 'g') {
        return state + 1;
    } else if (input == 'n' || input == 'p') {
        return state + 2;
    }
    return state;
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
            // Invalid transition
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

int main() {
    struct DFA dfaForCSC = {0, 0, 3, transitionForCSC};
    struct DFA dfaForContainsEnd = {0, 0, 3, transitionForContainsEnd};
    struct DFA dfaForStartsWithVowel = {0, 0, 1, transitionForStartsWithVowel};
    struct DFA dfaForEven01 = {0, 0, 0, transitionForEven01};

    struct NFA nfaForEndInAt = {0, 0, {2}, 1, transitionForEndInAt};
    struct NFA nfaForContainsGot = {0, 0, {3}, 1, transitionForContainsGot};
    struct NFA nfaForCharacterCounts = {0, 0, {2, 4}, 2, transitionForCharacterCounts};

    printf("CSC173 Project 1 by Gem and Tamuda \n");

    DFA_repl(&dfaForCSC, "recognizes exactly \"CSC\"");
    DFA_repl(&dfaForContainsEnd, "contains end");
    DFA_repl(&dfaForStartsWithVowel, "recognizes string starting with a vowel");
    DFA_repl(&dfaForEven01, "recognizes even number of 0's and 1's");

    NFA_repl(&nfaForEndInAt, "strings ending in \"at\"");
    NFA_repl(&nfaForContainsGot, "strings contains got");
    NFA_repl(&nfaForCharacterCounts, "Strings that have more than one a, e, h, i, or g, or more than two n’s or p’s");

    return 0;
}
