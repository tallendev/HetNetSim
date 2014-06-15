/**
 * This is a generic implementation of a linked list (template). It uses an 
 * inner Node class for data encapsulation. 
 *
 * Version: 06/12/2014
 * Author: Tyler Allen
 * Author: Matthew Leads
 *
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
        
        void FreeNode(Node* node);
        
        /**
         * Nodes making up our linked list.
         */
        class Node
        {
            private:
                /** The data stored by this node. */
                T data;
                /** The next element in the list following this node.*/
                Node* next;
            public:
                Node(T data);
                Node(T data, Node next);
                virtual ~Node();
                virtual T GetData();
                virtual void SetData(T data);
                virtual Node* GetNext();
                virtual void SetNext(Node* next);
        };

    public:
        class ListIterator
        {
            private:
                Node* current;
            public:
                ListIterator(LinkedList* list);
                bool HasNext();
                T Next();
        };
        LinkedList();
        void Add(T item);
        int GetSize();
        T Get(int index);
        ListIterator Iterator();
        ~LinkedList();
};

#include "LinkedList.inl"

#endif
