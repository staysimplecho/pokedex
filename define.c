// NOTICE           struct pokenode                                  
// NOTICE               *Node                                        
// NOTICE           struct pokedex  pokemon_type                     
// NOTICE              *Pokedex     *Pokemon     struct pokemon      
// NOTICE  pokedex.c ◀ pokedex.h ◀ pokemon.h ▶ pokemon.c           
// NOTICE                   ⬇                                        
// NOTICE                 main.c                                     

// NOTICE  Pokedex list related 
// a pointer to a pokedex template,e.g. new_pokedex points to a pokedex struct
typedef struct pokedex *Pokedex;

struct pokedex {
    struct pokenode *head;
    struct pokenode *tail;      // point to the last pokenode in the list
    struct pokenode *curr;      // point to the currently selected node
    int             n_total;    // store the total number of pokemons in pokedex
};

// a pointer to a pokenode
typedef struct pokenode *Node;

struct pokenode {
    struct pokenode *prev;      // point to the pokenode in the list before that one
    struct pokenode *next;
    int             found;
    Pokemon         pokemon;
};

// NOTICE  Pokemon info related 
// a pointer to a pokemon info struct
typedef struct pokemon *Pokemon;

typedef enum pokemon_type {
    INVALID_TYPE = -1,
    NONE_TYPE,
    NORMAL_TYPE,
    FIRE_TYPE,
    FIGHTING_TYPE,
    WATER_TYPE,
    FLYING_TYPE,
    GRASS_TYPE,
    POISON_TYPE,
    ELECTRIC_TYPE,
    GROUND_TYPE,
    PSYCHIC_TYPE,
    ROCK_TYPE,
    ICE_TYPE,
    BUG_TYPE,
    DRAGON_TYPE,
    GHOST_TYPE,
    DARK_TYPE,
    STEEL_TYPE,
    FAIRY_TYPE,
    MAX_TYPE
} pokemon_type;

// FIXME  cannot access this struct directly 
struct pokemon {
    int          magic_number;
    int          pokemon_id;
    char         *name;
    double       height;
    double       weight;
    pokemon_type type1;
    pokemon_type type2;
};

const static char *types[] = {
    [NONE_TYPE]     = "None",
    [NORMAL_TYPE]   = "Normal",
    [FIRE_TYPE]     = "Fire",
    [FIGHTING_TYPE] = "Fighting",
    [WATER_TYPE]    = "Water",
    [FLYING_TYPE]   = "Flying",
    [GRASS_TYPE]    = "Grass",
    [POISON_TYPE]   = "Poison",
    [ELECTRIC_TYPE] = "Electric",
    [GROUND_TYPE]   = "Ground",
    [PSYCHIC_TYPE]  = "Psychic",
    [ROCK_TYPE]     = "Rock",
    [ICE_TYPE]      = "Ice",
    [BUG_TYPE]      = "Bug",
    [DRAGON_TYPE]   = "Dragon",
    [GHOST_TYPE]    = "Ghost",
    [DARK_TYPE]     = "Dark",
    [STEEL_TYPE]    = "Steel",
    [FAIRY_TYPE]    = "Fairy",
};


// shift + alt + A          ▶ block comment
// windows + .              ▶ emoji symbols
// ctrl + PgUp/PgDn         ▶ switch file left/right
// ctrl + alt + L/R Arrow   ▶ split file to left/right



/*
Your program produced these errors:
=================================================================

pokedex.c:553 runtime error - malloc use after free

dcc explanation: access to memory that has already been freed.

Execution stopped here in print_each_name(selected=0x604000000210, pokenode=0x604000000190) in pokedex.c at line 553:

// The same number of *s are used for printing its name if it is not found yet
static void print_each_name(Node selected, Node pokenode) {
--> if (pokemon_id(pokenode->pokemon) == pokemon_id(selected->pokemon)) {
        printf("--> ");
    } else {


Function Call Traceback
print_each_name(selected=0x604000000210, pokenode=0x604000000190) called at line 203 of pokedex.c
print_pokemon(pokedex=0x603000000130) called at line 349 of main.c
do_print(pokedex=0x603000000130) called at line 123 of main.c
run_command(pokedex=0x603000000130, line=0x7ffe277aea80 "p") called at line 92 of main.c
explore_pokedex(supplied_pokedex=0x0) called at line 77 of main.c
main()

The input for this test was:
a 6 Squirtle 0.5 9 water None
a 7 Bulbasaur 0.7 6.9 poison grass
a 2 Ivysaur 1 13 grass poison
a 21 Spearow 0.3 2.0 flying normal
a 42 Golbat 1.6 55.0 flying poison
f
m 2
r
p
m 42
r
p
m 6
r
p

You can reproduce this test by executing these commands:
dcc pokedex.c pokemon.c main.c -o pokedex
echo -e 'a 6 Squirtle 0.5 9 water None\na 7 Bulbasaur 0.7 6.9 poison grass\na 2 Ivysaur 1 13 grass poison\na 21 Spearow 0.3 2.0 flying normal\na 42 Golbat 1.6 55.0 flying poison\nf\nm 2\nr\np\nm 42\nr\np\nm 6\nr\np' | ./pokedex
*/

/*
 
*/