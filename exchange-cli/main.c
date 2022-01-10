#include <stdio.h>
#include <stdlib.h>
#include "Node.h"

#define WIDTH 150
#define HALF_WIDTH (WIDTH/2)

//void printTree(Node *tree, int maxLvl);

/*void traversalTree(std::vector<std::string>& v, Node* tree, int lvl, int i1, int maxLvl = 0) {

    if (maxLvl != 0 && lvl >= maxLvl) {
        return;
    }
    std::string val = std::to_string(tree->value);
    int valSize = val.length();
    int curPos = i1;
    if (i1 - (valSize / 2) >= 0)
        curPos = i1 - valSize / 2;
    for (size_t i = curPos, j = 0; i < curPos + val.size(); i++, j++)
        v[lvl * 2][i] = val[j];
    int diff;
    if (tree->left != nullptr) {
        diff = HALF_WIDTH / pow(2, lvl);
        v[(lvl * 2) + 1][curPos + (valSize / 2) - diff / 2] = '/';
        traversalTree(v, tree->left, lvl + 1, i1 - diff, maxLvl);

    }
    if (tree->right != nullptr) {
        diff = HALF_WIDTH / pow(2, lvl);
        v[(lvl * 2) + 1][curPos + (valSize / 2) + diff / 2] = '\\';
        traversalTree(v, tree->right, lvl + 1, i1 + diff, maxLvl);
    }
    return;
}
*/

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

/*void printTree(Node *tree, int maxLvl) {
    if (tree == NULL) {
        printf("");
        //std::cout << std::string(HALF_WIDTH, ' ') << "nill" << std::endl;
        return;
    }
    int height = maxLvl;
    if (height == 0)
        heightTree(tree, 0, &height);
    std::vector<std::string> pic((height + 1) * 2 + 1, std::string(WIDTH, ' '));
    traversalTree(pic, tree, 1, HALF_WIDTH, maxLvl);
    for (size_t i = 0; i < pic.size(); i++) {
        printf("");
        //std::cout << pic[i] << std::endl;
    }
}
*/

int main()
{
    Node *node = NULL;
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
    while((count = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &uniq_number, &c_side, &qty, &price)) != EOF) {
        Node *n;

        if(count == 5) {
            printf("type:%c N=%d side:%c qty=%d price=%f\n", c_type, uniq_number, c_side, qty, price);
            insertNode(price);
        } else if(count == 2) {
            printf("type:%c N=%d\n", c_type, uniq_number);
        }
    }
    int max = 0;
    heightTree(root, 0, &max);
    printf("Max=%d\nEnd of file!\n", max);
    fclose(in);
    return 0;
}
