#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
#include "oidstore.h"
#include "hashtable.h"


void heightTreeOid(NodeOID* tree, int lvl, int* max) {
    if (*max < lvl)
        *max = lvl;
    if (tree->left != NILOID) {
        heightTreeOid(tree->left, lvl + 1, max);
    }
    if (tree->right != NILOID) {
        heightTreeOid(tree->right, lvl + 1, max);
    }
    return;
}


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

void deb_check_node(Node* tree, PriceData price_data) {
    Node *test = findNode(&tree, price_data);
    if(test != NULL) {
        if(test->data.price_level->head == NULL) {
            printf("stop\n");
            fflush(stdout);
        }
    }
}

void travers_chack_node(Node* tree) {
    if(tree == NIL)
        return;
    if(tree->data.price_level->size >= 0 && tree->data.price_level->head == NULL) {
        printf("stop\n");
        fflush(stdout);
    }
    travers_chack_node(tree->left);
    travers_chack_node(tree->right);
    return;
}

void travers_chack_head_tail_eq(Node* tree) {
    if(tree == NIL)
        return;
    if(tree->data.price_level->size > 1 && tree->data.price_level->head == tree->data.price_level->tail) {
        printf("stop\n");
        fflush(stdout);
    }
    travers_chack_head_tail_eq(tree->left);
    travers_chack_head_tail_eq(tree->right);
    return;
}

void test(Node* t1, Node *t2) {
    travers_chack_node(t1);
    travers_chack_node(t2);
    travers_chack_head_tail_eq(t1);
    travers_chack_head_tail_eq(t2);
}
int SumTreeRecursOID(NodeOID* tree) {
    if (tree == NILOID)
        return 0;
    return SumTreeRecursOID(tree->left) + SumTreeRecursOID(tree->right) + 1;
}

int SumLinkList(PriceLevel *price_level) {
    OrderLevel *lvl = price_level->head;
    int sum = 1;
    while(lvl != price_level->tail) {
        sum++;
        lvl = lvl->next;
    }
    return sum;
}

int SumTreeRecurs(Node* tree) {
    if (tree == NIL)
        return 0;
    int value = SumLinkList(tree->data.price_level);
    int left = SumTreeRecurs(tree->left);
    int right = SumTreeRecurs(tree->right);
    return left + right + value;
}
void printCountOID(NodeOID* tree) {
    int sum = SumTreeRecurs(tree);
    printf("count=%d\n", sum);
    fflush(stdout);
}

int cancle_order(Node **glass, NodeOID *noid){
    static int counter = 0;
    Node* node = *glass;
    PriceData price_data = {noid->data.price};
    Node *node_to_cncl = findNode(&node, price_data);
    if(node_to_cncl == NULL) {
        return 0;
    }
    delete_oid(node_to_cncl->data.price_level, noid->data.oid);
    if(node_to_cncl->data.price_level->size == 0) {
        deleteNode(&node, node_to_cncl);
        deb_check_node(node, price_data);
    }
    fflush(stdout);
    *glass = node;
    counter++;
    return 0;
}

Node* extr_elem(Node* tree, char side) {
    Node *tmp = tree;
    if(side == 'B') {
        while(tmp->left != NIL)
            tmp = tmp->left;
    } else {
        while(tmp->right != NIL)
            tmp = tmp->right;
    }
    return tmp;
}

unsigned int trade_id = 1;

int matching(Node** glass, PriceData *n, NodeOID **oidstr) {
    Node* tmp = extr_elem(*glass, n->side);
    char side = n->side == 'B' ? 'S' : 'B';

    while(tmp != NIL && n->qty != 0) {	// Если есть покупатели то выполняем сделки
        if(n->side == 'B') {
            if(tmp->data.price > n->price)
                break;
        } else {
            if(tmp->data.price < n->price)
                break;
        }
        OrderLevel *orders = tmp->data.price_level->head;			// Получаем первый в очереди ордер на покупку
        while(orders != NULL) {				// Обходим очередь или выход из цикла по break
            if(orders->value->qty > n->qty) {
                orders->value->qty -= n->qty;
                printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, side, orders->value->oid, n->oid, n->qty, orders->value->price);
                n->qty = 0;
                break;
            } else if(orders->value->qty < n->qty) {
                n->qty -= orders->value->qty;
                printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, side, orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                NodeOID *oid = findNodeOID(oidstr,(OID){orders->value->oid});
                deleteNodeOID(oidstr, oid);
                pop_front(tmp->data.price_level);
                orders = tmp->data.price_level->head;
            } else {
                printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, side, orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                NodeOID *oid = findNodeOID(oidstr,(OID){orders->value->oid});
                deleteNodeOID(oidstr, oid);
                pop_front(tmp->data.price_level);
                orders = tmp->data.price_level->head;
                n->qty = 0;
            }
        }
        if(tmp->data.price_level->size == 0)
            deleteNode(glass, tmp);
        if(n->qty != 0)
            tmp = extr_elem(*glass, n->side);
    }
    return 0;
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
    int oid = 0;
    char c_side;
    int qty = 0;
    float price = 0.;

    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;

    int counter = 0;
    int counter_args = 0;
    while((counter_args = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &oid, &c_side, &qty, &price)) != EOF) {
        if(counter_args == 1)
            continue;
        PriceData *n = NULL;
        n = (PriceData*)malloc(sizeof (PriceData));
        n->price = price;
        n->oid = oid;
        n->qty = qty;
        n->side = c_side;
        if(c_type == 'O') {
            if(n->side == 'B') {
                matching(&sell_glass, n, &oidstore);
                if(n->qty != 0) {
                    insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                    insertNode(&bye_glass, *n);
                }
            } else {
                matching(&bye_glass, n, &oidstore);
                if(n->qty != 0) {
                    insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                    insertNode(&sell_glass, *n);
                }
            }
        } else if(c_type == 'C') {
            NodeOID *noid = findNodeOID(&oidstore, (OID){n->oid});
            if(noid == NILOID || noid == NULL) {
                //printf("not found oid\n");
                continue;
            } else
                printf("X,%d\n", noid->data.oid);

            if(noid->data.side == 'B') {
                cancle_order(&bye_glass, noid);
            } else {
                cancle_order(&sell_glass, noid);
            }
            deleteNodeOID(&oidstore, noid);
        }
        counter++;
    }

    printf("\n");
    printf("sum orders oid=%d\n", SumTreeRecursOID(oidstore));
    printf("sum orders bye=%d\n", SumTreeRecurs(bye_glass));
    printf("sum orders sell=%d\n", SumTreeRecurs(sell_glass));

    int bye_max = 0;
    int sell_max = 0;
    int oid_max = 0;
    heightTree(bye_glass, 0, &bye_max);
    heightTree(sell_glass, 0, &sell_max);
    heightTreeOid(oidstore, 0, &oid_max);
    PriceData *p = (PriceData*)malloc(sizeof(PriceData));
    p->price = 250.61;
    Node *ntest = NIL;
    ntest = findNode(&bye_glass, *p);
    if(ntest != NULL)
        printf("found: %f\n",ntest->data.price);
    printf("\nMax_bye=%d\tmax_sell=%d \nEnd of file!\n", bye_max, sell_max);
    printf("max_height_oid=%d\n", oid_max);
    fclose(in);

    return 0;
}
