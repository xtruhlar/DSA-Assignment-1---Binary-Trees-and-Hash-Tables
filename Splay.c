#include <stdio.h>
#include <stdlib.h>


// Štruktúra nodu
typedef struct SplayNode {
    long long key;
    char* string;
    struct SplayNode *left;
    struct SplayNode *right;
} SPLAYNode;


// Pomocná funkcia pre vytvorenie nového nodu
SPLAYNode *createNode(int key, char* string) {
    SPLAYNode *SplayNode = (SPLAYNode*)malloc(sizeof(SPLAYNode));
    SplayNode->key = key;
    SplayNode->string = string;
    SplayNode->left = NULL;
    SplayNode->right = NULL;
    return SplayNode;
}

// Rotácia stromu doprava
SPLAYNode *rightRotate(SPLAYNode *x) {
    SPLAYNode *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
    // v implementací Splay netreba aktualizovať výšku ako v AVL
}

// Rotácia stromu doľava
SPLAYNode *leftRotate(SPLAYNode *x) {
    SPLAYNode *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

// Pomocná funkcia pre zistenie nodu s minimalnou hodnotou
SPLAYNode *min_value_nodeS(SPLAYNode *n) {
    SPLAYNode *current = n;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Funckia splay, ktorá zabezpečuje pridanie najnovšieho nodu do koreňa
SPLAYNode *splay(SPLAYNode *root, long key) {
    // Ak sme narazili na list alebo uzol s kľúčom, ktorý hľadáme, vrátime ho
    if (root == NULL || root->key == key) {
        return root;
    }
    // Hľadanie v ľavom podstrome
    if (root->key > key) {
        // Ak ľavý podstrom neexistuje, vrátime aktuálny uzol
        if (root->left == NULL) {
            return root;
        }
        // Rotácia na základe dvoch ďalších podmienok
        if (root->left->key > key) {
            // Splay na ľavom ľavom podstrome
            root->left->left = splay(root->left->left, key);
            // Prava rotácia na aktuálnom uzle
            root = rightRotate(root);
        } else if (root->left->key < key) {
            // Splay na ľavom pravom podstrome
            root->left->right = splay(root->left->right, key);
            // Ak existuje pravý podstrom, urobíme najskôr ľavú rotáciu na ľavom podstrome
            if (root->left->right != NULL) {
                root->left = leftRotate(root->left);
            }
        }
        // Ak ľavý podstrom neexistuje, vrátime aktuálny uzol
        if (root->left == NULL) {
            return root;
        } else {
            // Inak urobíme pravú rotáciu na aktuálnom uzle
            return rightRotate(root);
        }
    } else {
        if (root->right == NULL) {
            return root;
        }
        // Inak pouzijeme rotaciu doprava.
        if (root->right->key > key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left != NULL) {
                root->right = rightRotate(root->right);
            }
        } else if (root->right->key < key) {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }
        // Ak pravy podstrom neexistuje, vratime koren. Inak pouzijeme rotaciu do lava
        if (root->right == NULL) {
            return root;
        } else {
            return leftRotate(root);
        }
    }
}

// Funkcia pre vkladanie nového nodu do stromu
SPLAYNode *splayInsert(SPLAYNode *root, long key, char* string) {
    // Ak je koreň NULL, vytvoríme nový node a vrátime ho
    if (root == NULL) {
        return createNode(key, string);
    }
    // Vykoname operaciu "splay" na koreni splay stromu.
    root = splay(root, key);
    // Ak node s hladanym klucom uz existuje, vratime ho.
    if (root->key == key) {
        return root;
    }
    // Inak vytvorime novy node a umiestnime ho na spravne miesto v strome.
    SPLAYNode *SplayNode = createNode(key, string);
    if (root->key > key) {
        SplayNode->right = root;
        SplayNode->left = root->left;
        root->left = NULL;
    } else {
        SplayNode->left = root;
        SplayNode->right = root->right;
        root->right = NULL;
    }
    return SplayNode;
}

// Prehľadávanie stromu
SPLAYNode *splaySearch(SPLAYNode *root, long key) {
    // Strom je prázdny alebo kľúč sa nachádza v koreni
    while (root != NULL && root->key != key) {
        if (key < root->key) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    root = splay(root, key);
    return root;

}

// Vymazanie nodu zo stromu
SPLAYNode *splayDelete(SPLAYNode *root, long key) {
    // Krok 1: Normálne odstránenie nodu v BVS
    if (root == NULL) {
        return root;
    }

    SPLAYNode *temp;
    // rekurzívne vyhľadávanie nodu, ktorý chceme vymazať
    if (root->key < key) {
        root->right = splayDelete(root->right, key);
    } else if (root->key > key) {
        root->left = splayDelete(root->left, key);
    } else {
        // prípad: má pravé dieťa
        if (root->left == NULL) {
            temp = root->right;
            free(root);
            return temp;
        // prípad: má ľavé dieťa
        } else if (root->right == NULL) {
            temp = root->left;
            free(root);
            return temp;
        }
        // Prípad: Uzol ma obe deti. Najdeme nasledovnika nodu a nahradime node jeho hodnotou
        temp = min_value_nodeS(root->right);
        root->key = temp->key;
        root->right = splayDelete(root->right, temp->key);
    }

    return root;
}