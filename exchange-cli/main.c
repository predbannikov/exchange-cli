#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
#include "oidstore.h"


unsigned int trade_id = 1;
int left_dgt;
int right_dgt;

enum {STATE_START, STATE_OID, STATE_SIDE, STATE_QTY, STATE_PRICE, STATE_PRICE2, STATE_END} state_parser = STATE_START;
enum {STATE_ORDER, STATE_CANCLE} state_ticket = STATE_ORDER;
char shift_dgt = '0';

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

int cancle_order(Node **glass, NodeOID *noid){
    Node* node = *glass;
    Node *node_to_cncl = findNode(&node, noid->data.price);
    if(node_to_cncl == NULL) {
        return 0;
    }

    delete_Nth_of_oid(node_to_cncl->price_level, noid->data.oid);

    if(node_to_cncl->price_level->size == 0)
        deleteNode(&node, node_to_cncl);
    *glass = node;
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

void matching(Node** glass, Order *order, NodeOID **oidstr) {

    Node* border_elem = extr_elem(*glass, order->side);
    char side = order->side == 'B' ? 'S' : 'B';

    while(border_elem != NIL && order->qty != 0) {	// Если есть покупатели то выполняем сделки
        if(order->side == 'B') {
            if(border_elem->price_level->head->value->price > order->price)
                break;
        } else {
            if(border_elem->price_level->head->value->price < order->price)
                break;
        }
        OrderLevel *orders = border_elem->price_level->head;			// Получаем первый в очереди ордер на покупку
        while(orders != NULL) {				// Обходим очередь или выход из цикла по break
            if(orders->value->qty > order->qty) {
                orders->value->qty -= order->qty;

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

                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);

                NodeOID *oid = findNodeOID(oidstr, orders->value->oid);
                deleteNodeOID(oidstr, oid);

                pop_front(border_elem->price_level);
                orders = border_elem->price_level->head;
            } else {

                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);

                NodeOID *oid = findNodeOID(oidstr, orders->value->oid);
                deleteNodeOID(oidstr, oid);
                pop_front(border_elem->price_level);
                orders = border_elem->price_level->head;
                order->qty = 0;
            }
        }
        if(border_elem->price_level->size == 0)
            deleteNode(glass, border_elem);
        if(order->qty != 0)
            border_elem = extr_elem(*glass, order->side);
    }
}

int get_args(unsigned int *oid, char *side, unsigned int *qty, float *price, FILE *in) {
    *oid = 0;
    *qty = 0;
    int pc1 = 0, pc2 = 0;
    char ch;
    int cnt_right_dgt = 0;
    switch (state_ticket) {
    case STATE_ORDER:
        while((ch = getc(in)) != EOF && ch != '\n') {
            if(ch == ',') {
                state_parser++;
                continue;
            }
            switch (state_parser) {
            case STATE_START:
                break;
            case STATE_OID:
                *oid *= 10;
                *oid += (ch - shift_dgt);
                break;
            case STATE_SIDE:
                *side = ch;
                break;
            case STATE_QTY:
                *qty *= 10;
                *qty += (ch - shift_dgt);
                break;
            case STATE_PRICE:
                if(ch=='.') {
                    state_parser = STATE_PRICE2;
                    break;
                }
                pc1 *= 10;
                pc1 += (ch - shift_dgt);
                break;
            case STATE_PRICE2:
                if(ch == '\n') {
                    state_parser = STATE_END;
                    break;
                }
                cnt_right_dgt++;
                pc2 *= 10;
                pc2 += (ch - shift_dgt);
                break;
            case STATE_END:
                break;
            }
        }
        state_parser = STATE_START;
        break;
    case STATE_CANCLE:
        while((ch = getc(in)) != EOF && ch != '\n') {
            if(ch == ',')
                continue;
            *oid *= 10;
            *oid += (ch - shift_dgt);
        }
        break;
    }
    if(cnt_right_dgt == 1)
        *price = pc1 + pc2/10.;
    else
        *price = pc1 + pc2/100.;


    return 0;
}

void exchange(FILE *in) {
    char c_type;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;
    Order *order = NULL;
    order = (Order*)malloc(sizeof (Order));
    int counter_args = 0;

    char ch;

    while((ch = getc(in)) != EOF) {
        if(ch == 'O')
            state_ticket = STATE_ORDER;
        else
            state_ticket = STATE_CANCLE;
        get_args(&order->oid, &order->side, &order->qty, &order->price, in);

        if(counter_args == 1)
            continue;
        if(state_ticket == STATE_ORDER) {
            if(order->side == 'B') {
                matching(&sell_glass, order, &oidstore);
                if(order->qty != 0) {
                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
                    insertNode(&bye_glass, order);
                } else {
                    free(order);
                }
            } else {
                matching(&bye_glass, order, &oidstore);
                if(order->qty != 0) {
                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
                    insertNode(&sell_glass, order);
                } else {
                    free(order);
                }
            }
            order = (Order*)malloc(sizeof (Order));
        } else if(state_ticket == STATE_CANCLE) {
            NodeOID *noid = findNodeOID(&oidstore, order->oid);
            if(noid == NULL)
                continue;
            printf("X,%d\n", noid->data.oid);

            if(noid->data.side == 'B')
                cancle_order(&bye_glass, noid);
            else
                cancle_order(&sell_glass, noid);

            deleteNodeOID(&oidstore, noid);
        }
    }

//    while((counter_args = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &order->oid, &order->side, &order->qty, &order->price)) != EOF) {
//        if(counter_args == 1)
//            continue;
//        if(c_type == 'O') {
//            if(order->side == 'B') {
//                matching(&sell_glass, order, &oidstore);
//                if(order->qty != 0) {
//                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
//                    insertNode(&bye_glass, order);
//                } else {
//                    free(order);
//                }
//            } else {
//                matching(&bye_glass, order, &oidstore);
//                if(order->qty != 0) {
//                    insertNodeOID(&oidstore, (OID){order->oid, order->price, order->side});
//                    insertNode(&sell_glass, order);
//                } else {
//                    free(order);
//                }
//            }
//            order = (Order*)malloc(sizeof (Order));
//        } else if(c_type == 'C') {
//            NodeOID *noid = findNodeOID(&oidstore, order->oid);
//            if(noid == NULL)
//                continue;
//            printf("X,%d\n", noid->data.oid);

//            if(noid->data.side == 'B')
//                cancle_order(&bye_glass, noid);
//            else
//                cancle_order(&sell_glass, noid);

//            deleteNodeOID(&oidstore, noid);
//        }
//    }
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
        if(in == NULL ) {
            printf("Error openning file <%s>.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Missing argument <input_file>.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < 1; i++) {
        fseek(in, 0, SEEK_SET);
        exchange(in);
    }
    fclose(in);
    return EXIT_SUCCESS;
}
