#ifndef OIDSTORE_H
#define OIDSTORE_H

#include "Node.h"

typedef struct OID_ {
    unsigned oid;
    float price;
    char side;
} OID;

//typedef enum { BLACK, RED } nodeColor;

typedef struct NodeOID_ {
    struct NodeOID_ *left;         /* left child */
    struct NodeOID_ *right;        /* right child */
    struct NodeOID_ *parent;       /* parent */
    nodeColor color;               /* node color (BLACK, RED) */
    OID data;                      /* data stored in node */
} NodeOID;

int cmpLTOID(OID a, OID b) {
    return a.oid < b.oid;
}

int cmpEQOID(OID a, OID b) {
    return a.oid == b.oid;
}


#define NILOID &sentineloid           /* all leafs are sentinels */
NodeOID sentineloid = { NILOID, NILOID, 0, BLACK, 0};

/**************************
 *  rotate node x to left *
 **************************/
void rotateLeftOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;

    NodeOID *y = x->right;

    /* establish x->right link */
    x->right = y->left;
    if (y->left != NILOID) y->left->parent = x;

    /* establish y->parent link */
    if (y != NILOID) y->parent = x->parent;
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
    if (x != NILOID) x->parent = y;
    *tree = root;
}

/****************************
 *  rotate node x to right  *
 ****************************/
void rotateRightOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;

    NodeOID *y = x->left;

    /* establish x->left link */
    x->left = y->right;
    if (y->right != NILOID) y->right->parent = x;

    /* establish y->parent link */
    if (y != NILOID) y->parent = x->parent;
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
    if (x != NILOID) x->parent = y;
    *tree = root;
}

/*************************************
 *  maintain Red-Black tree balance  *
 *  after inserting node x           *
 *************************************/
void insertFixupOID(NodeOID** tree, NodeOID *x) {

    NodeOID* root = *tree;


    /* check Red-Black properties */
    while (x != root && x->parent->color == RED) {
        /* we have a violation */
        if (x->parent == x->parent->parent->left) {
            NodeOID *y = x->parent->parent->right;
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
                    rotateLeftOID(&root, x);
                }

                /* recolor and rotate */
                x->parent->color = BLACK;
                x->parent->parent->color = RED;
                rotateRightOID(&root, x->parent->parent);
            }
        } else {

            /* mirror image of above code */
            NodeOID *y = x->parent->parent->left;
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

/***********************************************
 *  allocate node for data and insert in tree  *
 ***********************************************/
NodeOID *insertNodeOID(NodeOID** tree, OID data) {
    NodeOID* current, *parent, *x;
    NodeOID* root = *tree;

    /* find where node belongs */
    current = root;
    parent = 0;
    while (current != NILOID) {
        if (cmpEQOID(data, current->data)) {
            return (current);
        }
        parent = current;
        current = cmpLTOID(data, current->data) ?
            current->left : current->right;
    }

    /* setup new node */
    if ((x = (NodeOID*) malloc (sizeof(*x))) == 0) {
        printf ("insufficient memory (insertNodeOID)\n");
        exit(1);
    }

    x->data = data;
    x->parent = parent;
    x->left = NILOID;
    x->right = NILOID;
    x->color = RED;

    /* insert node in tree */
    if(parent) {
        if(cmpLTOID(data, parent->data))
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

/*************************************
 *  maintain Red-Black tree balance  *
 *  after deleting node x            *
 *************************************/
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


/*****************************
 *  delete node z from tree  *
 *****************************/
void deleteNodeOID(NodeOID** tree, NodeOID *z) {
    NodeOID *x, *y;
    NodeOID* root = *tree;

    if (!z || z == NILOID) return;


    if (z->left == NILOID || z->right == NILOID) {
        /* y has a NILOID node as a child */
        y = z;
    } else {
        /* find tree successor with a NILOID node as a child */
        y = z->right;
        while (y->left != NILOID) y = y->left;
    }

    /* x is y's only child */
    if (y->left != NILOID)
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

    if (y != z) z->data = y->data;


    if (y->color == BLACK)
        deleteFixupOID (&root, x);

    free (y);
    *tree = root;
}

/*******************************
 *  find node containing data  *
 *******************************/
NodeOID *findNodeOID(NodeOID** tree, OID data) {
    NodeOID *current = *tree;
    while(current != NILOID)
        if(cmpEQOID(data, current->data))
            return (current);
        else
            current = cmpLTOID (data, current->data) ?
                current->left : current->right;
    return(0);
}
#endif // OIDSTORE_H
