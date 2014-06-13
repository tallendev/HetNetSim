/**
 * Includes external definitions of template-related function definitions.
 *
 * Version: 06/12/2014
 *
 * Author: Tyler Allen
 * Author: Matthew Leads
 *
 */

#ifndef LINKEDLIST_INL
#define LINKEDLIST_INL

/**
 * Default constructor for an empty list.
 */
template <typename T> LinkedList<T>::LinkedList()
{
    head = NULL;
    tail = NULL;
    size = 0;
}

/**
 * Add an item to the linked list.
 * Param: item - The data to be stored in the node.
 */
template <typename T> void LinkedList<T>::add(T item)
{
    if (head == NULL)
    {
        head = new Node(item);
        tail = head;
    }
    else
    {
        tail->setNext(new Node(item));
        tail = tail->getNext();
    }
    size++;
}

/**
 * Retrieve element from index specified.
 * If index < 0 or index >= size, it returns an item of type T.
 *
 * Param: index - the 1-based index from which to retreive an element.
 */
template <typename T> T LinkedList<T>::get(int index)
{
    if (index < 0 || index >= size) 
    {
        T empty;
        return empty;
    }
    else if (index = size - 1)
    {
        return tail->getData();
    }
    else
    {
        Node* probe = head;
        for (int i = 1; i < index; i++)
        {
            probe = probe->getNext();
        }
        return probe->getData();
    }
}

#endif
