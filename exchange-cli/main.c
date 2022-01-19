#include <stdio.h>
#include <stdlib.h>
#include "node.h"

/* *************************************************
 * использовать рабочую на данном датасете функцию *
 * вывода информации в файл                        *
 * *************************************************/
#define CUSTOM_PRINT
/* тест на чтение с буфера и обработка алгоритмом */
//#define TEST_CYCLE	1000

unsigned int trade_id = 1;
int left_dgt = 0;
int right_dgt = 0;
char buffer[64];
char ch;
int szbuff = 0;
int tmp = 0;
FILE *in = NULL, *out = NULL;

/* состояния для парсинга входящей строки аналог get_args */
enum {STATE_START, STATE_OID, STATE_SIDE, STATE_QTY, STATE_PRICE_LEFT, STATE_PRICE_RIGHT } state_parser = STATE_START;
enum {STATE_ORDER, STATE_CANCLE} state_ticket = STATE_ORDER;

/* парсинг входящей строки */
void get_args(unsigned int *oid, char *side, unsigned int *qty, float *price) {
    *oid = 0;
    *qty = 0;
    left_dgt = 0;
    right_dgt = 0;
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
                *oid += (ch - '0');
                break;
            case STATE_SIDE:
                *side = ch;
                break;
            case STATE_QTY:
                *qty *= 10;
                *qty += (ch - '0');
                break;
            case STATE_PRICE_LEFT:
                if(ch=='.') {
                    state_parser = STATE_PRICE_RIGHT;
                    break;
                }
                left_dgt *= 10;
                left_dgt += (ch - '0');
                break;
            case STATE_PRICE_RIGHT:
                cnt_right_dgt++;
                right_dgt *= 10;
                right_dgt += (ch - '0');
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
            *oid += (ch - '0');
        }
        break;
    }
    if(cnt_right_dgt == 1)
        *price = left_dgt + right_dgt/10.;
    else
        *price = left_dgt + right_dgt/100.;
}

/* кастомная версия fprintf с совершённой сделкой */
void print_trade(unsigned int oid, unsigned int oid2, unsigned int qty, char side, float price) {

    putc('T', out);
    putc(',', out);

    tmp = trade_id++;
    do buffer[szbuff++] = tmp % 10 + '0'; while ((tmp /= 10) > 0);
    while (szbuff > 0)
        putc(buffer[--szbuff], out);
    putc(',', out);
    putc(side, out);
    putc(',', out);

    left_dgt = price + 0.005;
    right_dgt = ((price - left_dgt + 0.005) * 100);
    if(right_dgt % 10 == 0)
        right_dgt /= 10;
    do buffer[szbuff++] = right_dgt % 10 + '0'; while ((right_dgt /= 10) > 0);
    buffer[szbuff++] = '.';
    do buffer[szbuff++] = left_dgt % 10 + '0'; while ((left_dgt /= 10) > 0);
    buffer[szbuff++] = ',';
    do buffer[szbuff++] = qty % 10 + '0'; while ((qty /= 10) > 0);
    buffer[szbuff++] = ',';
    do buffer[szbuff++] = oid2 % 10 + '0'; while ((oid2 /= 10) > 0);
    buffer[szbuff++] = ',';
    do buffer[szbuff++] = oid % 10 + '0'; while ((oid /= 10) > 0);

    while (szbuff > 0)
        putc(buffer[--szbuff], out);
    putc('\n', out);
}

/* кастомная версия fprintf с отменённым ордером */
void print_cancle(unsigned int oid) {
    putc('X', out);
    putc(',', out);
    do buffer[szbuff++] = oid % 10 + '0';
    while ((oid /= 10) > 0);
    while (szbuff > 0)
        putc(buffer[--szbuff], out);
   putc('\n', out);
}

/* очистить дерево прайсов с ордерами */
void free_nodes(Node* tree) {
    while(tree != NIL) {
        PriceLevel* price_level = tree->price_level;
        OrderLinkList* orders = price_level->head;
        while(orders != NULL) {
            OrderLinkList *tmp = orders;
            orders = orders->next;
            free(tmp->value);
            free(tmp);
        }
        deleteNode(&tree, tree);
    }
}

/* очистить дерево OID с прайсами */
void free_oidstore(NodeOID* tree) {
    while(tree != NILOID)
        deleteNodeOID(&tree, tree);
}

/* отмена ордера */
int cancle_order(Node **glass, NodeOID *noid){
    Node* node = *glass;
    Node *node_to_cncl = findNode(&node, noid->data.price);
    if(node_to_cncl == NULL)
        return 0;

    delete_Nth_of_oid(node_to_cncl->price_level, noid->data.oid);

    if(node_to_cncl->price_level->size == 0)
        deleteNode(&node, node_to_cncl);
    *glass = node;
    return 0;
}

/* получить наименьший/наибольший прайс ордеров */
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

/* произвести сделки ордеров при новопоступившем ордере*/
void matching(Node** glass, Order *order, NodeOID **oidstr) {

    Node* border_elem = extr_elem(*glass, order->side);
    char side = order->side == 'B' ? 'S' : 'B';

    while(border_elem != NIL && order->qty != 0) {						// Если есть покупатели то выполняем сделки
        if(order->side == 'B') {
            if(border_elem->price_level->head->value->price > order->price)
                break;
        } else {
            if(border_elem->price_level->head->value->price < order->price)
                break;
        }
        OrderLinkList *orders = border_elem->price_level->head;			// Получаем первый в очереди ордер на покупку
        while(orders != NULL) {											// Обходим очередь или выход из цикла по break
            if(orders->value->qty > order->qty) {
                orders->value->qty -= order->qty;
#ifdef CUSTOM_PRINT
                print_trade(orders->value->oid, order->oid, order->qty, side, orders->value->price);
#else
                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt %10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, order->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, order->qty, left_dgt, right_dgt);
#endif

                order->qty = 0;
                break;
            } else if(orders->value->qty < order->qty) {
                order->qty -= orders->value->qty;
#ifdef CUSTOM_PRINT
                print_trade(orders->value->oid, order->oid, orders->value->qty, side, orders->value->price);
#else
                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);
#endif
                NodeOID *oid = findNodeOID(oidstr, orders->value->oid);
                deleteNodeOID(oidstr, oid);

                pop_front_list(border_elem->price_level);
                orders = border_elem->price_level->head;
            } else {

#ifdef CUSTOM_PRINT
                print_trade(orders->value->oid, order->oid, orders->value->qty, side, orders->value->price);
#else
                left_dgt = (int) orders->value->price + 0.005;
                right_dgt = (int)((orders->value->price - left_dgt + 0.005) * 100);
                if(right_dgt % 10 == 0)
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt/10);
                else
                    printf("T,%u,%c,%d,%d,%d,%d.%d\n", trade_id++, side, orders->value->oid, order->oid, orders->value->qty, left_dgt, right_dgt);
#endif
                NodeOID *oid = findNodeOID(oidstr, orders->value->oid);
                deleteNodeOID(oidstr, oid);
                pop_front_list(border_elem->price_level);
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

/* цикл обработки полученных ордеров и вставки их в очередь */
void exchange() {
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    NodeOID* oidstore = NILOID;
    Order *order = NULL;
    order = (Order*)malloc(sizeof (Order));
    if (order == NULL) {
        fprintf(stderr, "error: insufficient memory (exchange)\n");
        exit(EXIT_FAILURE);
    }
    while((ch = getc(in)) != EOF) {
        if(ch == 'O')
            state_ticket = STATE_ORDER;
        else
            state_ticket = STATE_CANCLE;
        get_args(&order->oid, &order->side, &order->qty, &order->price);

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
            if (order == NULL) {
                fprintf(stderr, "error: insufficient memory (exchange)\n");
                exit(EXIT_FAILURE);
            }
        } else if(state_ticket == STATE_CANCLE) {
            NodeOID *noid = findNodeOID(&oidstore, order->oid);
            if(noid == NULL)
                continue;
#ifdef CUSTOM_PRINT
            print_cancle(noid->data.oid);
#else
            printf("X,%d\n", noid->data.oid);
//            fprintf(out, "X,%d\n", noid->data.oid);
#endif

            if(noid->data.side == 'B')
                cancle_order(&bye_glass, noid);
            else
                cancle_order(&sell_glass, noid);

            deleteNodeOID(&oidstore, noid);
        }
    }

    free(order);
    free_nodes(bye_glass);
    free_nodes(sell_glass);
    free_oidstore(oidstore);
}

int main(int argc, char **argv)
{
    if(argc > 1 && argc < 3) {
        in = fopen(argv[1], "rt");
        if(in == NULL ) {
            fprintf(stderr, "Error openning file to read <%s>.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
#ifdef TEST_CYCLE
        out = fopen("/dev/null", "wt");
#else
        out = fopen("out.txt", "wt");
#endif
        if(out == NULL) {
            fprintf(stderr, "Error opening file to write out.txt\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Missing argument <input_file>.\n");
        exit(EXIT_FAILURE);
    }
#ifdef TEST_CYCLE
    for(int i = 0; i < TEST_CYCLE; i++) {
        fseek(in, 0, SEEK_SET);
        exchange();
    }
#else
    exchange();
#endif
    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}
