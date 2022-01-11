#include <stdio.h>
#include <stdlib.h>
#include "Node.h"

#define WIDTH 150
#define HALF_WIDTH (WIDTH/2)



void heightTree(Node* tree, int lvl, int* max) {
    if (*max < lvl)
        *max = lvl;
    if (tree->left != NIL) {
        heightTree(tree->left, lvl + 1, max);
    }
    if (tree->right != NIL) {
        heightTree(tree->right, lvl + 1, max);
    }
    return;
}


int main()
{
    FILE *in = fopen("input.txt", "rt");
    if(in == NULL )
    {
        printf("Error openning file!\n");
        exit(EXIT_FAILURE);
    }
    char c_type;
    int uniq_number = 0;
    char c_side;
    int qty = 0;
    float price = 0.;

    int count = 0;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    while((count = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &uniq_number, &c_side, &qty, &price)) != EOF) {
        PriceData *n = NULL;
        n = (PriceData*)malloc(sizeof (PriceData));

        n->price = price;
        if(count == 5) {
            if(c_side == 'B')
                insertNode(&bye_glass, *n);
            else
                insertNode(&sell_glass, *n);

            //printf("type:%c N=%d side:%c qty=%d price=%f\n", c_type, uniq_number, c_side, qty, price);
        } else if(count == 2) {
            //printf("type:%c N=%d\n", c_type, uniq_number);
        }
    }
    int bye_max = 0;
    int sell_max = 0;
    heightTree(bye_glass, 0, &bye_max);
    heightTree(sell_glass, 0, &sell_max);
    PriceData *p = (PriceData*)malloc(sizeof(PriceData));
    p->price = 250.61;
    Node *n = NIL;
    n = findNode(&bye_glass, *p);
    if(n != NULL)
        printf("%f\n",n->data.price);
    printf("Max_bye=%d\tmax_sell=%d \nEnd of file!\n", bye_max, sell_max);
    fclose(in);
    return 0;
}
