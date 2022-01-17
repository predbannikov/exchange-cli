#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Node.h"
#include "oidstore.h"
//#include "hashtable.h"



void free_nodes(Node* tree) {
    while(tree != NIL) {
        PriceLevel* price_level = tree->price_level;
        OrderLevel* orders = price_level->head;
        while(orders != NULL) {
            OrderLevel *tmp = orders;
            orders = orders->next;
            free(tmp->value);
            free(tmp);
        }
        deleteNode(&tree, tree);
    }
}

void free_oidstore(NodeOID* tree) {
    while(tree != NILOID)
        deleteNodeOID(&tree, tree);
}

//void heightTreeOid(NodeOID* tree, int lvl, int* max) {
//    if (*max < lvl)
//        *max = lvl;
//    if (tree->left != NILOID) {
//        heightTreeOid(tree->left, lvl + 1, max);
//    }
//    if (tree->right != NILOID) {
//        heightTreeOid(tree->right, lvl + 1, max);
//    }
//    return;
//}

//void heightTree(Node* tree, int lvl, int* max) {
//    if (*max < lvl)
//        *max = lvl;
//    if (tree->left != NIL) {
//        heightTree(tree->left, lvl + 1, max);
//    }
//    if (tree->right != NIL) {
//        heightTree(tree->right, lvl + 1, max);
//    }
//    return;
//}

//void deb_check_node(Node* tree, PriceData price_data) {
//    Node *test = findNode(&tree, &price_data);
//    if(test != NULL) {
//        if(test->data->price_level->head == NULL) {
//            printf("stop\n");
//            fflush(stdout);
//        }
//    }
//}

//void travers_chack_node(Node* tree) {
//    if(tree == NIL)
//        return;
//    if(tree->data->price_level->size >= 0 && tree->data->price_level->head == NULL) {
//        printf("stop\n");
//        fflush(stdout);
//    }
//    travers_chack_node(tree->left);
//    travers_chack_node(tree->right);
//    return;
//}

//void travers_chack_head_tail_eq(Node* tree) {
//    if(tree == NIL)
//        return;
//    if(tree->data->price_level->size > 1 && tree->data->price_level->head == tree->data->price_level->tail) {
//        printf("stop\n");
//        fflush(stdout);
//    }
//    travers_chack_head_tail_eq(tree->left);
//    travers_chack_head_tail_eq(tree->right);
//    return;
//}

//void test(Node* t1, Node *t2) {
//    travers_chack_node(t1);
//    travers_chack_node(t2);
//    travers_chack_head_tail_eq(t1);
//    travers_chack_head_tail_eq(t2);
//}

//int SumTreeRecursOID(NodeOID* tree) {
//    if (tree == NILOID)
//        return 0;
//    return SumTreeRecursOID(tree->left) + SumTreeRecursOID(tree->right) + 1;
//}

//int SumLinkList(PriceLevel *price_level) {
//    OrderLevel *lvl = price_level->head;
//    int sum = 1;
//    while(lvl != price_level->tail) {
//        sum++;
//        lvl = lvl->next;
//    }
//    return sum;
//}

//int SumTreeRecurs(Node* tree) {
//    if (tree == NIL)
//        return 0;
//    int value = SumLinkList(tree->data->price_level);
//    int left = SumTreeRecurs(tree->left);
//    int right = SumTreeRecurs(tree->right);
//    return left + right + value;
//}

//void printCountOID(NodeOID* tree) {
//    int sum = SumTreeRecurs(tree);
//    printf("count=%d\n", sum);
//    fflush(stdout);
//}

int cancle_order(Node **glass, NodeOID *noid){
    static int counter = 0;
    Node* node = *glass;
    Node *node_to_cncl = findNode(&node, noid->data.price);
    if(node_to_cncl == NULL) {
        return 0;
    }
    delete_Nth_of_oid(node_to_cncl->price_level, noid->data.oid);
    if(node_to_cncl->price_level->size == 0) {
        deleteNode(&node, node_to_cncl);
    }
    //fflush(stdout);
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

float macheps(void)
{
    float e = 1.0f;
    while (1.0f + e / 2.0f > 1.0f)
        e /= 2.0f;
    return e;
}

//void morphNumericString (char *s, int n) {
//    char *p;
//    int count;

//    p = strchr (s,'.');         // Find decimal point, if any.
//    if (p != NULL) {
//        count = n;              // Adjust for more or less decimals.
//        while (count >= 0) {    // Maximum decimals allowed.
//             count--;
//             if (*p == '\0')    // If there's less than desired.
//                 break;
//             p++;               // Next character.
//        }

//        *p-- = '\0';            // Truncate string.
//        while (*p == '0')       // Remove trailing zeros.
//            *p-- = '\0';

//        if (*p == '.') {        // If all decimals were zeros, remove ".".
//            *p = '\0';
//        }
//    }
//}

//char fdgt_str[7];	//
int left_dgt;
int right_dgt;

void matching(Node** glass, Order *order, NodeOID **oidstr) {
    Node* tmp = extr_elem(*glass, order->side);
    char side = order->side == 'B' ? 'S' : 'B';

    while(tmp != NIL && order->qty != 0) {	// Если есть покупатели то выполняем сделки
        if(order->side == 'B') {
            if(tmp->price_level->head->value->price > order->price)
                break;
        } else {
            if(tmp->price_level->head->value->price < order->price)
                break;
        }
        OrderLevel *orders = tmp->price_level->head;			// Получаем первый в очереди ордер на покупку
        while(orders != NULL) {				// Обходим очередь или выход из цикла по break
            if(orders->value->qty > order->qty) {
                orders->value->qty -= order->qty;
//                sprintf(fdgt_str,"%.2f", orders->value->price);
//                morphNumericString(fdgt_str, 2);

                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt %10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, order->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, order->qty, left_dgt, right_dgt);


                order->qty = 0;
                break;
            } else if(orders->value->qty < order->qty) {
                order->qty -= orders->value->qty;
//                sprintf(fdgt_str,"%.2f", orders->value->price);
//                morphNumericString(fdgt_str, 2);
                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);
                NodeOID *oid = findNodeOID(oidstr,(OID){orders->value->oid, 0, 0});
                deleteNodeOID(oidstr, oid);
                pop_front(tmp->price_level);
                orders = tmp->price_level->head;
            } else {
//                sprintf(fdgt_str,"%.2f", orders->value->price);
//                morphNumericString(fdgt_str, 2);
                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);
//               printf("T,%u,%c,%d,%d,%d,%s\n", trade_id++, side, orders->value->oid, n->oid, orders->value->qty, fdgt_str);
                NodeOID *oid = findNodeOID(oidstr,(OID){orders->value->oid, 0, 0});
                deleteNodeOID(oidstr, oid);
                pop_front(tmp->price_level);
                orders = tmp->price_level->head;
                order->qty = 0;
            }
        }
        if(tmp->price_level->size == 0)
            deleteNode(glass, tmp);
        if(order->qty != 0)
            tmp = extr_elem(*glass, order->side);
    }
}

void exchange(FILE *in) {
    char c_type;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;
    Order *order = NULL;
    order = (Order*)malloc(sizeof (Order));
    int counter = 0;
    int counter_args = 0;
    while((counter_args = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &order->oid, &order->side, &order->qty, &order->price)) != EOF) {
        if(counter_args == 1)
            continue;
        if(c_type == 'O') {
            if(order->side == 'B') {
                matching(&sell_glass, order, &oidstore);
                if(order->qty != 0) {
                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
                    insertNode(&bye_glass, order);
                }
            } else {
                matching(&bye_glass, order, &oidstore);
                if(order->qty != 0) {
                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
                    insertNode(&sell_glass, order);
                }
            }
        } else if(c_type == 'C') {
            NodeOID *noid = findNodeOID(&oidstore, (OID){order->oid, 0, 0});
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
        order = (Order*)malloc(sizeof (Order));
    }
    free(order);
    free_nodes(bye_glass);
    free_nodes(sell_glass);
    free_oidstore(oidstore);
}

int main(int argc, char **argv)
{
    FILE *in;
    if(argc > 1 && argc < 3) {
        in = fopen(argv[1], "rt");
        if(in == NULL )
        {
            printf("Error openning file <%s>.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Missing argument <input_file>.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < 10; i++) {
        fseek(in, 0, SEEK_SET);
        exchange(in);
    }
    fclose(in);
    return EXIT_SUCCESS;
}
