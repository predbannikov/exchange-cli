#ifndef PRICELEVEL_H
#define PRICELEVEL_H
/* Связный список - очередь ордеров */

#include <stdlib.h>
#include <stdio.h>

/* данные ордера */
typedef struct Order_ {
    unsigned int oid;
    unsigned int qty;
    float price;
    char side;
} Order;

/* связный список ордеров */
typedef struct OrderLinkList_ {
    Order *value;
    struct OrderLinkList_ *next;
} OrderLinkList;

/* контейнер - размер, начало, конец связного списка */
typedef struct PriceLevel_ {
    OrderLinkList *head;
    OrderLinkList *tail;
    unsigned int size;
} PriceLevel;

/* выделение памяти, создание контейнера PriceLevel*/
PriceLevel* createLinkedList() {
    PriceLevel* tmp = (PriceLevel*) malloc(sizeof(PriceLevel));
    if (tmp == NULL) {
        fprintf(stderr, "error: insufficient memory (createLinkedList)\n");
        exit(EXIT_FAILURE);
    }
    tmp->head = tmp->tail = NULL;
    tmp->size = 0;
    return tmp;
}

/* вставка ордера в конец очереди */
void push_back_list(PriceLevel *list, Order *value) {
    OrderLinkList *tmp = (OrderLinkList*) malloc(sizeof(OrderLinkList));
    if (tmp == NULL) {
        fprintf(stderr, "error: insufficient memory (push_back_list)\n");
        exit(EXIT_FAILURE);
    }

    tmp->next = NULL;
    tmp->value = value;
    if(list->tail)
        list->tail->next = tmp;
    if (list->head == NULL) {
        list->head = tmp;
    }
        list->tail = tmp;
    list->size++;
}

/* выталкивание первого ордера из очереди */
Order* pop_front_list(PriceLevel *list) {
    OrderLinkList *tmp = NULL;
    Order *value = NULL;
    if (list->head == NULL) {
        fprintf(stderr, "error: (pop_front_list)");
    }
    tmp = list->head;
    value = tmp->value;
    list->head = list->head->next;
    if (list->tail == tmp) {
        list->tail = NULL;
    }
    free(tmp->value);
    free(tmp);
    list->size--;
    return value;
}

/* удаление ордера с номером oid из очереди */
void delete_Nth_of_oid(PriceLevel *list, unsigned int oid) {
    OrderLinkList *ol = list->head;
    if (ol->value->oid == oid) {
        pop_front_list(list);
    } else {
        while(ol->next != NULL) {
            if(ol->next->value->oid == oid)
                break;
            ol = ol->next;
        }
        if(ol->next != NULL) {
            OrderLinkList *tmp = ol->next;
            ol->next = ol->next->next;
            if(list->tail == tmp) {
                list->tail = ol;
            }
            free(tmp->value);
            free(tmp);
            list->size--;
        }
    }
}

#endif // PRICELEVEL_H
