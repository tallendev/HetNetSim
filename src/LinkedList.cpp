/**
 * Concrete implementations of the methods in the LinkedList class.
 *
 * Version - 06/12/2014
 * Author - Tyler Allen
 * Author - Matthew Leads
 */

#include "LinkedList.h"

/**
 * Default constructor for an empty list.
 */
template <typename T> LinkedList<T>::LinkedList()
{
    head = NULL;
    tail = NULL;
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
        tail.setNext(new Node(item));
    }
}

/**
 * Deconstructor for linked list. Frees all data associated with nodes. Does 
 * not attempt to free T, as source of T is unknown.
 */
template <typename T> LinkedList<T>::~LinkedList()
{
   if (head)
   {
       freeNode(head); 
   }
}

/**
 * Helper for the deconstructor. This is currently recursive - could be 
 * reimplemented iteratively for possible better performance.
 *
 * Param: node - The node to free.
 */ 
template <typename T> void LinkedList<T>::freeNode(Node* node)
{
    if (node.getNext())
    {
        freeNode(node.getNext());
    }
    delete node;
}

/**
 * Constructor for node. Must at least have data.
 *
 * Param: data - the data to store in the node.
 */
template <typename T> LinkedList<T>::Node::Node (T data)
{
    Node (data, NULL);
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
template<typename T> T LinkedList<T>::Node::getData()
{
    return data;
}

/**
 * Change data for a given node.
 *
 * Param: newData - the data to store in the node.
 */
template <typename T> void LinkedList<T>::Node::setData(T newData)
{
    data = newData;
}

/**
 * Gets next element in the list.
 *
 * Return: The next node in the list.
 */
template <typename T> typename LinkedList<T>::Node* LinkedList<T>::Node::getNext ()
{
    return next;
}

/**
 * Changes the node that this node links to. 
 * Param: newNext - Node to replace next with.
 */
template <typename T> void LinkedList<T>::Node::setNext (Node* newNext)
{
    newNext = next;
}
