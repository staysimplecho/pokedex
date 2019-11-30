// Assignment 2 19T1 COMP1511: Pokedex
// pokedex.c
//
// This program was written by Sijia Hao (z5141464)
// on 24/04/2019
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: Minor clarifications about `struct pokenode`.
// Version 1.1.0: Moved destroy_pokedex function to correct location.
// Version 1.1.1: Renamed "pokemon_id" to "id" in change_current_pokemon.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Add any extra #includes your code needs here.
//
// But note you are not permitted to use functions from string.h
// so do not #include <string.h>

#include "pokedex.h"

// add your own #defines here.
#define DIFF_UP_LOW 32


// Note you are not permitted to use arrays in struct pokedex,
// you must use a linked list.
//
// You are also not permitted to use arrays in any of the functions
// below.
//
// The only exception is that char arrays are permitted for
// search_pokemon and functions it may call, as well as the string
// returned by pokemon_name (from pokemon.h).
//
// You will need to add fields to struct pokedex.
// You may change or delete the head field.

struct pokedex {
    struct pokenode *head;
    struct pokenode *tail;
    struct pokenode *curr;
    int             n_total;
};

// You don't have to use the provided struct pokenode, you are free to
// make your own struct instead.
// If you use the provided struct pokenode, you will need to add fields
// to it to store other information.

struct pokenode {
    struct pokenode *prev;
    struct pokenode *next;
    struct pokenode *evolve;
    int             found;
    Pokemon         pokemon;
};

// Add any other structs you define here.

// a pointer to a pokenode
typedef struct pokenode *Node;

// Add prototypes for any extra functions you create here.
static int  check_existence(Pokedex pokedex, int id);
static Node is_in_pokedex(Pokedex pokedex, int id, Node pokenode);
static int  n_in_range(Pokedex pokedex, int factor);
static int  has_multi_types(Pokemon pokemon);
static void print_asterisks(Pokemon pokemon);
static void print_each_name(Node selected, Node pokenode);
static int  cases_invalid_evolution(Pokedex pokedex, int from_id, int to_id);
static void print_evo_info(Node pokenode);
static void print_type_formatted(Node pokenode);
static int  cases_invalid_type(pokemon_type type);
static int  type_match(Node pokenode, pokemon_type type);
static void insertion_sort(Pokedex pokedex);
static void insert_into_position(Pokedex pokedex, Node key);
static int  name_match(Node pokenode, char *text);
static int  str_len(char *string);
static void str_cpy(char *des, char *src, int len);
static void to_lower(char *string);
static int  equal_strings(char *des, char *src, int len_src);

// You need to implement the following 20 functions.
// In other words, replace the lines calling fprintf & exit with your code.
// You can find descriptions of what each function should do in pokedex.h

Pokedex new_pokedex(void) {
    Pokedex new_pokedex = malloc(sizeof(struct pokedex));
    assert(new_pokedex != NULL);
    new_pokedex->head = NULL;
    new_pokedex->tail = NULL;
    new_pokedex->curr = NULL;
    new_pokedex->n_total = 0;
    return new_pokedex;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

void add_pokemon(Pokedex pokedex, Pokemon pokemon) {
    /*  no need to check if pokemon is NULL
        since new_pokemon() in pokemon.c and do_add() in main.c did for us
    if (pokemon == NULL) {
        fprintf(stderr, "Invalid Pokemon\n");
        exit(1);
    }
    */

    /*  For an empty list
        1>  malloc memory block for a new pokenode
        2>  initialize the new pokenode
        3>  update pokedex
        4>  change current selected pokemon to this pokemon
    */
    if (pokedex->n_total == 0) {
        Node pokenode = malloc(sizeof(struct pokenode));
        assert(pokenode != NULL);
        pokenode->prev = NULL;
        pokenode->next = NULL;
        pokenode->evolve = NULL;
        pokenode->found = 0;
        pokenode->pokemon = pokemon;
        pokedex->head = pokenode;
        pokedex->tail = pokenode;
        pokedex->curr = pokenode;
        (pokedex->n_total)++;
    } else {
    /*  General case for non-empty list
        1> check for existence-------------------|--------pokemon exists-------
        2> malloc memory block for a new pokenode|2> destroy(free) this pokemon
        3> update and append it to the tail      |3> print error message
        4> update pokedex                        |4> exit program
    */
        if (check_existence(pokedex, pokemon_id(pokemon)) == 1) {
            destroy_pokemon(pokemon);
            fprintf(stderr, "Same ID detected in current Pokedex\n");
            exit(1);
        }
        Node pokenode = malloc(sizeof(struct pokenode));
        assert(pokenode != NULL);
        pokenode->prev = pokedex->tail;
        pokenode->next = NULL;
        pokenode->evolve = NULL;
        pokenode->found = 0;
        pokenode->pokemon = pokemon;
        pokedex->tail->next = pokenode;
        pokedex->tail = pokenode;
        (pokedex->n_total)++;
    }
}

void detail_pokemon(Pokedex pokedex) {
    // if the list is empty then do nothing
    if (pokedex->n_total == 0) {
        return;
    }
    // general cases: found or not + 2 types or 1 type
    if (pokedex->curr->found == 0) {
        printf("Id: %03d\nName: ", pokemon_id(pokedex->curr->pokemon));
        print_asterisks(pokedex->curr->pokemon);
        printf("Height: --\nWeight: --\nType: --\n");
    } else {
        int first_type = pokemon_first_type(pokedex->curr->pokemon);
        printf("Id: %03d\nName: %s\nHeight: %.1lfm\nWeight: %.1lfkg\nType: %s",
                                            pokemon_id(pokedex->curr->pokemon),
                                          pokemon_name(pokedex->curr->pokemon),
                                        pokemon_height(pokedex->curr->pokemon),
                                        pokemon_weight(pokedex->curr->pokemon),
                                           pokemon_type_to_string(first_type));
        if (has_multi_types(pokedex->curr->pokemon) == 0) {
            printf("\n");
        } else {
            int second_type = pokemon_second_type(pokedex->curr->pokemon);
            printf(" %s\n", pokemon_type_to_string(second_type));
        }
    }
}

Pokemon get_current_pokemon(Pokedex pokedex) {
    // if the Pokedex is empty then exit
    if (pokedex->n_total == 0) {
        fprintf(stderr, "The Pokedex is empty! See U :)\n");
        exit(1);
    }

    return pokedex->curr->pokemon;
}

void find_current_pokemon(Pokedex pokedex) {
    // if the Pokedex is empty then do nothing
    if (pokedex->n_total != 0) {
        pokedex->curr->found = 1;
    }
}

void print_pokemon(Pokedex pokedex) {
    // if the Pokedex is empty then do nothing
    Node temp = pokedex->head;
    while (temp != NULL) {
        print_each_name(pokedex->curr, temp);
        temp = temp->next;
    }
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

void next_pokemon(Pokedex pokedex) {
    /*  error controlling: do nothing
        1>  an empty list, this should be checked first since accessing 
            pokedex->curr->next will be invalid if the list is empty
        2>  reach the end of the list
    */
    if (pokedex->n_total != 0 && pokedex->curr->next != NULL) {
        pokedex->curr = pokedex->curr->next;
    }
}

void prev_pokemon(Pokedex pokedex) {
    /*  error controlling: do nothing
        1>  an empty list, this should be checked first since accessing 
            pokedex->curr->prev will be invalid if the list is empty
        2>  reach the head of the list
    */
    if (pokedex->n_total != 0 && pokedex->curr->prev != NULL) {
        pokedex->curr = pokedex->curr->prev;
    }
}

void change_current_pokemon(Pokedex pokedex, int id) {
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (id == pokemon_id(temp->pokemon)) {
            pokedex->curr = temp;
            return;
        }
        temp = temp->next;
    }
}

void remove_pokemon(Pokedex pokedex) {
    /*  cases:
        1>  empty list -> do nothing
        2>  only one Pokemon in the Pokedex -> NULL
        3>  curr points to the head of the Pokedex -> points to new head
        4>  curr points to the tail of the Pokedex -> points to new tail
        5>  normal case
    */
    if (pokedex->n_total == 0) {
        return;
    } else if (pokedex->n_total == 1) {
        Node delete = pokedex->curr;
        destroy_pokemon(delete->pokemon);
        pokedex->head = NULL;
        pokedex->tail = NULL;
        pokedex->curr = NULL;
        (pokedex->n_total)--;
        free(delete);
    } else if (pokedex->curr->prev == NULL) {
        Node delete = pokedex->curr;
        destroy_pokemon(delete->pokemon);
        pokedex->head = pokedex->curr->next;
        pokedex->head->prev = NULL;
        pokedex->curr = pokedex->head;
        (pokedex->n_total)--;
        free(delete);
    } else if (pokedex->curr->next == NULL) {
        Node delete = pokedex->curr;
        destroy_pokemon(delete->pokemon);
        pokedex->tail = pokedex->tail->prev;
        pokedex->tail->next = NULL;
        pokedex->curr = pokedex->tail;
        (pokedex->n_total)--;
        free(delete);
    } else {
        Node delete = pokedex->curr;
        destroy_pokemon(delete->pokemon);
        pokedex->curr->prev->next = pokedex->curr->next;
        pokedex->curr->next->prev = pokedex->curr->prev;
        pokedex->curr = pokedex->curr->next;
        (pokedex->n_total)--;
        free(delete);
    }
}

void destroy_pokedex(Pokedex pokedex) {
    /*  cases:
        1>  non-empty list, free Pokemon, Pokenode, Pokedex
        2>  empty list, free Pokedex
    */
    if (pokedex->n_total != 0) {
        while (pokedex->head != NULL) {
            Node delete = pokedex->head;
            pokedex->head = pokedex->head->next;
            destroy_pokemon(delete->pokemon);
            free(delete);
        }
    }
    free(pokedex);
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

void go_exploring(Pokedex pokedex, int seed, int factor, int how_many) {
    //  error controlling
    if (pokedex->n_total < how_many) {
        fprintf(stderr, "Cannot find that many Pokemons\n");
        exit(1);
    } else if (n_in_range(pokedex, factor) < how_many) {
        fprintf(stderr, "Cannot find that many Pokemons\n");
        exit(1);
    }

    // normal case
    srand(seed);
    while (how_many > 0) {
        int id = rand();
        if (id >= 0 && id < factor) {
            Node id_node = NULL;
            id_node = is_in_pokedex(pokedex, id, id_node);
            if (id_node != NULL) {
                id_node->found = 1;
                how_many--;
            }
        }
    }
}

int count_found_pokemon(Pokedex pokedex) {
    int n_found = 0;
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (temp->found == 1) {
            n_found++;
        }
        temp = temp->next;
    }
    return n_found;  
}

int count_total_pokemon(Pokedex pokedex) {
    return pokedex->n_total;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

void add_pokemon_evolution(Pokedex pokedex, int from_id, int to_id) {
    //  error controlling
    if (cases_invalid_evolution(pokedex, from_id, to_id) == 1) {    
        fprintf(stderr, "Invalid Pokemon Evolution!\n");
        exit(1);
    }

    Node from_node = NULL;
    Node to_node = NULL;
    from_node = is_in_pokedex(pokedex, from_id, from_node);
    to_node = is_in_pokedex(pokedex, to_id, to_node);
    from_node->evolve = to_node;
}

void show_evolutions(Pokedex pokedex) {
    // if the Pokedex is empty then exit
    if (pokedex->n_total == 0) {
        fprintf(stderr, "The Pokedex is empty! See U :)\n");
        exit(1);
    }

    Node temp = pokedex->curr;
    print_evo_info(temp);
    temp = temp->evolve;
    while (temp != NULL) {
        printf(" --> ");
        print_evo_info(temp);
        temp = temp->evolve;
    }
    printf("\n");
}

int get_next_evolution(Pokedex pokedex) {
    // if the Pokedex is empty then exit
    if (pokedex->n_total == 0) {
        fprintf(stderr, "The Pokedex is empty! See U :)\n");
        exit(1);
    }

    if (pokedex->curr->evolve != NULL) {
        return pokemon_id(pokedex->curr->evolve->pokemon);
    } else {
        return DOES_NOT_EVOLVE;
    }  
}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////

Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {
    // error controlling
    if (cases_invalid_type(type) == 1) {
        fprintf(stderr, "Invalid Pokemon type for collection\n");
        exit(1);
    }

    // construct a new pokedex
    Pokedex type_pokedex = new_pokedex();

    // loop throught the main Pokedex to find matching Nodes
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (type_match(temp, type) == 1 && temp->found == 1) {
            add_pokemon(type_pokedex, clone_pokemon(temp->pokemon));
            type_pokedex->tail->found = 1;
        }
        temp = temp->next;
    }
    return type_pokedex;
}

Pokedex get_found_pokemon(Pokedex pokedex) {
    // construct a new pokedex
    Pokedex found_pokedex = new_pokedex();

    // loop throught the main Pokedex to find matching Nodes
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (temp->found == 1) {
            add_pokemon(found_pokedex, clone_pokemon(temp->pokemon));
            found_pokedex->tail->found = 1;
        }
        temp = temp->next;
    }

    // sort the Pokedex when it is not empty
    if (found_pokedex->n_total != 0) {
        insertion_sort(found_pokedex);
    }

    return found_pokedex;
}

Pokedex search_pokemon(Pokedex pokedex, char *text) {
    // construct a new pokedex
    Pokedex ch_pokedex = new_pokedex();
    
    // if text contains invalid characters to have in a Pokemon name
    if (pokemon_valid_name(text) == 0) {
        return ch_pokedex;
    }

    // loop throught the main Pokedex to find matching Nodes
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (temp->found == 1 && name_match(temp, text) == 1) {
            add_pokemon(ch_pokedex, clone_pokemon(temp->pokemon));
            ch_pokedex->tail->found = 1;
        }
        temp = temp->next;
    }
    return ch_pokedex;
}

// Add definitions for your own functions below.
// Make them static to limit their scope to this file.

// Check if the given pokemon_id belongs to an existing Pokemon in the Pokedex
// Return 1 if YES
// Return 0 if NO
static int check_existence(Pokedex pokedex, int id) {
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (pokemon_id(temp->pokemon) == id) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// Advanced function for checking the existence of a given id
// ...They have different return values which distinguish their usage for different purposes
// Return NULL if NO
// Return a pointer to that Pokenode if YES
static Node is_in_pokedex(Pokedex pokedex, int id, Node pokenode) {
    Node temp = pokedex->head;
    while (temp != NULL) {
        if (pokemon_id(temp->pokemon) == id) {
            pokenode = temp;
            return pokenode;
        }
        temp = temp->next;
    }
    return NULL;
}

// Count how many Pokemons in the Pokedex have an id in range of 0 ~ (factor-1)
static int n_in_range(Pokedex pokedex, int factor) {
    int n = 0;
    Node temp = pokedex->head;
    while (temp != NULL)
    {
        int id = pokemon_id(temp->pokemon);
        if (id >= 0 && id < factor) {
            n++;
        }
        temp = temp->next;
    }
    return n;
}

// Check if a Pokemon has two types
// Return 1 is YES --- 2 types
// Return 0 is NO  --- 1 type
static int has_multi_types(Pokemon pokemon) {
    int type2 = pokemon_second_type(pokemon);
    if (type2 == NONE_TYPE) {
        return 0;
    } else {
        return 1;
    }
}

// Print out the name of a Pokemon by replacing letters with asterisks
static void print_asterisks(Pokemon pokemon) {
    int i = 0;
    char *name = pokemon_name(pokemon);
    while (name[i] != '\0') {
        printf("*");
        i++;
    }
    printf("\n");
}


// Print out the id and name of a Pokemon
// --> would be attached if it is the current selected Pokemon
// The same number of *s are used for printing its name if it is not found yet
static void print_each_name(Node selected, Node pokenode) {
    if (pokemon_id(pokenode->pokemon) == pokemon_id(selected->pokemon)) {
        printf("--> ");
    } else {
        printf("    ");
    }
    printf("#%03d: ", pokemon_id(pokenode->pokemon));
    if (pokenode->found == 1) {
        printf("%s\n", pokemon_name(pokenode->pokemon));
    } else {
        print_asterisks(pokenode->pokemon);
    }
}

// Check if an evolution is invalid due to
// 1> Pokemon with from_id or to_id is not in the Pokedex
// 2> from_id and to_id are the same
// Return 1 if INVALID
// Return 0 if VALID
static int cases_invalid_evolution(Pokedex pokedex, int from_id, int to_id) {
    if (from_id == to_id) {
        return 1;
    } else if (check_existence(pokedex, from_id) == 0) {
        return 1;
    } else if (check_existence(pokedex, to_id) == 0) {
        return 1;
    } else {
        return 0;
    }
}

// Print out the id, name and type of a Pokemon
// ? used for printing its name and type if it is not found yet
static void print_evo_info(Node pokenode) {
    printf("#%03d", pokemon_id(pokenode->pokemon));
    if (pokenode->found == 1) {
        printf(" %s", pokemon_name(pokenode->pokemon));
        print_type_formatted(pokenode);
    } else {
        printf(" ???? [????]");
    }
}

// Print out the type of a Pokemon in []
static void print_type_formatted(Node pokenode) {
    if (has_multi_types(pokenode->pokemon) == 0) {
        int first_type = pokemon_first_type(pokenode->pokemon);
        printf(" [%s]", pokemon_type_to_string(first_type));
    } else {
        int first_type = pokemon_first_type(pokenode->pokemon);
        int second_type = pokemon_second_type(pokenode->pokemon);
        printf(" [%s, %s]", pokemon_type_to_string(first_type),
                            pokemon_type_to_string(second_type));
    }
}

// Check if the given type makes no sense to the collection
// Return 1 if INVALID
// Return 0 if VALID
static int  cases_invalid_type(pokemon_type type) {
    if (type == INVALID_TYPE) {
        return 1;
    } else if (type == NONE_TYPE) {
        return 1;
    } else if (type == MAX_TYPE) {
        return 1;
    } else {
        return 0;
    }
}

// Check if a Pokemon has one type that matches the given type
// Return 1 is YES
// Return 0 is NO
static int type_match(Node pokenode, pokemon_type type) {
    if (pokemon_first_type(pokenode->pokemon) == type) {
        return 1;
    } else if (pokemon_second_type(pokenode->pokemon) == type) {
        return 1;
    } else {
        return 0;
    }
}

// Sorting the pokedex in ascending order of pokemon_id
// An implementation of insertion sort
static void insertion_sort(Pokedex pokedex) {
    Node temp = pokedex->head->next;
    while (temp != NULL) {
        insert_into_position(pokedex, temp);
        temp = temp->next;
    }
}

// Positioning the key node in the partially ascending ordered Pokedex
// Only migrate informations rather than the links(i.e. *prev and *next)
// An example is shown as follows:
// ---------------------------------------------------------------------------
// | 2 | 4 | 5 | 9 | 14 | key | next | 27365 | 8 | 44 | 45 | list continues...
// ---------------------------------------------------------------------------
// <--appear in order-->  key
static void insert_into_position(Pokedex pokedex, Node pokenode) {
    // save a copy of the given pokenode
    Node key = malloc(sizeof(struct pokenode));
    assert(key != NULL);
    key->prev = NULL;
    key->next = NULL;
    key->evolve = pokenode->evolve;
    key->found = pokenode->found;
    key->pokemon = pokenode->pokemon;
    int id = pokemon_id(key->pokemon);
    
    Node temp = pokenode;
    while (temp->prev != NULL && pokemon_id(temp->prev->pokemon) > id) {
        temp->evolve = temp->prev->evolve;
        temp->found = temp->prev->found;
        temp->pokemon = temp->prev->pokemon;
        temp = temp->prev;
    }
    temp->evolve = key->evolve;
    temp->found = key->found;
    temp->pokemon = key->pokemon;
    free(key);
}

// Check if a Pokemon was found and has a name containing the given string
// Return 1 if YES
// Return 0 if NO
static int name_match(Node pokenode, char *text) {
    int len_name = str_len(pokemon_name(pokenode->pokemon));
    char *name = malloc((len_name + 1) * sizeof(char));
    assert(name != NULL);
    str_cpy(name, pokemon_name(pokenode->pokemon), len_name);
    to_lower(name);

    int len_text = str_len(text);
    char *given = malloc((len_name + 1) * sizeof(char));
    assert(given != NULL);
    str_cpy(given, text, len_name);
    to_lower(given);

    // For example, name[8 chars] = "Squirtle", text[3 chars] = "tle",
    // at name[len_name - len_text + 1] = name[6] = 'l',
    // the matching would never happen
    int i = 0;
    while (i <= len_name - len_text) {
        if (name[i] == given[0]) {
            if (equal_strings(name + i, given, len_text) == 1) {
                free(name);
                free(given);
                return 1;
            }
        }
        i++;
    }
    free(name);
    free(given);
    return 0;
}

// Count the number of characters in a string
// '\0' is not included
static int str_len(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}

// Copy source string to destination string
static void str_cpy(char *des, char *src, int len) {
    int i = 0;
    while (i < len + 1) {
        des[i] = src[i];
        i++;
    }
}

// Swap letters in a string to lower case
static void to_lower(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] >= 'A' && string[i] <= 'Z') {
            string[i] += DIFF_UP_LOW;
        }
        i++;
    }
}

// Check if des string is the same as src string
static int equal_strings(char *des, char *src, int len_src) {
    int i = 0;
    int n = 0;
    while (i < len_src) {
        if (des[i] == src[i]) {
            n++;
        }
        i++;
    }
    if (n == len_src) {
        return 1;
    } else {
        return 0;
    }
}
