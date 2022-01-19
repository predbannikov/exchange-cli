#ifndef NODE_H
#define NODE_H
/* Самобалансирующиеся деревья Ордеров и Номеров(OID)*/

#include <stdio.h>
#include <stdlib.h>
#include "PriceLevel.h"

#define EPS 0.0001
#define ABS(x) ((x) < 0 ? - (x) : (x))
#define cmpLTOID(a, b) (a < b)
#define cmpEQOID(a, b) (a == b)
#define cmpLT(a, b) (a < b)
#define cmpEQ(a, b) (ABS(a - b) <= EPS)
/* листья деревьев */
#define NIL &sentinel
#define NILOID &sentineloid

/* метка для красно-чёрного дерева */
typedef enum { BLACK, RED } nodeColor;

/* соответствие номеров ордеров к прайсам */
typedef struct OID_ {
    unsigned int oid;
    float price;
    char side;
} OID;

/* дерево номеров OID */
typedef struct NodeOID_ {
    struct NodeOID_ *left;
    struct NodeOID_ *right;
    struct NodeOID_ *parent;
    nodeColor color;
    OID data;
} NodeOID;

/* дерево прайсов */
typedef struct Node_ {
    struct Node_ *left;
    struct Node_ *right;
    struct Node_ *parent;
    nodeColor color;
    PriceLevel *price_level;
} Node;

/* блоки для инициализации листьев */
Node sentinel = { NIL, NIL, 0, BLACK, 0};
NodeOID sentineloid = { NILOID, NILOID, 0, BLACK, 0};

/**************************
 *  поворот ноды x в лево *
 **************************/
void rotateLeft(Node** glass_tree, Node *x) {
    Node* root = *glass_tree;
    Node *y = x->right;

    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;

    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    else {
        root = y;
    }

    y->left = x;
    if (x != NIL) x->parent = y;
    *glass_tree = root;
}

/****************************
 *  поворот ноды x в право  *
 ****************************/
void rotateRight(Node** glass_tree, Node *x) {
    Node* root = *glass_tree;
    Node *y = x->left;

    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;

    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    }
    else {
        root = y;
    }

    y->right = x;
    if (x != NIL) x->parent = y;
    *glass_tree = root;
}

/*******************************
 *  Балансировка дерева после  *
 *  вставки ноды x             *
 *******************************/
void insertFixup(Node** glass_tree, Node *x) {

    Node* root = *glass_tree;

    while (x != root && x->parent->color == RED) {
        if (x->parent == x->parent->parent->left) {
            Node *y = x->parent->parent->right;
            if (y->color == RED) {

                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                if (x == x->parent->right) {
                    x = x->parent;
                    rotateLeft(&root, x);
                }

                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRight(&root, x->parent->parent);
            }
        } else {

            Node *y = x->parent->parent->left;
            if (y->color == RED) {

                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                if (x == x->parent->left) {
                    x = x->parent;
                    rotateRight(&root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateLeft(&root, x->parent->parent);
            }
        }
    }
    root->color = BLACK;
    *glass_tree = root;
}

/************************************
 *  Выделение памяти и всавка ноды  *
 ************************************/
Node *insertNode(Node** glass_tree, Order *ord) {
    static int counter = 0;
    counter++;
    Node* current, *parent, *x;
    Node* root = *glass_tree;

    current = root;
    parent = 0;
    while (current != NIL) {
        if (cmpEQ(ord->price, current->price_level->head->value->price)) {

            push_back_list(current->price_level, ord);
            return (current);
        }
        parent = current;
        current = cmpLT(ord->price, current->price_level->head->value->price) ?
            current->left : current->right;
    }

    if ((x = malloc (sizeof(Node))) == 0) {
        fprintf (stderr, "error: insufficient memory (insertNode)\n");
        exit(EXIT_FAILURE);
    }

    x->parent = parent;
    x->left = NIL;
    x->right = NIL;
    x->color = RED;
    x->price_level = createLinkedList();
    push_back_list(x->price_level, ord);

    if(parent) {
        if(cmpLT(ord->price, parent->price_level->head->value->price))
            parent->left = x;
        else
            parent->right = x;
    } else {
        root = x;
    }

    insertFixup(&root, x);
    *glass_tree = root;
    return(x);
}

/***********************************************
 *  Балансировка дерева после удаления ноды x  *
 ***********************************************/
void deleteFixup(Node** glass_tree, Node *x) {

    Node* root = *glass_tree;

    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeft (&root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRight (&root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeft (&root, x->parent);
                x = root;
            }
        } else {
            Node *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRight (&root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeft (&root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRight (&root, x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
    *glass_tree = root;
}


/******************************
 *  Удаление ноды z и дерева  *
 ******************************/
void deleteNode(Node** glass_tree, Node *z) {
    Node *x, *y;
    Node* root = *glass_tree;

    free(z->price_level);

    if (!z || z == NIL) return;


    if (z->left == NIL || z->right == NIL) {
        y = z;
    } else {
        y = z->right;
        while (y->left != NIL) y = y->left;
    }

    if (y->left != NIL)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;
    if (y->parent)
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    else
        root = x;

    if (y != z) z->price_level = y->price_level;


    if (y->color == BLACK)
        deleteFixup (&root, x);

    free(y);
    *glass_tree = root;
}

/************************************
 *  Найти ноду (Ордеров) по прайсу  *
 ************************************/
Node *findNode(Node** glass_tree, float price) {
    Node *current = *glass_tree;
    while(current != NIL)
        if(cmpEQ(price, current->price_level->head->value->price))
            return (current);
        else
            current = cmpLT (price, current->price_level->head->value->price) ?
                current->left : current->right;
    return(0);
}



/**************************
 *  поворот ноды x в лево *
 **************************/
void rotateLeftOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;

    NodeOID *y = x->right;

    x->right = y->left;
    if (y->left != NILOID) y->left->parent = x;

    if (y != NILOID) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    } else {
        root = y;
    }

    y->left = x;
    if (x != NILOID) x->parent = y;
    *tree = root;
}

/****************************
 *  поворот ноды x в право  *
 ****************************/
void rotateRightOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;

    NodeOID *y = x->left;

    x->left = y->right;
    if (y->right != NILOID) y->right->parent = x;

    if (y != NILOID) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    } else {
        root = y;
    }

    y->right = x;
    if (x != NILOID) x->parent = y;
    *tree = root;
}

/*******************************
 *  Балансировка дерева после  *
 *  вставки ноды x             *
 *******************************/
void insertFixupOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;


    while (x != root && x->parent->color == RED) {
        if (x->parent == x->parent->parent->left) {
            NodeOID *y = x->parent->parent->right;
            if (y->color == RED) {

                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                if (x == x->parent->right) {
                    x = x->parent;
                    rotateLeftOID(&root, x);
                }

                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRightOID(&root, x->parent->parent);
            }
        } else {

            NodeOID *y = x->parent->parent->left;
            if (y->color == RED) {

                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                if (x == x->parent->left) {
                    x = x->parent;
                    rotateRightOID(&root, x);
                }
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateLeftOID(&root, x->parent->parent);
            }
        }
    }
    root->color = BLACK;
    *tree = root;
}

/************************************
 *  Выделение памяти и всавка ноды  *
 ************************************/
NodeOID *insertNodeOID(NodeOID** tree, OID data) {
    NodeOID* current, *parent, *x;
    NodeOID* root = *tree;

    current = root;
    parent = 0;
    while (current != NILOID) {
        if (cmpEQOID(data.oid, current->data.oid)) {
            return (current);
        }
        parent = current;
        current = cmpLTOID(data.oid, current->data.oid) ?
            current->left : current->right;
    }

    if ((x = (NodeOID*) malloc (sizeof(*x))) == 0) {
        fprintf (stderr, "error: insufficient memory (insertNodeOID)\n");
        exit(EXIT_FAILURE);
    }

    x->data = data;
    x->parent = parent;
    x->left = NILOID;
    x->right = NILOID;
    x->color = RED;

    if(parent) {
        if(cmpLTOID(data.oid, parent->data.oid))
            parent->left = x;
        else
            parent->right = x;
    } else {
        root = x;
    }

    insertFixupOID(&root, x);
    *tree = root;
    return(x);
}

/***********************************************
 *  Балансировка дерева после удаления ноды x  *
 ***********************************************/
void deleteFixupOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;

    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            NodeOID *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateLeftOID (&root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rotateRightOID (&root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rotateLeftOID (&root, x->parent);
                x = root;
            }
        } else {
            NodeOID *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rotateRightOID (&root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rotateLeftOID (&root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rotateRightOID (&root, x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
    *tree = root;
}


/******************************
 *  Удаление ноды z и дерева  *
 ******************************/
void deleteNodeOID(NodeOID** tree, NodeOID *z) {
    NodeOID *x, *y;
    NodeOID* root = *tree;

    if (!z || z == NILOID) return;


    if (z->left == NILOID || z->right == NILOID) {
        y = z;
    } else {
        y = z->right;
        while (y->left != NILOID) y = y->left;
    }

    if (y->left != NILOID)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;
    if (y->parent)
        if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
    else
        root = x;

    if (y != z) z->data = y->data;


    if (y->color == BLACK)
        deleteFixupOID (&root, x);

    free(y);
    *tree = root;
}

/**************************************
 *  Найти ноду (price) по номеру OID  *
 **************************************/
NodeOID *findNodeOID(NodeOID** tree, unsigned int oid) {
    NodeOID *current = *tree;
    while(current != NILOID)
        if(cmpEQOID(oid, current->data.oid))
            return (current);
        else
            current = cmpLTOID (oid, current->data.oid) ?
                current->left : current->right;
    return(0);
}
#endif // NODE_H
