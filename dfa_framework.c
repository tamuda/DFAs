#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct DFA {
    int startState;
    int currentState;
    int acceptState;
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

bool DFA_run(struct DFA* dfa, const char* input) {
    dfa->currentState = dfa->startState;
    for (int i = 0; i < strlen(input); ++i) {
        dfa->currentState = dfa->transitionFunction(dfa->currentState, input[i]);
    }
    return dfa->currentState == dfa->acceptState;
}

void DFA_repl(struct DFA* dfa, const char* description) {
    char input[128];
    printf("Testing DFA that %s...\n", description);

    while (1) {
        printf("Enter an input (\"quit\" to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove the newline character

        if (strcmp(input, "quit") == 0) break;

        printf("Result for input \"%s\": %s\n", input, DFA_run(dfa, input) ? "true" : "false");
    }
}

int main() {
    struct DFA dfaForCSC = {0, 0, 3, transitionForCSC};
    struct DFA dfaForContainsEnd = {0, 0, 3, transitionForContainsEnd};
    struct DFA dfaForStartsWithVowel = {0, 0, 1, transitionForStartsWithVowel};
    struct DFA dfaForEven01 = {0, 0, 0, transitionForEven01};

    printf("CSC173 Project 1 by Gem and Tamuda \n");

   DFA_repl(&dfaForCSC, "recognizes exactly \"CSC\"");
    DFA_repl(&dfaForContainsEnd, "contains end");
    DFA_repl(&dfaForStartsWithVowel, "recognizes string starting with a vowel");
    DFA_repl(&dfaForEven01, "recognizes even number of 0's and 1's");


    return 0;
}
