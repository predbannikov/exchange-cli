#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
#include "oidstore.h"
#include "hashtable.h"



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

int cancle_order(Node **glass, NodeOID *noid){
    Node* node = *glass;
    PriceData price_data = {noid->data.price};
    Node *nd = findNode(&node, price_data);
    if(nd == NULL) {
        printf("Error: Node not contain price_level=%f for this oid=%d\n", noid->data.price, noid->data.oid);
        fflush(stdout);
        return 0;
    }
    OrderLevel *ol = nd->data.price_level->head;
    if(ol != NULL && ol->value->oid == noid->data.oid) {
        OrderLevel *tmp = ol->next;
        ol->next = ol->next->next;
        free(tmp);
    } else {
        while(ol->next != NULL) {
            if(ol->next->value->oid == noid->data.oid)
                break;
            ol = ol->next;
        }
        if(ol->next == NULL) {
            printf("Error: OrderLevel %f not contain oid=%d for delete\n", noid->data.price, noid->data.oid );
        } else {
            OrderLevel *tmp = ol->next;
            ol->next = ol->next->next;
            free(tmp);
        }
    }
    if(nd->data.price_level->size == 0)
        deleteNode(&node, nd);
    *glass = node;
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
    int uniq_number = 0;
    char c_side;
    int qty = 0;
    float price = 0.;

    unsigned int trade_id = 0;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;
    while(fscanf(in, "%c,%d,%c,%d,%f", &c_type, &uniq_number, &c_side, &qty, &price) != EOF) {
        PriceData *n = NULL;
        n = (PriceData*)malloc(sizeof (PriceData));

        n->price = price;
        n->oid = uniq_number;
        n->qty = qty;
        n->side = c_side;
        if(c_type == 'O') {
            if(c_side == 'B') {
                Node* tmp = sell_glass;
                while(tmp->left != NIL)
                    tmp = tmp->left;
                while(tmp->data.price >= price && tmp != NIL && n->qty != 0) {	// Если есть покупатели то выполняем сделки
                    OrderLevel *orders = tmp->data.price_level->head;	// Получаем первый в очереди ордер на покупку
                    while(orders != NULL) {	// Обходим очередь или выход из цикла по break
                        if(orders->value->qty > n->qty) {
                            // Ордер который прилетел закрыт
                            orders->value->qty -= n->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, n->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                            n->qty = 0;
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        deleteNode(&sell_glass, tmp);
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
                    insertNodeOID(&oidstore, (OID){n->oid, n->side});
                    insertNode(&bye_glass, *n);
                }
            } else {
                // Ордер на продажу S
                Node* tmp = bye_glass;	// самая высокая цена на покупку
                while(tmp->right != NIL)
                    tmp = tmp->right;
                while(tmp->data.price >= price && tmp != NIL && n->qty != 0) {	// Если есть покупатели то выполняем сделки
                    OrderLevel *orders = tmp->data.price_level->head;	// Получаем первый в очереди ордер на покупку
                    while(orders != NULL) {	// Обходим очередь или выход из цикла по break
                        if(orders->value->qty > n->qty) {
                            // Ордер который прилетел закрыт
                            orders->value->qty -= n->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, n->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){n->oid, n->price, n->side});
                            n->qty = 0;
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->oid, n->oid, orders->value->qty, orders->value->price);
                            insertNodeOID(&oidstore, (OID){orders->value->oid, n->price, orders->value->side});
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        deleteNode(&bye_glass, tmp);
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
                    insertNodeOID(&oidstore, (OID){n->oid, n->side});
                    insertNode(&sell_glass, *n);
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
                cancle_order(&bye_glass, noid);
            } else {
                cancle_order(&sell_glass, noid);
            }
            deleteNodeOID(&oidstore, noid);
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
    printf("\nMax_bye=%d\tmax_sell=%d \nEnd of file!\n", bye_max, sell_max);
    fclose(in);

    return 0;
}
