#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
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

float macheps(void)
{
    float e = 1.0f;

    while (1.0f + e / 2.0f > 1.0f)
        e /= 2.0f;
    return e;
}

int main()
{

    table_t  tb;
    void*  k;
    iter_t it;
    slist* p_;
    OrderId orderid = {'S', 280.3};

    table_init(&tb, &cmp_str, &hash_str, &free_str);
//    for(int i = 0; i < 500; i++) {
//        orderid.price = (rand()%10) / 100. + 300.;
//        table_insert(&tb, &i, orderid);
//    }

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

    int count_args = 0;
    unsigned int trade_id = 0;
    Node* bye_glass = NIL;
    Node* sell_glass = NIL;
    max_bye = 0;
    max_sell = 1000000;
    const float eps = macheps();
    while((count_args = fscanf(in, "%c,%d,%c,%d,%f", &c_type, &uniq_number, &c_side, &qty, &price)) != EOF) {
        PriceData *n = NULL;
        n = (PriceData*)malloc(sizeof (PriceData));

        n->price = price;
        n->number_pack = uniq_number;
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
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->num_pack, n->number_pack, n->qty, orders->value->price);
                            n->qty = 0;
                            // TODO Отправляем сделки
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->num_pack, n->number_pack, orders->value->qty, orders->value->price);
                            table_remove(&tb, &orders->value->num_pack);
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'S', orders->value->num_pack, n->number_pack, orders->value->qty, orders->value->price);
                            table_remove(&tb, &orders->value->num_pack);
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        table_remove(&tb, &n->number_pack);
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
                    //printf("Orders bye closed");// Исполняем ордер выходим
                    insertNode(&bye_glass, *n);
                    orderid.price = n->price;
                    orderid.side = 'B';
                    table_insert(&tb, &n->number_pack, orderid);
//                    table_insert(&tb, &n->number_pack, );
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
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->num_pack, n->number_pack, n->qty, orders->value->price);
                            n->qty = 0;
                            // TODO Отправляем сделки
                            break;
                        } else if(orders->value->qty < n->qty) {
                            n->qty -= orders->value->qty;
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->num_pack, n->number_pack, orders->value->qty, orders->value->price);
                            table_remove(&tb, &orders->value->num_pack);
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            // Ордер который был в стакане закрыт
                        } else {
                            printf("T,%u,%c,%d,%d,%d,%.2f\n", trade_id++, 'B', orders->value->num_pack, n->number_pack, orders->value->qty, orders->value->price);
                            table_remove(&tb, &orders->value->num_pack);
                            pop_front(tmp->data.price_level);
                            orders = tmp->data.price_level->head;
                            n->qty = 0;
                            // Оба ордера закрыты
                        }
                    }
                    if(tmp->data.price_level->size == 0) {
                        table_remove(&tb, &n->number_pack);
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
//                    printf("Orders sell closed");// Исполняем ордер выходим
                    insertNode(&sell_glass, *n);
                    orderid.price = n->price;
                    orderid.side = 'S';
                    table_insert(&tb, &n->number_pack, orderid);
                }
            }

            //printf("type:%c N=%d side:%c qty=%d price=%f\n", c_type, uniq_number, c_side, qty, price);
        } else if(c_type == 'C') {
            slist *p = table_find(&tb, &uniq_number);
            if(p == NULL) {
                printf("error find number pack\n");
                continue;
            }
            float cancle_node = p->val.price;
            char glass_side = p->val.side;
            PriceData pcdt = {cancle_node};
            if(glass_side == 'B') {
                Node *nd = findPriceLvl(&bye_glass, pcdt);
                //printf("Cancle B: %d", nd->data.number_pack);
                deleteNode(&bye_glass, nd);
            } else {
                Node *nd = findPriceLvl(&sell_glass, pcdt);
                //printf("Cancle B: %d", nd->data.number_pack);
                deleteNode(&sell_glass, nd);
            }
            //printf("type:%c N=%d\n", c_type, uniq_number);
        }
    }

//    iter_reset(&it);
//    int counter_tb = 0;
//    while(iter_each(&it, &tb, &k, &orderid))
//        printf("%d (%d %c %f)\n", counter_tb++, *(int*)k, orderid.side, orderid.price);
//    fflush(stdout);

    int bye_max = 0;
    int sell_max = 0;
    heightTree(bye_glass, 0, &bye_max);
    heightTree(sell_glass, 0, &sell_max);
    PriceData *p = (PriceData*)malloc(sizeof(PriceData));
    p->price = 250.61;
    Node *n = NIL;
    n = findPriceLvl(&bye_glass, *p);
    if(n != NULL)
        printf("%f\n",n->data.price);
    printf("\nMax_bye=%d\tmax_sell=%d \nEnd of file!\n", bye_max, sell_max);
    fclose(in);

    return 0;
}
