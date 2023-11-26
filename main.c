#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "murmurhash3.h"
#define ROTL32(x,y) ((x << y) | (x >> (32 - y)))

// import jednotlyvých struktúr
#include "AVLTree.c"
#include "Splay.c"
#include "linear_p.c"
#include "hash_table_with_chaining.c"

// makrá
#define NOTEST 1000000 // najvacsi pocet prvkov
#define TESTARRAY 1000000
#define TESTSTRINGS 1000000
#define STRINGLNG 25

#define NANO 1000000000

#define NUM_SYMBOLS 52
#define NUM_DIGITS 10
#define NUM_SPECIAL_CHARS 4

// Stringy
char symbols[NUM_SYMBOLS] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                             'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
char digits[NUM_DIGITS] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
char specialChars[NUM_SPECIAL_CHARS] = {'?', '!', '.', ','};
char** generateRandomStrings(int numStrings, int strLen) {
    char** strings = malloc(numStrings * sizeof(char*));
    int i, j, index;

    for (i = 0; i < numStrings; i++) {
        strings[i] = malloc((strLen+1) * sizeof(char));
        for (j = 0; j < strLen; j++) {
            if (j == 0) {
                index = rand() % NUM_SYMBOLS;
                strings[i][j] = symbols[index];
            }
            else if (j < strLen - 3) {
                int r = rand() % 3;
                if (r == 0) {
                    index = rand() % NUM_SYMBOLS;
                    strings[i][j] = symbols[index];
                }
                else if (r == 1) {
                    index = rand() % NUM_DIGITS;
                    strings[i][j] = digits[index];
                }
                else {
                    index = rand() % NUM_SPECIAL_CHARS;
                    strings[i][j] = specialChars[index];
                }
            }
            else {
                index = rand() % NUM_DIGITS;
                strings[i][j] = digits[index];
            }
        }
        strings[i][strLen] = '\0';
    }

    return strings;
}

// jednotlivé testy
// AVL
AVLNode *testAVL(AVLNode* rootavl, long testArray[], char* strings[], int goal){
    struct timespec beginAVL, endAVL;
    for (int j = 0; j < 10; j++) {
        clock_gettime(CLOCK_REALTIME, &beginAVL);
        for (int i = 0; i < goal; i++) {
            rootavl = AVLinsert(rootavl, testArray[i], strings[i]);
        }
        clock_gettime(CLOCK_REALTIME, &endAVL);
    }
    return rootavl;
}
void testAVLinsert(AVLNode* rootavl, long testArray[], char* strings[], int goal){
    struct timespec beginAVL, endAVL;
    long long secondsAVL = 0;
    long long nanosecondsAVL = 0;
    double averageAVL = 0.0;
    long decoy = 0;
    for(int k =0; k < 100000; k++){
       decoy = rand()*rand() / 15520;
    }
    for (int j = 0; j < 10; j++) {
    clock_gettime(CLOCK_REALTIME, &beginAVL);
    for (int i = 0; i < goal; i++) {
        rootavl = AVLinsert(rootavl, testArray[i], strings[i]);
    }
    clock_gettime(CLOCK_REALTIME, &endAVL);
        secondsAVL = endAVL.tv_sec - beginAVL.tv_sec;
        nanosecondsAVL = endAVL.tv_nsec - beginAVL.tv_nsec;
        averageAVL += (secondsAVL + nanosecondsAVL * 1e-9);
    }
    printf("AVL INS   | %7d | %lf\n",goal, ((averageAVL/10)*NANO)/goal);
}
void testAVLsearch(AVLNode* rootavl, long testArray[], char* strings[], int goal){
    rootavl = testAVL(rootavl, testArray, strings, goal);
    struct timespec beginAVL, endAVL;
    long secondsAVL = 0;
    long nanosecondsAVL = 0;
    double averageAVL = 0.0;
    // Start measuring time
    for (int j = 0; j < 10; j++) {
        clock_gettime(CLOCK_REALTIME, &beginAVL);
        for (int i = 0; i < goal; i++) {
            AVLsearch(rootavl, testArray[rand() * rand() % TESTARRAY]);
        }
        // Stop measuring time and calculate the elapsed time
        clock_gettime(CLOCK_REALTIME, &endAVL);
        secondsAVL = endAVL.tv_sec - beginAVL.tv_sec;
        nanosecondsAVL = endAVL.tv_nsec - beginAVL.tv_nsec;
        averageAVL += (secondsAVL + nanosecondsAVL * 1e-9);
    }
    printf("AVL SRC   | %7d | %lf\n", goal, (((averageAVL)*NANO)/goal)/10);
}
void testAVLdelete(AVLNode* rootavl, long testArray[], int goal) {
    struct timespec beginAVL, endAVL;
    long secondsAVL = 0;
    long nanosecondsAVL = 0;
    double averageAVL = 0.0;
    for(int k =0; k < 100000; k++){
        long decoy = rand()*rand() / 15520;
    }
    for (int j = 0; j < 10; j++) {
        clock_gettime(CLOCK_REALTIME, &beginAVL);
        for (int i = 0; i < goal; i++) {
            AVLdelete(rootavl, testArray[rand() * rand() % TESTARRAY]);
        }
        clock_gettime(CLOCK_REALTIME, &endAVL);
        secondsAVL = endAVL.tv_sec - beginAVL.tv_sec;
        nanosecondsAVL = endAVL.tv_nsec - beginAVL.tv_nsec;
        averageAVL += (secondsAVL + nanosecondsAVL * 1e-9);
    }
    printf("AVL DEL   | %7d | %lf\n", goal, (((averageAVL)*NANO)/goal)/10);
}

// SPLAY
SPLAYNode* testSPLAY(SPLAYNode* rootsplay, long testArray[], char* strings[], int goal){
    struct timespec beginSPLAY, endSPLAY;
    for(int j = 0; j < 10; j++){
        // Start measuring time
        clock_gettime(CLOCK_REALTIME, &beginSPLAY);
        for (long i = 0; i < goal; i++) {
            rootsplay = splayInsert(rootsplay, testArray[i], strings[i]);
        }
        // Stop measuring time and calculate the elapsed time
        clock_gettime(CLOCK_REALTIME, &endSPLAY);
    }
    return rootsplay;
}
void testSplayinsert(SPLAYNode* rootsplay, long testArray[], char* strings[], int goal){
    struct timespec beginSPLAY, endSPLAY;
    long secondsSPLAY = 0;
    long nanosecondsSPLAY = 0;
    double averageSplay = 0.0;
    for(int k =0; k < 100000; k++){
        long decoy = rand()*rand() / 15520;
        decoy += 2587 * 52;
    }
    for(int j = 0; j < 10; j++){
    clock_gettime(CLOCK_REALTIME, &beginSPLAY);
    for (long i = 0; i < goal; i++) {
        rootsplay = splayInsert(rootsplay, testArray[i], strings[i]);
    }
    clock_gettime(CLOCK_REALTIME, &endSPLAY);
    secondsSPLAY = endSPLAY.tv_sec - beginSPLAY.tv_sec;
    nanosecondsSPLAY = endSPLAY.tv_nsec - beginSPLAY.tv_nsec;
    averageSplay += secondsSPLAY + nanosecondsSPLAY * 1e-9;
    }

    printf("SPLAY INS | %7d | %lf\n",goal, ((averageSplay/10)*NANO)/goal);
}
void testSplaysearch(SPLAYNode* rootsplay, long testArray[], char* strings[], int goal){
    rootsplay = testSPLAY(rootsplay, testArray, strings, goal);
    struct timespec beginSPLAY, endSPLAY;
    long secondsSPLAY = 0;
    long nanosecondsSPLAY = 0;
    double averageSplay = 0.0;
    for(int j = 0; j < 10; j++){
        clock_gettime(CLOCK_REALTIME, &beginSPLAY);
        for (int i = 0; i < goal; i++) {
            splaySearch(rootsplay, testArray[rand() * rand() % TESTARRAY]);
        }
        clock_gettime(CLOCK_REALTIME, &endSPLAY);
        secondsSPLAY = endSPLAY.tv_sec - beginSPLAY.tv_sec;
        nanosecondsSPLAY = endSPLAY.tv_nsec - beginSPLAY.tv_nsec;
        averageSplay += secondsSPLAY + nanosecondsSPLAY * 1e-9;
    }
    printf("SPLAY SRC | %7d | %lf\n", goal,  ((averageSplay*NANO)/goal)/10);
}
void testSplaydelete(SPLAYNode* rootsplay, long testArray[], char* strings[], int goal){
    struct timespec beginSPLAY, endSPLAY;
    long secondsSPLAY = 0;
    long nanosecondsSPLAY = 0;
    double averageSplay = 0.0;
    for(int j = 0; j < 10; j++){
        clock_gettime(CLOCK_REALTIME, &beginSPLAY);
        for (int i = 0; i < goal; i++) {
            splayDelete(rootsplay, testArray[rand() * rand() % TESTARRAY]);
        }
        clock_gettime(CLOCK_REALTIME, &endSPLAY);
        secondsSPLAY = endSPLAY.tv_sec - beginSPLAY.tv_sec;
        nanosecondsSPLAY = endSPLAY.tv_nsec - beginSPLAY.tv_nsec;
        averageSplay += secondsSPLAY + nanosecondsSPLAY * 1e-9;
    }
    printf("SPLAY DEL | %7d | %lf\n", goal,  ((averageSplay*NANO)/goal)/10);
}

// CHAIN
void testchain(HashChain* table, char**  strings, int goal){
    struct timespec beginChain, endChain;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        insertChain(table, strings[i], rand()%goal);
    }
    clock_gettime(CLOCK_REALTIME, &endChain);

}
void testChaininsert(HashChain* table, char**  strings, int goal){
    // Start measuring time
    struct timespec beginChain, endChain;
    double averageChain = 0;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        insertChain(table, strings[i], rand()%goal);
    }
    // Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("CHAIN INS | %7d | %lf\n",goal, (averageChain*NANO)/goal);
}
void testChainsearch(HashChain* table, char**  strings, int goal){
    testchain(table, strings, goal);
    // Start measuring time
    struct timespec beginChain, endChain;
    double averageChain = 0;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        searchChain(table, strings[(rand()*rand()) % TESTARRAY]);
    }
    // Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("CHAIN SRC | %7d | %lf\n",goal, (averageChain*NANO)/goal);
}
void testChaindelete(HashChain* table, char**  strings, int goal){
    double averageChain = 0;
    struct timespec beginChain, endChain;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        deleteChain(table, strings[i]);
    }
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("CHAIN DEL | %7d | %lf\n",goal, (averageChain*NANO)/goal);
}

// LP
void testlp(HashTable * ht, char**  strings, int goal){
    struct timespec beginChain, endChain;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        hash_table_insert(ht, strings[i], rand()%goal);
    }
    clock_gettime(CLOCK_REALTIME, &endChain);
}
void testlinearprobing(HashTable * ht, char**  strings, int goal){
    // Start measuring time
    struct timespec beginChain, endChain;
    double averageChain = 0;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        hash_table_insert(ht, strings[i], rand()%goal);
    }
    // Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("LP INS    | %7d | %lf\n",goal, (averageChain*NANO)/goal);
}
void testlinearprobingsearch(HashTable * ht, char**  strings, int goal){
    testlp(ht, strings, goal);
    // Start measuring time
    struct timespec beginChain, endChain;
    double averageChain = 0;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        hash_table_search(ht, strings[(rand()*rand()) % goal]);
    }
    // Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("LP DEL    | %7d | %lf\n", goal, (averageChain*NANO)/goal);
}
void testlinearprobingdelete(HashTable * ht, char**  strings, int goal){
    double averageChain = 0;
    // Start measuring time
    struct timespec beginChain, endChain;
    clock_gettime(CLOCK_REALTIME, &beginChain);
    for (int i = 0; i < goal; i++){
        hash_table_remove(ht, strings[i]);
    }
    // Stop measuring time and calculate the elapsed time
    clock_gettime(CLOCK_REALTIME, &endChain);
    long secondsChain = endChain.tv_sec - beginChain.tv_sec;
    long nanosecondsChain = endChain.tv_nsec - beginChain.tv_nsec;
    averageChain += secondsChain + nanosecondsChain * 1e-9;
    printf("LP DEL    | %7d | %lf\n",goal, (averageChain*NANO)/goal);
}

// main
int main() {
    srand (time (NULL));

    // vytvorenie 1M rand() čísel a 1M rand() stringov
    long *testArray = malloc(TESTARRAY * sizeof(long));
    if (testArray == NULL) {
        printf("Failed to allocate memory for testArray\n");
        return 1;
    }
    for (long i = 0; i < TESTARRAY; i++) {
        testArray[i] = rand() * rand();
    }
    char** strings = generateRandomStrings(TESTSTRINGS, STRINGLNG);

    AVLNode *rootavl = NULL;
    HashChain *table = create_table_chain(97);
    HashTable* ht = hash_table_init(97);
    SPLAYNode *rootsplay = createNode(rand(), strings[rand()]);

    // decoy na rozbehnutie procesora
    for(int k =0; k < 100000; k++){
        long decoy = rand()*rand() / 15520;
        decoy += 2587 * 52;
    }

    //long goal = 10000;
    long goal = 25000;

    // test Insert
    printf("\nInsert\n");
    while(goal <= NOTEST){
//        testAVLinsert(rootavl, testArray, strings, goal);
//        testSplayinsert(rootsplay, testArray, strings, goal);
//        testChaininsert(table, strings,goal);
//        testlinearprobing(ht, strings, goal);
        goal += 25000;
       // goal += 10000;
    }

     goal = 10000;
     goal = 25000;
    printf("\n--------------------------------\n");

    // test Search
    printf("\nSearch\n");
    while(goal <= NOTEST){
//            testAVLsearch(rootavl, testArray, strings, goal);
//            testSplaysearch(rootsplay, testArray, strings, goal);
//            testChainsearch(table, strings,goal);
//            testlinearprobingsearch(ht, strings, goal);
            goal += 25000;
    //        goal += 10000;
    }

    //goal = 10000;
    goal = 25000;
    printf("\n--------------------------------\n");

    // test Delete
    printf("\nDelete\n");
    while(goal <= NOTEST){
/*
            rootavl = testAVL(rootavl, testArray, strings, goal);
            testAVLdelete(rootavl, testArray, goal);
*/
/*
            rootsplay = testSPLAY(rootsplay, testArray, strings, goal);
            testSplaydelete(rootsplay, testArray, strings, goal);
*/
/*
            testchain(table, strings, goal);
            testChaindelete(table, strings,goal);
*/
/*
            testlp(ht, strings, goal);
            testlinearprobingdelete(ht, strings, goal);
*/
        goal += 25000;
        //goal += 10000;
    }

    // reset
    free(rootsplay);
    free(rootavl);
    free(testArray);
    free(table);
    hash_table_delete_lp(ht);
    for (long i = 0; i < TESTSTRINGS; i++) {
        free(strings[i]);
    }
    free(strings);

    return 0;
}