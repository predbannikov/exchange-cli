#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Order_ {
    unsigned int oid;
    unsigned int qty;
    char side;
    float price;
} Order;

typedef struct OrderLevel_ {
    Order *value;
    struct OrderLevel_ *next;
} OrderLevel;

typedef struct PriceLevel_ {
    OrderLevel *head;
    OrderLevel *tail;
    unsigned int size;
} PriceLevel;

PriceLevel* createLinkedList() {
    PriceLevel* tmp = (PriceLevel*) malloc(sizeof(PriceLevel));
    tmp->head = tmp->tail = NULL;
    tmp->size = 0;
    return tmp;
}

void push_back(PriceLevel *list, Order *value) {
    OrderLevel *tmp = (OrderLevel*) malloc(sizeof(OrderLevel));
    if (tmp == NULL) {
        printf("error malloc:");
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

Order* pop_front(PriceLevel *list) {
    OrderLevel *tmp = NULL;
    Order *value = NULL;
    if (list->head == NULL) {
        printf("error:");
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

void delete_oid(PriceLevel *list, unsigned oid) {
    OrderLevel *ol = list->head;
    if (ol->value->oid == oid) {
        pop_front(list);
    } else {
        while(ol->next != NULL) {
            if(ol->next->value->oid == oid)
                break;
            ol = ol->next;
        }
        if(ol->next != NULL) {
            OrderLevel *tmp = ol->next;
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
