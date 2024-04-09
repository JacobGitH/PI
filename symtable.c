
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "symtable.h"

// počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t){
    return t->arraySize;
}

// ruší všechny záznamy
void htab_clear(htab_t * t){
        size_t i = 0;
        size_t index = 0;
        while(index < t->arraySize){
            if(t->records[index]){
                struct htab_item *tmpErase = t->records[index];
                while(t->records[index]->next){
                    t->records[index]=t->records[index]->next;
                    free((char *)tmpErase->data->key);
                    free(tmpErase->data);
                    free(tmpErase);
                    i++;
                    tmpErase = t->records[index];
                }
                if(t->records[index]){
                    free((char *)tmpErase->data->key);
                    free(t->records[index]->data);
                    free(t->records[index]);
                    t->records[index]=NULL;
                    i++;
                }
            }
            index++;
            if(i==t->size){
                t->size = 0;
                return;
            }
        }
}

// ruší zadaný záznam
bool htab_erase(htab_t * t, htab_key_t key){
    size_t index = htab_hash_function(key) % t->arraySize;
    if(t->records[index]) {
        struct htab_item *tmp = t->records[index];
        if(tmp->data->key){
            if(strcmp(tmp->data->key,key)==0){
                t->records[index] = t->records[index]->next;
                free(tmp);
                t->size--;
                return 1;
            }
            while(tmp->next){
                if(tmp->next->data->key && strcmp(tmp->next->data->key,key)==0){
                    if(tmp->next->next){
                        struct htab_item *tmpErase = tmp->next;
                        tmp->next = tmp->next->next;
                        free(tmpErase);
                        t->size--;
                        return 1;
                    }
                    free(tmp->next);
                    tmp->next = NULL;
                    t->size--;
                    return 1;
                }
                tmp = tmp->next;
            }
        }
    }
    return 0;
}

htab_pair_t * htab_find(htab_t * t, htab_key_t key) {
    size_t index = htab_hash_function(key) % t->arraySize;
    if(t->records[index]) {        //lookup
        struct htab_item *tmpItem = t->records[index];
        while (tmpItem->data->key) {
            if (strcmp(tmpItem->data->key, key) == 0) {
                return tmpItem->data;
            }
            if (tmpItem->next) {
                tmpItem = tmpItem->next;
            } else {
                return NULL;
            }
        }
    }
    return NULL;
}  // hledání

// for_each: projde všechny záznamy a zavolá na ně funkci f
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)){
    for(size_t i = 0; i < t->arraySize; i++){
        if(t->records[i]){
            struct htab_item *tmpItem = t->records[i];
            f(tmpItem->data);
            while(tmpItem->next){
                tmpItem = tmpItem->next;
                f(tmpItem->data);
            }
        }
    }
}

// destruktor tabulky
void htab_free(htab_t * t){
    htab_clear(t);
    free(t);
}

// Rozptylovací (hash) funkce
size_t htab_hash_function(htab_key_t str){
    u_int32_t h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}

// konstruktor tabulky
htab_t *htab_init(size_t n) {
    htab_t *table =malloc(sizeof(htab_t) + (sizeof(struct htab_item *)) * n +1);
    if(table == NULL){
        exit(99);
    }
    table->size=0;
    table->arraySize=n;
    for(size_t i = 0; i < n ; i++){
        table->records[i]= NULL;
    }
    return table;
}

struct htab_pair *pair_init(htab_key_t key){
    struct htab_pair *tmpPair = malloc(sizeof(struct htab_pair));
    if(tmpPair==NULL){
        exit(99);
    }
    tmpPair->key = malloc(strlen(key)+1);
    strcpy((char *)tmpPair->key,key);
    tmpPair->value = 0;
    tmpPair->arguments = NULL;

    return tmpPair;
}

htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key){
    size_t index = htab_hash_function(key) % t->arraySize;
    struct htab_pair *tmpPair;
    if((tmpPair = htab_find(t, key))){          //lookup
        return tmpPair;
    }else{                                      //create
        tmpPair =  pair_init(key);
        struct htab_item *tmpItem = t->records[index];
        struct htab_item *tmpIt = malloc(sizeof(struct htab_item));
        if(t->records[index]) {
            while (t->records[index]->next) {
                t->records[index] = t->records[index]->next;
            }
            tmpIt->data = tmpPair;
            tmpIt->next = NULL;
            t->records[index]->next = tmpIt;
            t->records[index] = tmpItem;
        }else{
            tmpIt->data = tmpPair;
            tmpIt->next = NULL;
            t->records[index]=tmpIt;
        }
        t->size++;
        return tmpPair;
    }
}

// počet záznamů v tabulce
size_t htab_size(const htab_t * t){
    return t->size;
}