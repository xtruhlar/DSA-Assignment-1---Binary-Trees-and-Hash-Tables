#include <stdio.h>
#include <stdlib.h>


// Štruktúra nodu
typedef struct AVLNode {
    long long key;
    char* string;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

// Pomocná funkcia pre vytvorenie nového nodu
AVLNode *create_AVLNode(int key, char* string) {
    AVLNode *new_AVLNode = (AVLNode *) malloc(sizeof(AVLNode));
    new_AVLNode->key = key;
    new_AVLNode->string = string;
    new_AVLNode->left = NULL;
    new_AVLNode->right = NULL;
    new_AVLNode->height = 1; // Výška nového nodu je 1
    return new_AVLNode;
}

// Pomocná funkcia pre zistenie výšky nodu
int height(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return root->height;
}

// Pomocná funkcia pre zistenie max
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Pomocná funckia pre zistenie nodu s minimalnou hodnoutou
AVLNode *min_value_node(AVLNode *n) {
    AVLNode *current = n;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Pomocná funkcia pre výpočet balance faktoru
int balance_factor(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return height(root->left) - height(root->right);
}

// Rotácia stromu doľava
AVLNode *rotate_left(AVLNode *root) {
    AVLNode *new_root = root->right;
    AVLNode *subtree = new_root->left;
    new_root->left = root;
    root->right = subtree;

    // Aktualizácia výšok nodov
    root->height = 1 + max(height(root->left), height(root->right));
    new_root->height = 1 + max(height(new_root->left), height(new_root->right));

    return new_root;
}

// Rotácia stromu doprava
AVLNode *rotate_right(AVLNode *root) {
    AVLNode *new_root = root->left;
    AVLNode *subtree = new_root->right;
    new_root->right = root;
    root->left = subtree;

    // Aktualizácia výšok nodov
    root->height = 1 + max(height(root->left), height(root->right));
    new_root->height = 1 + max(height(new_root->left), height(new_root->right));

    return new_root;
}

// Funkcia pre vkladanie nového nodu do stromu
AVLNode * AVLinsert(AVLNode * node, int key, char* string) {
    if (node == NULL) {
        return create_AVLNode(key, string);
    }

    if (key < node->key) {
        node->left = AVLinsert(node->left, key, string);
    }
    else if (key > node->key) {
        node->right = AVLinsert(node->right, key, string);
    }
    else {
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balance_factor(node);

    if (balance > 1 && key < node->left->key) {
        return rotate_right(node);
    }
    if (balance < -1 && key > node->right->key) {
        return rotate_left(node);
    }
    if (balance > 1 && key > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

// Prehľadávanie stromu
AVLNode * AVLsearch(AVLNode *root, int key) {
// Strom je prázdny alebo kľúč sa nachádza v koreni
    while (root != NULL && root->key != key) {
        if (key < root->key) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root;

}

// Vymazanie nodu zo stromu
AVLNode *AVLdelete(AVLNode *root, int key) {
    // Krok 1: Normálne odstránenie nodu v BVS
    if (root == NULL) {
        return root;
    }

    if (key < root->key) {
        root->left = AVLdelete(root->left, key);
    } else if (key > root->key) {
        root->right = AVLdelete(root->right, key);
    } else {
        // node má maximálne jedno dieťa
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode *temp = root->left ? root->left : root->right;

            // Prípad: node nemá žiadne dieťa
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                // Prípad: node má jedno dieťa
                *root = *temp;
            }
            free(temp);
        } else {
            // node má dve deti
            AVLNode *temp = min_value_node(root->right);

            root->key = temp->key;

            root->right = AVLdelete(root->right, temp->key);
        }
    }
    // Krok 2: Ak strom má len jeden node
    if (root == NULL) {
        return root;
    }

    // Krok 3: Aktualizácia výšky aktuálneho nodu
    root->height = 1 + max(height(root->left), height(root->right));

    // Krok 4: Výpočet rozdielu výšok podstromov aktuálneho nodu
    int balance = balance_factor(root);

    // Krok 5: Výber jednej zo štyroch možností rotácie
    // Left Left Case
    if (balance > 1 && balance_factor(root->left) >= 0) {
        return rotate_right(root);
    }

    // Left Right Case
    if (balance > 1 && balance_factor(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    // Right Right Case
    if (balance < -1 && balance_factor(root->right) <= 0) {
        return rotate_left(root);
    }

    // Right Left Case
    if (balance < -1 && balance_factor(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}