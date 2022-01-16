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
int SumTreeRecurs(NodeOID* tree) {
    if (tree == NILOID)
        return 0;
    return SumTreeRecurs(tree->left) + SumTreeRecurs(tree->right) + 1;
}

void printCountOID(NodeOID* tree) {
    int sum = SumTreeRecurs(tree);
    printf("count=%d\n", sum);
    fflush(stdout);
}

int cancle_order(Node **glass, NodeOID *noid){
    static int counter = 0;
    Node* node = *glass;
    travers_chack_head_tail_eq(node);
    //travers_chack_node(node);
    PriceData price_data = {noid->data.price};
    Node *node_to_cncl = findNode(&node, price_data);
    if(node_to_cncl == NULL) {
        printf("Error: Node not contain price_level=%f for this oid=%d\n", noid->data.price, noid->data.oid);
        fflush(stdout);
        return 0;
    }
    //travers_chack_node(node);
    //deb_check_node(node, price_data);

    //travers_chack_node(node);

    //PriceLevel pclvl = *node_to_cncl->data.price_level;
    //delete_oid(&pclvl, noid->data.oid);
    if(counter == 104)
        printf("stop");
    if(counter == 280)
        printf("stop");
    travers_chack_head_tail_eq(node);
    delete_oid(node_to_cncl->data.price_level, noid->data.oid);

    //travers_chack_node(node);

    if(node_to_cncl->data.price_level->size == 0) {
        deleteNode(&node, node_to_cncl);
        deb_check_node(node, price_data);
    }
    fflush(stdout);
    travers_chack_node(node);
    *glass = node;
    counter++;
    travers_chack_head_tail_eq(node);
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

    unsigned int trade_id = 1;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;


    int counter = 0;
    int counter_args = 0;
    while((counter_args = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &oid, &c_side, &qty, &price)) != EOF) {
        if(counter_args == 1)
            continue;
        test(bye_glass, sell_glass);
//        if(counter == 22)
//            break;
        PriceData *n = NULL;
        n = (PriceData*)malloc(sizeof (PriceData));

        n->price = price;
        n->oid = oid;
        n->qty = qty;
        n->side = c_side;
        if(counter == 4189)
            printf("stop");
        if(c_type == 'O') {
            if(n->side == 'B') {
                Node* tmp = sell_glass;
                while(tmp->left != NIL)
                    tmp = tmp->left;
                if(counter == 291)
                    printf("stop");
                while(tmp->data.price <= n->price && tmp != NIL && n->qty != 0) {	// Если есть покупатели то выполняем сделки
                    OrderLevel *orders = tmp->data.price_level->head;			// Получаем первый в очереди ордер на покупку
                    while(orders != NULL) {				// Обходим очередь или выход из цикла по break
                        if(orders->value->qty > n->qty) {
                            // Ордер который прилетел закрыт
                            orders->value->qty -= n->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, n->qty, orders->value->price);
                            n->qty = 0;
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            //if(counter == 14)
                            //    printf("stop\n");
                            //PriceData pcdatatest = tmp->data;
                            pop_front(tmp->data.price_level);
                            //deb_check_node(sell_glass, pcdatatest);
                           orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            //PriceData pcdatatest = tmp->data;
                            pop_front(tmp->data.price_level);
                            //deb_check_node(sell_glass, pcdatatest);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        PriceData pcdatatest = tmp->data;
                        deleteNode(&sell_glass, tmp);
                        deb_check_node(sell_glass, pcdatatest);
                    }
                    if(n->qty != 0) {
                        tmp = sell_glass;
                        while(tmp->left != NIL)
                            tmp = tmp->left;
                        if(tmp == NIL)
                            break;
                    }
                }
                if(n->qty != 0) {
                    insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                    insertNode(&bye_glass, *n);
                    deb_check_node(bye_glass, *n);
                }
            } else {
                // Ордер на продажу S
                Node* tmp = bye_glass;	// самая высокая цена на покупку
                while(tmp->right != NIL)
                    tmp = tmp->right;
                while(tmp->data.price >= n->price && tmp != NIL && n->qty != 0) {	// Если есть покупатели то выполняем сделки
                    OrderLevel *orders = tmp->data.price_level->head;	// Получаем первый в очереди ордер на покупку
                    while(orders != NULL) {	// Обходим очередь или выход из цикла по break
                        if(orders->value->qty > n->qty) {
                            // Ордер который прилетел закрыт
                            orders->value->qty -= n->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, n->qty, orders->value->price);
                            n->qty = 0;
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
//                            PriceData pcdatatest = tmp->data;
                            pop_front(tmp->data.price_level);
//                            deb_check_node(bye_glass, pcdatatest);
                            orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid + 2, n->price, orders->value->side});
//                            PriceData pcdatatest = tmp->data;
                            pop_front(tmp->data.price_level);
//                            deb_check_node(bye_glass, pcdatatest);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        PriceData pcdatatest = tmp->data;
                        deleteNode(&bye_glass, tmp);
                        deb_check_node(bye_glass, pcdatatest);
                    }
                    if(n->qty != 0) {
                        tmp = bye_glass;
                        while(tmp->right != NIL)
                            tmp = tmp->right;
                        if(tmp == NIL)
                            break;
                    }
                }
                if(n->qty != 0) {
                    insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                    insertNode(&sell_glass, *n);
                    deb_check_node(sell_glass, *n);
                }
            }

        } else if(c_type == 'C') {
            NodeOID *noid = findNodeOID(&oidstore, (OID){n->oid});
            if(noid == NILOID || noid == NULL) {
                printf("not found oid\n");
                continue;
            } else
                printf("X,%d\n", noid->data.oid);

            fflush(stdout);
            if(noid->data.side == 'B') {
                travers_chack_node(bye_glass);
                cancle_order(&bye_glass, noid);
                //if(counter == 4189)
                travers_chack_node(bye_glass);
            } else {
                cancle_order(&sell_glass, noid);
                if(counter == 4189)
                    travers_chack_node(sell_glass);
            }
            //printCountOID(oidstore);
            deleteNodeOID(&oidstore, noid);
            //printCountOID(oidstore);
        }
        fflush(stdout);
        test(bye_glass, sell_glass);
        counter++;
        //n = (PriceData*)malloc(sizeof (PriceData));
    }

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
        printf("%f\n",ntest->data.price);
    printf("\nMax_bye=%d\tmax_sell=%d \nEnd of file!\n", bye_max, sell_max);
    printf("max_height_oid=%d\n", oid_max);
    fclose(in);

    return 0;
}
