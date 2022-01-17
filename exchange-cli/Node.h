#ifndef NODE_H
#define NODE_H
/* red-black tree */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "PriceLevel.h"


#define EPS 0.0001

/* Red-Black tree description */
typedef enum { BLACK, RED } nodeColor;

typedef struct Node_ {
    struct Node_ *left;         /* left child */
    struct Node_ *right;        /* right child */
    struct Node_ *parent;       /* parent */
    nodeColor color;            /* node color (BLACK, RED) */
    //PriceData data;             /* data stored in node */
    PriceLevel *price_level;
} Node;

int cmpLT(float a, float b) {
    return a < b;
}

int cmpEQ(float a, float b) {
    return fabsf(a - b) <= EPS;
}

#define NIL &sentinel           /* all leafs are sentinels */
Node sentinel = { NIL, NIL, 0, BLACK, 0};

/**************************
 *  rotate node x to left *
 **************************/
void rotateLeft(Node** glass_tree, Node *x) {

    Node* root = *glass_tree;

    Node *y = x->right;

    /* establish x->right link */
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    } else {
        root = y;
    }

    /* link x and y */
    y->left = x;
    if (x != NIL) x->parent = y;
    *glass_tree = root;
}

/****************************
 *  rotate node x to right  *
 ****************************/
void rotateRight(Node** glass_tree, Node *x) {

    Node* root = *glass_tree;

    Node *y = x->left;

    /* establish x->left link */
    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;

    /* establish y->parent link */
    if (y != NIL) y->parent = x->parent;
    if (x->parent) {
        if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
    } else {
        root = y;
    }

    /* link x and y */
    y->right = x;
    if (x != NIL) x->parent = y;
    *glass_tree = root;
}

/*************************************
 *  maintain Red-Black tree balance  *
 *  after inserting node x           *
 *************************************/
void insertFixup(Node** glass_tree, Node *x) {

    Node* root = *glass_tree;

    /* check Red-Black properties */
    while (x != root && x->parent->color == RED) {
        /* we have a violation */
        if (x->parent == x->parent->parent->left) {
            Node *y = x->parent->parent->right;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                /* uncle is BLACK */
                if (x == x->parent->right) {
                    /* make x a left child */
                    x = x->parent;
                    rotateLeft(&root, x);
                }

                /* recolor and rotate */
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRight(&root, x->parent->parent);
            }
        } else {

            /* mirror image of above code */
            Node *y = x->parent->parent->left;
            if (y->color == RED) {

                /* uncle is RED */
                x->parent->color = BLACK;
                y->color = BLACK;
                x->parent->parent->color = RED;
                x = x->parent->parent;
            } else {

                /* uncle is BLACK */
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

/***********************************************
 *  allocate node for data and insert in tree  *
 ***********************************************/
Node *insertNode(Node** glass_tree, Order *ord) {
    static int counter = 0;
    counter++;
    Node* current, *parent, *x;
    Node* root = *glass_tree;

    /* find where node belongs */
    current = root;
    parent = 0;
    while (current != NIL) {
        if (cmpEQ(ord->price, current->price_level->head->value->price)) {

            push_back(current->price_level, ord);
            return (current);
        }
        parent = current;
        current = cmpLT(ord->price, current->price_level->head->value->price) ?
            current->left : current->right;
    }

    /* setup new node */
    if ((x = malloc (sizeof(Node))) == 0) {
        printf ("insufficient memory (insertNode)\n");
        exit(1);
    }

    x->parent = parent;
    x->left = NIL;
    x->right = NIL;
    x->color = RED;
    x->price_level = createLinkedList();
    push_back(x->price_level, ord);

    /* insert node in tree */
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

/*************************************
 *  maintain Red-Black tree balance  *
 *  after deleting node x            *
 *************************************/
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


/*****************************
 *  delete node z from tree  *
 *****************************/
void deleteNode(Node** glass_tree, Node *z) {
    Node *x, *y;
    Node* root = *glass_tree;
    free(z->price_level);

    if (!z || z == NIL) return;


    if (z->left == NIL || z->right == NIL) {
        /* y has a NIL node as a child */
        y = z;
    } else {
        /* find tree successor with a NIL node as a child */
        y = z->right;
        while (y->left != NIL) y = y->left;
    }

    /* x is y's only child */
    if (y->left != NIL)
        x = y->left;
    else
        x = y->right;

    /* remove y from the parent chain */
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

/*******************************
 *  find node containing data  *
 *******************************/
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
#endif // NODE_H
