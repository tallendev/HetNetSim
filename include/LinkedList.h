/**
 * This is a generic implementation of a linked list (template). It uses an 
 * inner Node class for data encapsulation. 
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leads
 *
 * TODO: We should implement an iterator inner class so that this guy can safely
 * and efficiently be iterated over.
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

template <typename T> class LinkedList;

/**
 * Implementation of a LinkedList that accepts a generic data type.
 */
template <typename T> class LinkedList
{
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
        class Node
        {
            private:
                /** The next element in the list following this node.*/
                Node* next;
                /** The data stored by this node. */
                T data;
            public:
                Node(T data);
                Node(T data, Node next);
                virtual T getData();
                virtual void setData(T data);
                virtual Node* getNext();
                virtual void setNext(Node* next);
        };
    public:
        LinkedList();
        void add(T item);
        int getSize();
        T get(int index);
        ~LinkedList();
};

#include "LinkedList.inl"

#endif
