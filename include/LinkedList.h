/**
 * This is a generic implementation of a linked list (template). It uses an
 * inner Node class for data encapsulation.
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leeds
 *
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

template <typename T> class LinkedList;

/**
 * Implementation of a LinkedList that accepts a generic data type.
 */
template <typename T> class LinkedList {
        class Node;
    private:
        /** The head of the linked list. */
        Node* head;
        /** Tail of the linked list for quick adds. */
        Node* tail;
        /** Current size of the linked list. */
        int size;

        void freeNode(Node* node);

        /**
         * Nodes making up our linked list.
         */
        class Node {
            private:
                /** The data stored by this node. */
                T data;
                /** The next element in the list following this node.*/
                Node* next;
            public:
                Node(T data);
                Node(T data, Node next);
                virtual ~Node();
                virtual T getData();
                virtual void setData(T data);
                virtual Node* getNext();
                virtual void setNext(Node* next);
        };

    public:
        class ListIterator {
                friend class LinkedList;
            private:
                Node* current;
            protected:
                ListIterator(LinkedList* list) {
                    current = list->head;
                }
            public:
                ~ListIterator();
                bool hasNext();
                T next();
        };
        LinkedList();
        void add(T item);
        int getSize();
        T get(int index);
        ListIterator iterator();
        ~LinkedList();
};

#include "LinkedList.inl"

#endif
