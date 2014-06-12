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
    }
    size++;
}

/**
 * Retrieve element from index specified. You MUST verify index is smaller 
 * than size, or behavior is undefined.
 * TODO: could be more efficient if tail pointer was made use of.
 *
 * Param: index - the index from which to retreive an element.
 */
template <typename T> T LinkedList<T>::get(int index)
{
    Node* node = head;
    for (int i = 1; i < index; i++)
    {
        node = node->next;
    }
    return node->getData();
}

#endif
