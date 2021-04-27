#include <iostream>
#include <fstream>
#include <cstring>

#include <vector>

using string = std::string;
using ifstream = std::ifstream;

//Characters are used to represent variables
struct TerminalRule {
    char Symbol;
    char Yield;
};

struct NonTerminalRule {
    char Symbol;
    char Yield[2];
};

//Linked list type structure to represent sets
struct SymbolSet {
    struct Node {
        char symbol;
        Node* next;
    };

    Node* root = NULL;
    //Used for printing purposes
    int count = 0;

    //Add a symbol to the set
    void add(char symbol){
        //Create the root node if neccesary
        if (root == NULL){
            root = new Node{symbol,NULL}; count++;
            return;
        }

        //Append the symbol to the end if it isn't already in the set
        Node* c = root;
        while (c->next != NULL) {
            if (c->symbol == symbol)
                return;
            c = c->next;
        }

        if (c->symbol != symbol){
            c->next = new Node{symbol,NULL}; count++;
        }
    }

    void print(int targetWidth){
        int padding = targetWidth - 2 - (count * 2) + 1;
        if (count==0)
            padding--;

        for (int i=0;i<padding;i++)
                std::cout << ' ';
        std::cout << '{';
    
        Node* c = root;
        while (c != NULL) {

            std::cout << c->symbol;
            if (c->next != NULL)
                std::cout << ',';
            c = c->next;
        }
        std::cout << '}';
    }
};

//Data
/*
TerminalRule rules[4]{
    {'A','a'},
    {'B','b'},
    {'X','a'},
    {'Y','b'}
};
NonTerminalRule ntRules[3] = {
    {'S',{'A','B'}},
    {'A',{'X','A'}},
    {'B',{'Y','B'}}
};

*/
static std::vector<TerminalRule> rules;
static std::vector<NonTerminalRule> ntRules;

int ruleCount = 0;
int ntRuleCount = 0;

inline bool isLower(char c){
    return (c >= 'a' && c <= 'z');
}

void parseLanguage(const char* path){
    ifstream file = ifstream(path);
    char* data = new char[10];

    //Get number of rules
    file.getline(data,10);
    int numRules = atoi(data);
    std::cout << "Parsing " << numRules << " rules.\n";

    //Parse each rule    
    for (int i = 0; i < numRules; i++){
        file.getline(data,10);
        //If the first symbol is lowercase, the symbol is terminal
        if (isLower(data[3])){
            TerminalRule* newRule = new TerminalRule{data[0],data[3]};
            rules.push_back(*newRule);
            ruleCount++;
        } else {
            NonTerminalRule ntRule = NonTerminalRule{};
            ntRule.Symbol = data[0];
            ntRule.Yield[0] = data[3];
            ntRule.Yield[1] = data[4];
            ntRules.push_back(ntRule);
            ntRuleCount++;
        }
    }
}

//Data
/*
TerminalRule rules[4]{
    {'A','a'},
    {'B','b'},
    {'X','a'},
    {'Y','b'}
};

NonTerminalRule ntRules[3] = {
    {'S',{'A','B'}},
    {'A',{'X','A'}},
    {'B',{'Y','B'}}
};

int ntRuleCount = 3;

*/

bool ruleCheck(char* rule,char* product){
    return (rule[0]==product[0] && rule[1] == product[1]);
}

void cartesianCheck(SymbolSet* target, SymbolSet* setA, SymbolSet* setB){
    char* product = new char[2];

    //Cartesian Product
    using Node = SymbolSet::Node;
    
    Node* symbolA = setA->root;
    Node* symbolB = setB->root;

    while (symbolA!=NULL){
        while(symbolB!=NULL){
            product[0] = symbolA->symbol;
            product[1] = symbolB->symbol;

            for (int i=0;i<ntRuleCount;i++){
                NonTerminalRule& rule = ntRules[i];

                if (ruleCheck(rule.Yield,product))
                    target->add(rule.Symbol);
            }

            symbolB = symbolB->next;
        }
        symbolB = setB->root;
        symbolA = symbolA->next;
    }
}

//Processes
int main(int argc,const char** argv){

    parseLanguage(argv[1]);

    const char* word = argv[2];
    int wordLength = strlen(argv[2]);

    //Create the table of sets
    SymbolSet** table = new SymbolSet*[wordLength];
    for (int i=0; i < wordLength; i++)
        table[i] = new SymbolSet[wordLength];


    //Diagonal
    for (int i = 0; i < wordLength; i++){
        for (int r = 0; r < ruleCount; r++){
            TerminalRule& rule = rules[r];

            if (rule.Yield == word[i]){
                table[i][i].add(rule.Symbol);
            }
        }
    }

    //Cartesian Products of filled results
    for (int j = 1; j < wordLength; j++) {
        for (int i = 0 ; i < wordLength - j; i++){
            int targetX = i+j;
            int targetY = i;

            SymbolSet* target = &table[targetX][targetY];

            for (int k=0;k < j; k++){
                SymbolSet* setA = &table[k+i][targetY];
                SymbolSet* setB = &table[targetX][targetY + k+1];

                cartesianCheck(target,setA,setB);
            }

        }
    }


    //Print Table
    for (int i=0;i<wordLength;i++){
        for (int j=0;j<wordLength;j++){
            table[j][i].print(10);
        }
        std::cout << '\n';
    }

    return 0;
}