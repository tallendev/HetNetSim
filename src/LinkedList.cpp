/**
 * Concrete implementations of the methods in the LinkedList class.
 *
 * Version - 06/12/2014
 * Author - Tyler Allen
 * Author - Matthew Leads
 */

#include "LinkedList.h"


/**
 * Constructor for node. Must at least have data.
 *
 * Param: data - the data to store in the node.
 */
template <typename T> LinkedList<T>::Node::Node (T data)
{
    Node (data, NULL);
}

template <typename T> int LinkedList<T>::GetSize()
{
    return size;
}


/**
 * Constructor for node. Initializes the data and next fields.
 *
 * Param: data - the data to store in the node.
 * Param: next - the next element in the list.
 */
template <typename T> LinkedList<T>::Node::Node (T data, Node next) 
                                                : data (data), next (next)
{

}

/**
 * Retrieve data from a given node.
 *
 * Return: The data stored in the node.
 */
template<typename T> T LinkedList<T>::Node::GetData()
{
    return data;
}

/**
 * Change data for a given node.
 *
 * Param: newData - the data to store in the node.
 */
template <typename T> void LinkedList<T>::Node::SetData(T newData)
{
    data = newData;
}

/**
 * Gets next element in the list.
 *
 * Return: The next node in the list.
 */
template <typename T> typename LinkedList<T>::Node* LinkedList<T>::Node::GetNext ()
{
    return next;
}

/**
 * Changes the node that this node links to. 
 * Param: newNext - Node to replace next with.
 */
template <typename T> void LinkedList<T>::Node::SetNext (Node* newNext)
{
    newNext = next;
}

/**
 * Constructor for list iterator.
 */
template <typename T> LinkedList<T>::ListIterator::ListIterator(LinkedList* list)
{
    current = list->head;
}

/**
 * Checks if there is a "next" element. Call this before calling next.
 *
 * Return: True if next() is a valid call. False if not.
 */
template <typename T> bool LinkedList<T>::ListIterator::HasNext()
{
    return (bool) current;
}

/**
 * Returns next element. Call hasNext() first. The behavior of this while 
 * hasNext() is false is undefined.
 *
 * Return: The next element in the list.
 */
template <typename T> T LinkedList<T>::ListIterator::Next()
{
    T data = current->GetData();
    current = current->next;
    return data;
}

/**
 * Returns an iterator over this list.
 */
template <typename T> typename LinkedList<T>::ListIterator LinkedList<T>::Iterator()
{
    return ListIterator(this);
}
