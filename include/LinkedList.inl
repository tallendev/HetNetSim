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
 * Helper for the deconstructor. This is currently recursive - could be 
 * reimplemented iteratively for possible better performance.
 *
 * Param: node - The node to free.
 */
template <typename T> void LinkedList<T>::FreeNode(Node* node)
{
    if (node->GetNext())
    {
        FreeNode(node->GetNext());
    }
    delete node;
}

/**
 * Destructor for LinkedList. Frees all nodes.
 */
template <typename T> LinkedList<T>::~LinkedList()
{
    if (head)
        FreeNode(head);
}

/**
 * Add an item to the linked list.
 * Param: item - The data to be stored in the node.
 */
template <typename T> void LinkedList<T>::Add(T item)
{
    if (head == NULL)
    {
        head = new Node(item);
        tail = head;
    }
    else
    {
        tail->SetNext(new Node(item));
        tail = tail->GetNext();
    }
    size++;
}

/**
 * Retrieve element from index specified.
 * If index < 0 or index >= size, behavior is undefined.
 *
 * Param: index - the 1-based index from which to retreive an element.
 */
template <typename T> T LinkedList<T>::Get(int index)
{
    if (index == size - 1)
    {
        return tail->GetData();
    }
    else
    {
        Node* probe = head;
        for (int i = 1; i < index; i++)
        {
            probe = probe->GetNext();
        }
        return probe->GetData();
    }
}

#endif
