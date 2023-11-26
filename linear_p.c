#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR_LP 0.90
#define SHRINK_FACTOR_LP 0.25

// Definícia nodu
typedef struct HashTable {
    char** keys;
    int* values;
    int size;
    int count;
} HashTable;

// Pôvodná hash funckia, ktorá bola veľmi pomalá a mala množstvo kolízií
/*
int hash_f(char* key, int table_len){
    int hash = 0;
    for (int i = 0; i < strlen(key); i++){
        hash += (int)key[i] + ((int)key[i]) * (i + 1);
    }
    return hash %= table_len;
}
 */


// Hashovacia funkcia murmur3
uint32_t hash_function_lp(const void* key, size_t len) {
    int leng = 25;
    const uint8_t* data = (const uint8_t*)key;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t r1 = 15;
    const uint32_t r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;
    uint32_t hash = 0x1b873593;
    uint32_t k;

    for (size_t i = 0; i < leng; i += 4) {
        k = *(uint32_t*)(data + i);
        k *= c1;
        k = ROTL32(k, r1);
        k *= c2;

        hash ^= k;
        hash = ROTL32(hash, r2);
        hash = hash * m + n;
    }

    const uint8_t* tail = (const uint8_t*)(data + (leng & ~3));
    k = 0;
    switch (len & 3) {
        case 3: k ^= tail[2] << 16;
        case 2: k ^= tail[1] << 8;
        case 1: k ^= tail[0];
                k *= c1;
                k = ROTL32(k, r1);
                k *= c2;
                hash ^= k;
    }

    hash ^= leng;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    // mod by the table size to get a value within the table range
    return hash % len;
}
/* Credits: https://github.com/jrslepak/murmur3/blob/master/MurmurHash3.h*/


// Funkcia, ktorá inicializuje tabuľku o veľkosti "size"
HashTable* hash_table_init(int size) {
    HashTable* ht = malloc(sizeof(HashTable));
    ht->size = size;
    ht->keys = calloc(size, sizeof(char*));
    ht->values = calloc(size, sizeof(int));
    ht->count = 0;
    return ht;
}


// Funkcia, ktorá dealokuje pamäť, ktorú tabuľka mala alokovanú
void hash_table_delete_lp(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        if (ht->keys[i]) {
            free(ht->keys[i]);
        }
    }
    free(ht->keys);
    free(ht->values);
    free(ht);
}


// Funkcia, ktorá "resizuje" tabuľku podľa load faktora
void hash_table_resize_lp(HashTable* ht, int new_size) {
    // Alokácia pamäte pre nové pole kľúčov a hodnôt
    char** new_keys = calloc(new_size, sizeof(char*));
    int* new_values = calloc(new_size, sizeof(int));
    // Pre každý prvok v pôvodnej tabuľke skontroluj, či má kľúč,
    for (int i = 0; i < ht->size; i++) {
        if (ht->keys[i]) {
            int index = hash_function_lp(ht->keys[i], new_size);
            while (new_keys[index]) {
                index = (index + 1) % new_size;
            }
            // ak áno, prirad mu nový index a ulož do nového poľa
            new_keys[index] = ht->keys[i];
            new_values[index] = ht->values[i];
        }
    }
    // Uvoľnenie starého poľa kľúčov a hodnôt
    free(ht->keys);
    free(ht->values);
    ht->keys = new_keys;
    ht->values = new_values;
    ht->size = new_size;
}


// Funckia, ktorá vloží node do tabuľky
void hash_table_insert(HashTable* ht, char* key, int value) {
    // Kontrola, či sa tabuľka nepreplnila. Ak áno, zväčšíme ju pomocou lineárneho zväčšovania.
    if ((float)ht->count / ht->size >= LOAD_FACTOR_LP) {
        hash_table_resize_lp(ht, ht->size * 2);
    }
    // Vypočítaj index pomocou hashovacej funkcie
    int index = hash_function_lp(key, ht->size);
    // Prechádzaj tabuľku, kým nenájdeš voľné miesto alebo prvok s rovnakým kľúčom
    while (ht->keys[index]) {
        if (strcmp(ht->keys[index], key) == 0) {
            ht->values[index] = value;
            return;
        }
        // Ak sa voľné miesto nenašlo, pokračuj hľadaním na ďalšom mieste v tabuľke.
        index = (index + 1) % ht->size;
    }
    // Vložiť prvok do voľného miesta v tabuľke.
    ht->keys[index] = strdup(key);
    ht->values[index] = value;
    ht->count++;
}


// Funckia, ktorá vyhľadá node v tabuľke
int hash_table_search(HashTable* ht, char* key) {
    // Vypočítaj index pomocou hashovacej funkcie
    int index = hash_function_lp(key, ht->size);
    while (ht->keys[index]) {
        // Porovnaj kľúčový reťazec s aktuálnym prvkom
        if (strcmp(ht->keys[index], key) == 0) {
            // Ak sa kľúčové reťazce zhodujú, vráť hodnotu pre daný kľúč
            return ht->values[index];
        }
        // Ak sa kľúčový reťazec nezhoduje, pokračuj hľadaním na ďalšom mieste v tabuľke
        index = (index + 1) % ht->size;
    }
    return 0;
}


// Funkcia, ktorá odstráni node z tabuľky
void hash_table_remove(HashTable* ht, char* key) {
    // Vypočítaj index pomocou hashovacej funkcie
    int index = hash_function_lp(key, ht->size);
    // Prechádzaj tabuľkou kým nenájdeš prvok alebo nenarazíš na prázdne miesto
    while (ht->keys[index]) {
        // Ak sa kľúčové reťazce zhodujú, uvolni pamäť pre kľúčový reťazec
        if (strcmp(ht->keys[index], key) == 0) {
            free(ht->keys[index]);
            ht->keys[index] = NULL;
            // Nastav hodnotu na 0 a zniž počet prvkov v tabuľke.
            ht->values[index] = 0;
            ht->count--;

            // Rehashovanie zvyšných prvkov tabuľky, ktoré boli posunuté pri odstraňovaní prvku
            int next_index = (index + 1) % ht->size;
            while (ht->keys[next_index]) {
                // Ulož kľúč a hodnotu
                char* temp_key = ht->keys[next_index];
                int temp_value = ht->values[next_index];
                // Vynuluj hodnoty a zniž počet prvkov v tabuľke
                ht->keys[next_index] = NULL;
                ht->values[next_index] = 0;
                ht->count--;
                // Vlož prvok na správne miesto v tabuľke
                hash_table_insert(ht, temp_key, temp_value);
                next_index = (next_index + 1) % ht->size;
            }
            // Ak sa počet prvkov v tabuľke zmenšil pod určitú hranicu, resizeuj tabuľku
            if ((float)ht->count / ht->size <= SHRINK_FACTOR_LP && ht->size > 1) {
                hash_table_resize_lp(ht, ht->size / 2);
            }
            return;
        }
        // Ak sa kľúčový reťazec nezhoduje, pokračuj hľadaním na ďalšom mieste v tabuľke
        index = (index + 1) % ht->size;
    }
}