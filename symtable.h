
#ifndef __STABLE_H__
#define __STABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"

#define HASHCAPACITY 1000


// Tabulka:
struct htab;    // neúplná deklarace struktury - uživatel nevidí obsah

typedef struct htab htab_t;     
//struct htab_item;

// Typy:
typedef const char * htab_key_t;        // typ klíče
typedef int htab_value_t;               // typ hodnoty

// Dvojice dat v tabulce:
typedef struct htab_pair {
    htab_key_t    key;          // klíč
    htab_value_t  value;        // asociovaná hodnota
    pNode arguments;
    expr* pointer;               // ukazatel na definici
} htab_pair_t;                  // typedef podle zadání


struct htab {
    size_t  size;
    size_t  arraySize;
    struct htab_item *records[];
};     // typedef podle zadání

struct htab_item{
    htab_pair_t *data;
    struct htab_item* next;
};       


// Rozptylovací (hash) funkce 
size_t htab_hash_function(htab_key_t str);

htab_pair_t *pair_init(htab_key_t key);

// Funkce pro práci s tabulkou:
htab_t *htab_init(size_t n);                    // konstruktor tabulky
htab_t *htab_move(size_t n, htab_t *from);      // přesun dat do nové tabulky
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t);     // velikost pole

htab_pair_t * htab_find(htab_t * t, htab_key_t key);  // hledání
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky


#endif //__STABLE_H__