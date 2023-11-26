#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Faktor záťaže
#define LOAD_FACTOR 1.5
#define SHRINK_FACTOR 0.45

// Definícia nodu
typedef struct HashWithChaining {
    char* key;
    int value;
    struct HashWithChaining* next;
} HashWithChaining;

// Definícia poľa spájaných zoznamov
typedef struct HashChain {
    int size;
    int count;
    HashWithChaining** array;
} HashChain;

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
uint32_t hash_function_chain(const void* key, size_t len) {
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


// Funckia, ktorá vytvorí nový node, s daným value a key
HashWithChaining* create_node_chain(char* key, int value) {
    HashWithChaining* node = (HashWithChaining*) malloc(sizeof(HashWithChaining));
    node->key = key;
    node->value = value;
    node->next = NULL;
    return node;
}


// Funckia, ktorá vytvrorí tabuľku o "size" veľkosti
HashChain* create_table_chain(int size) {
    HashChain* table = (HashChain*) malloc(sizeof(HashChain));
    table->size = size;
    table->count = 0;
    table->array = (HashWithChaining**) calloc(size, sizeof(HashWithChaining*));
    return table;
}


// Funckia, ktorá vloží nový node do tabuľky
void insertChain(HashChain* table, char* key, int value) {
    // Získame index pomocou hashovacej funkcie
    long index = hash_function_chain(key, table->size);
    HashWithChaining* current = table->array[index];
    // Uložíme si aj predchádzajúci prvok
    HashWithChaining* prev = NULL;

    // Prechádzame cez reťazec a hľadáme, či už tam daný kľúč neexistuje
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Ak sa nájde kľúč, upravíme hodnotu a končíme
            current->value = value;
            return;
        }
        prev = current;
        current = current->next;
    }

    // Ak sme nenášli kľúč, vytvoríme nový prvok
    HashWithChaining* node = create_node_chain(key, value);

    // Ak sme nenášli žiadny prvok v reťazci, uložíme nový prvok na začiatok reťazca
    if (prev == NULL) {
        table->array[index] = node;
    } else {
        // Inak uložíme nový prvok za posledný prvok v reťazci
        prev->next = node;
    }
    // Zvýšime počet prvkov v hashovacej tabuľke
    table->count++;

    // Vypočítame load factor
    double load_factor = (double) table->count / table->size;

    // Ak load factor prekročí hodnotu LOAD_FACTOR, vytvoríme novú hashovaciu tabuľku a presunieme do nej prvky z pôvodnej
    if (load_factor >= LOAD_FACTOR) {
        HashChain* new_table = create_table_chain(table->size * 2);

        for (int i = 0; i < table->size; i++) {
            HashWithChaining* current = table->array[i];
            while (current != NULL) {
                insertChain(new_table, current->key, current->value);
                current = current->next;
            }
        }

        // Nahradíme pôvodnú tabuľku novou
        *table = *new_table;
        free(new_table);
    }
}


// Funkcia, ktorá vymaže node z tabuľky
void deleteChain(HashChain* table, char* key) {
    // Získanie indexu položky, v tabuľke pomocou hashovacej funckie
    int index = hash_function_chain(key, table->size);
    HashWithChaining *current = table->array[index];
    HashWithChaining *prev = NULL;

    // Prechádzanie reťazcom, kým sa nenájde položka s daným kľúčom alebo sa nedostaneme na koniec reťazca
    while (current != NULL) {
        // Porovnanie kľúča s aktuálnou položkou
        if (strcmp(current->key, key) == 0) {
            // Odstránenie aktuálnej položky z reťazca
            if (prev == NULL) {
                table->array[index] = current->next;
            } else {
                prev->next = current->next;
            }
            // Uvoľnenie pamäti alokovanej pre aktuálnu položku
            free(current);
            // Dekrementácia počtu položiek v tabuľke
            table->count--;

            // Výpočet koeficientu záťaže tabuľky
            double load_factor = (double) table->count / table->size;

            // Ak sa koeficient záťaže dostal pod SHRINK_FACTOR a veľkosť tabuľky je väčšia ako 1, zmenšíme veľkosť tabuľky
            if (load_factor <= SHRINK_FACTOR && table->size > 1) {
                HashChain *new_table = create_table_chain(table->size / 2);

                // Prechádzanie celej pôvodnej tabuľky a vkladanie položiek do novej tabuľky
                for (int i = 0; i < table->size; i++) {
                    HashWithChaining *
                            current = table->array[i];
                    while (current != NULL) {
                        insertChain(new_table, current->key, current->value);
                        HashWithChaining *temp = current;
                        current = current->next;
                        free(temp);
                    }
                }

                // Nahradíme pôvodnú tabuľku novou
                *table = *new_table;
                free(new_table);
            }

            return;
        }
        prev = current;
        current = current->next;
    }
}


// Funckia, ktorá prehľadá tabuľku, a vráti hľadaný node
char* searchChain(HashChain* table, char* key) {
    // Získanie indexu položky, v tabuľke pomocou hashovacej funckie
    int index = hash_function_chain(key, table->size);
    // Vyhľadanie prvej položky v danom retazci
    HashWithChaining *current = table->array[index];
    // Prehľadávanie reťazca až do konca
    while (current != NULL) {
        // Porovnanie kľúča s aktuálnou položkou
        if (strcmp(current->key, key) == 0) {
            return current->key;
        }
        current = current->next;
    }

    return "N/A";
}