/*
 *      GRAMMAR
 *      proposition = p | q | r
 *      BC = v | ^ | >
 *      formula = proposition | -formula | (formula BC formula)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int Fsize = 50; /*maximum formula length*/
int inputs = 10;/* number of formulas expected in input.txt*/
int ThSize = 100;/* maximum size of set of formulas, if needed*/
int TabSize = 500; /*maximum length of tableau queue, if needed*/


/*----- FUNCTION DEFINITIONS -----*/

int parse(char *g);

char *tail(char *g);

char *partone(char *g);

char *parttwo(char *g);

int BC(char connective);

char *partition(char *g, int start, int end);

int proposition(char c);

char get_BC(char *g);


/*----- PARSER FUNCTIONS -----*/

/*
* Function: decide whether a character is a member of proposition character set in the grammar
* Returns: integer, 1 if true, 0 if false
*/
int proposition(char c) {
    if (c == 'p' || c == 'q' || c == 'r' || c == 's') return 1;
    return 0;
}

/*
* Function: decide whether a character is a member of binary connective character set in the grammar
* Returns: integer, 1 if true, 0 if false
*/
int BC(char connective) {
    if (connective == 'v' || connective == '^' || connective == '>') return 1;
    return 0;
}

/*
* Function: cuts the first character, leaving the tail of a particular character array (string)
* Returns: pointer to the first element of the new tail character array
*/
char *tail(char *g) {
    int i = 0, g_length = strlen(g);
    char *new_sting = (char *) malloc(g_length * sizeof(char));

    while (g[i + 1] != '\0') {
        new_sting[i] = g[i + 1];
        i++;
    }

    new_sting[i] = '\0';
    return new_sting;
}

/*
* Function: counts the number of brackets and determines the highest level binary connective in a given formula
* Returns: a single character
*/
char get_BC(char *g) {
    int bracket_count = 0, i = 0;
    while (1) {
        if (g[i] == '\0') return '0';
        else if (g[i] == '(') bracket_count++;
        else if (g[i] == ')') bracket_count--;
        else if (BC(g[i]) == 1 && bracket_count == 1) return g[i];
        i++;
    }
}

/*
* Function: universal function to divide a given string g between interval index "start" to the position "end"
* Returns: pointer to the first element of char array of the newly partitioned string
*/
char *partition(char *g, int start, int end) {
    int length_of_seg = end - start;
    char *partitioned = (char *) malloc(((sizeof(g) / 2) + 1) * sizeof(char));

    int i;
    for (i = 0; i < length_of_seg; i++) {
        partitioned[i] = g[i + start];
    }

    partitioned[length_of_seg] = '\0';
    return partitioned;
}

/*
* Function: determines highest level binary connective, to return the first part of that formula
* Returns: pointer to the first element of char array of the first part of the formula
*/
char *partone(char *g) {
    int bracket_count = 0, i = 0;
    while (1) {
        if (g[i] == '(') bracket_count++;
        else if (g[i] == ')') bracket_count--;
        else if (BC(g[i]) == 1 && bracket_count == 1) return partition(g, 1, i);
        i++;
    }
}

/*
* Function: determines highest level binary connective, to return the second part of that formula
* Returns: pointer to the first element of char array of the second part of the formula
*/
char *parttwo(char *g) {
    int bracket_count = 0, i = 0;
    while (1) {
        if (g[i] == '(') bracket_count++;
        else if (g[i] == ')') bracket_count--;
        else if (BC(g[i]) == 1 && bracket_count == 1) return partition(g, i + 1, strlen(g) - 1);
        i++;
    }
}

/*
* Function: determines whether a formula is a valid one, and what type of formula it is
* Returns: 1 if a proposition, 2 if negation, 3 if binary, 0 otherwise (not a formula)
*/
int parse(char *g) {
    /* Proposition (p | q | r) */
    if (proposition(g[0]) && g[1] == '\0') return 1;

    /* Negation (-) */
    if (g[0] == '-' && parse(tail(g)) != 0) return 2;

    /* Binary (formula BC formula) */
    if (g[0] == '(' && g[strlen(g) - 1] == ')' && get_BC(g) != '0') {
        if (parse(partone(g)) != 0 && parse(parttwo(g)) != 0) return 3;
    }

    /* Not a formula */
    return 0;
}

/*----- TABLEAU FUNCTIONS -----*/

/* A set will contain a list of words. Use NULL for emptyset.  */
struct set {
    char *item;/*first word of non-empty set*/
    struct set *tail;/*remaining words in the set*/
};

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
    struct set *S; /* pointer to first set in non-empty list */
    struct tableau *rest; /*list of pointers to other sets*/
};

/*
* Function: given a number of negatives, removes that number of negation symbols from a string
* Returns: pointer to the first element of char array of the new string
*/
char *removeNegative(char *t, int num_of_negs) {
    char *g = (char *) malloc(((sizeof(t) - 1) * sizeof(char)));
    int i = 0, k;
    
    for (k = num_of_negs; k < strlen(t); k++) {
        g[i] = t[k];
        i++;
    }
    g[i] = '\0';
    t = g;
    return t;
}

/*
* Function: decides whether or not a string is a terminal element or not (proposition, or a negated proposition)
* Returns: integer, 1 if true, 0 if false
*/
int terminalElement(char *g) {
    if (g == NULL) return 0;
    if (proposition(g[0]) || (g[0] == '-' && proposition(g[1]))) return 1;
    else return 0;
}

/*
* Function: given a list of strings, concatinates them into one string
* Returns: pointer to the first element of char array of the new string
*/
char *concatStrings(char **lst, int size) {
    char *g = (char *) malloc(sizeof(char) * Fsize);
    int i;

    for (i = 0; i < size; i++) {
        strcat(g, lst[i]);
    }
    return g;
}

/*
* Function: processes formulas that are a negation of a conjunction, disjunction or an implication
* Returns: pointer to the first element of the new char array
*/
char *negationProcess(char *g) {
    char *resulting_formula = (char *) malloc(sizeof(char) * Fsize);
    
    // Remove a single negation symbol from string g
    g = removeNegative(g, 1);

    // Negation of a conjunction == disjunction of negated elements (alpha formula)
    if (get_BC(g) == '^') {
        char *concat_strings[6] = {"(-", partone(g), "v", "-", parttwo(g), ")"};
        resulting_formula = concatStrings(concat_strings, 6);
    }

    // Negation of a disjunction == conjunction of negated elements (beta formula)
    if (get_BC(g) == 'v') {
        char *concat_strings[6] = {"(-", partone(g), "^", "-", parttwo(g), ")"};
        resulting_formula = concatStrings(concat_strings, 6);
    }

    // Negation of an implication == conjunction of the elements, second element negated (alpha formula)
    if (get_BC(g) == '>') {
        char *concat_strings[6] = {"(", partone(g), "^", "-", parttwo(g), ")"};
        resulting_formula = concatStrings(concat_strings, 6);
    }
    return resulting_formula;
}

/*
* Function: complete tableau for a case of negated formula type
* Returns: pointer to the first element of the new formula completed after alpha or beta expansion
*/
char *completeNegation(char *g) {
    // Remove 2n negations if more than 1 negation exists
    if (g[0] == '-' && g[1] == '-') {
        int signCounter = 0;
        while (g[signCounter] == '-') {
            signCounter++; // count number of negation signs
        }
        // if even number of negation signs, simplify
        if (signCounter % 2 == 0) {
            g = removeNegative(g, signCounter);
        } else {
            g = removeNegative(g, 2);
        }
    }

    // If negation and not a negated proposition
    if (g[0] == '-' && strlen(g) > 2) {
        while (g[0] == '-' && strlen(g) > 2) {
            return negationProcess(g);
        }
    }
    return g;
}

/*
* Function: complete tableau for a case of a conjunction formula type
* Returns: pointer to the first element of the new completed formula
*/
void completeAnd(struct set *set) {
    struct set *result = (struct set *) malloc(sizeof(struct set));

    result->item = parttwo(set->item);
    result->tail = set->tail;

    set->tail = result;
    set->item = partone(set->item);
}

/*
* Function: complete tableau for a case of a disjunction formula type
* Returns: nothing, works on the set structure pointers "set", "currHead" and a tableau structure pointer "tableau"
*/
void completeOr(struct set *set, struct set *currHead, struct tableau *tableau) {
    struct tableau *newTableau = (struct tableau *) malloc(sizeof(struct tableau));
    struct set *newSet = (struct set *) malloc(sizeof(struct set));
    struct set *currentSet = currHead;
    
    newTableau->S = newSet;
    while (strcmp(currentSet->item, set->item) != 0) {
        newSet->item = currentSet->item;
        newSet->tail = (struct set *) malloc(sizeof(struct set));
        newSet = newSet->tail;
        currentSet = currentSet->tail;
    }

    newSet->item = parttwo(set->item);
    currentSet = currentSet->tail;

    if (currentSet) {
        newSet->tail = (struct set *) malloc(sizeof(struct set));
        newSet = newSet->tail;
        while (currentSet) {
            newSet->item = currentSet->item;
            if (currentSet->tail) newSet = (struct set *) malloc(sizeof(struct set));
            else newSet->tail = NULL;
            currentSet = currentSet->tail;
        }
    } else {
        newSet->tail = NULL;
    }
    
    set->item = partone(set->item);
    newTableau->rest = tableau->rest;
    tableau->rest = newTableau;
}

/*
* Function: complete tableau for a case of an implication formula type
* Returns: nothing, works on the set structure pointers "set", "currHead" and a tableau structure pointer "tableau"
*/
void completeImplication(struct set *set, struct set *currHead, struct tableau *tableau) {
    struct tableau *newTableau = (struct tableau *) malloc(sizeof(struct tableau));
    struct set *newSet = (struct set *) malloc(sizeof(struct set));
    struct set *currentSet = currHead;
    char *firstPart = (char *) malloc(sizeof(char) * Fsize);
    
    newTableau->S = newSet;  
    while (strcmp(currentSet->item, set->item) != 0) {
        newSet->item = currentSet->item;
        newSet->tail = (struct set *) malloc(sizeof(struct set));
        newSet = newSet->tail;
        currentSet = currentSet->tail;
    }
    newSet->item = parttwo(set->item);
    currentSet = currentSet->tail;
    
    if (currentSet) {
        newSet->tail = (struct set *) malloc(sizeof(struct set));
        newSet = newSet->tail;
        while (currentSet) {
            newSet->item = currentSet->item;
            if (currentSet->tail) newSet = (struct set *) malloc(sizeof(struct set));
            else newSet->tail = NULL;
            currentSet = currentSet->tail;
        }
    } else {
        newSet->tail = NULL;
    }

    strcat(firstPart, "-");
    strcat(firstPart, partone(set->item));
    set->item = firstPart;
    newTableau->rest = tableau->rest;
    tableau->rest = newTableau;
}

/*
* Function: populates the "seen" list with characters passed as parameter in the correct adjecent order
* Returns: nothing, edits the pointer to the original integer list
*/
void closed_list(int *seen, char *g) {
    if (strcmp(g, "p") == 0) seen[0] = 1;
    else if (strcmp(g, "-p") == 0) seen[1] = 1;
    else if (strcmp(g, "q") == 0) seen[2] = 1;
    else if (strcmp(g, "-q") == 0) seen[3] = 1;
    else if (strcmp(g, "r") == 0) seen[4] = 1;
    else seen[5] = 1;
}

/*
* Function: checks if two adjacent elements are marked 1 in the array
* Returns: if mark 1 is present in two adjacent elements - a contradiction exists in the tableau
*/
int check_closure(const int *seen) {
    if ((seen[0] && seen[1]) == 1 || (seen[2] && seen[3]) == 1 || (seen[4] && seen[5]) == 1) return 1;
    else return 0;
}

/*
* Function: re-initialise the "seen" list with 0's
* Returns: nothing, as it edits the given pointer to the integer list "lst"
*/
void empty_list(int *lst) {
    int i;
    for (i = 0; i < 6; i++) lst[i] = 0;
}

/*
* Function: iterates through the tableau and checks for contradictions using check_closure() function
* Returns: integer, 0 if the formula is not satisfiable, 1 if the formula is satisfiable
*/
int closed(struct tableau *t) {
    struct set *set;
    int seen[6], result = 0;

    // iterate through the tableau structure
    while (t) {
        set = t->S;
        empty_list(seen); // reset the "seen" list for this branch
        // iterate through the branch
        while (set) {
            closed_list(seen, set->item); // populate the "seen" integer array
            set = set->tail;
        }
        result = check_closure(seen);
        if (!result) return 0;
        t = t->rest;
    }
    return 1;
}

/*
* Function: iterate through the tableau, populate the structures with appropriate expansion values
* Returns: nothing, works on pointer "t" to the tableau structure
*/
void complete(struct tableau *t) {
    struct tableau *tableau = t;
    struct set *set;

    while (tableau) {
        set = tableau->S;
        struct set *currHead = tableau->S;

        while (set) {
            int isTerminal = terminalElement(set->item);
            
            // If the formula is not a proposition
            if (!isTerminal) {

                // If Negation
                if (parse(set->item) == 2) {
                    set->item = completeNegation(set->item);
                }
                if (isTerminal) {
                    set = set->tail;
                    continue;
                }

                char operator = get_BC(set->item);
                if (operator == '^') {
                    completeAnd(set); // highest level is a conjunction
                } else if (operator == '>') {
                    completeImplication(set, currHead, tableau); // highest level is an implication
                } else if (operator == 'v') {
                    completeOr(set, currHead, tableau); // highest level is a disjunction
                }
            } else { // If it is a proposition
                set = set->tail;
            }

        }
        tableau = tableau->rest;
    }
}

/*
* Function: main function, execution starts here
* Returns: integer, its value is insignificant for purposes of the parser & prover
*/
int main() {
    /*You should not need to alter the input and output parts of the program below.*/
    char *name = calloc(Fsize, sizeof(char));
    char *left = calloc(Fsize, sizeof(char));
    char *right = calloc(Fsize, sizeof(char));
    FILE *fp, *fpout;


    /* reads from input.txt, writes to output.txt*/
    if ((fp = fopen("input.txt", "r")) == NULL) {
        printf("Error opening file");
        exit(1);
    }
    if ((fpout = fopen("output.txt", "w")) == NULL) {
        printf("Error opening file");
        exit(1);
    }

    int k;
    for (k = 0; k < inputs; k++) {
        fscanf(fp, "%s", name);
        switch (parse(name)) {
            case (0):
                fprintf(fpout, "%s is not a formula.\n", name);
                break;
            case (1):
                fprintf(fpout, "%s is a proposition.\n ", name);
                break;
            case (2):
                fprintf(fpout, "%s is a negation.\n", name);
                break;
            case (3):
                fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", name, partone(name), parttwo(name));
                break;
            default:
                printf("Invalid input!\n");
        }

        if (parse(name) != 0) {
            /* Initialize the tableau with the formula */
            char *s = calloc(Fsize, sizeof(char));
            strcpy(s, name);
            struct set *S = calloc(1, sizeof(struct set));
            S->item = s;
            S->tail = NULL;
            struct tableau *t = calloc(1, sizeof(struct tableau));
            t->S = S;
            t->rest = NULL;

            /* Completes the tableau and checks if it is closed */
            complete(t);
            if (closed(t)) {
                fprintf(fpout, "%s is not satisfiable.\n", name);
            } else {
                fprintf(fpout, "%s is satisfiable.\n", name);
            }

            /* Frees all the bits of memory on the tableau*/
            free(t);
        } else {
            fprintf(fpout, "I told you, %s is not a formula.\n", name);
        }
    }

    fclose(fp);
    fclose(fpout);
    free(left);
    free(right);
    free(name);

    return 0;
}