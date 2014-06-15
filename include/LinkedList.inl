/**
 * Includes external definitions of template-related function definitions.
 *
 * Version: 06/15/2014
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
    next = newNext;
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
 * Constructor for node. Must at least have data.
 *
 * Param: data - the data to store in the node.
 */
template <typename T> LinkedList<T>::Node::Node (T initData) 
                                               : data (initData), next(NULL)
{

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
 * Destructor for Nodes.
 */
template <typename T> LinkedList<T>::Node::~Node()
{
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

/**
 * Returns the size of the LinkedList.
 */
template <typename T> int LinkedList<T>::GetSize()
{
    return size;
}

/**
 * Constructor for list iterator.
 */
template <typename T> LinkedList<T>::ListIterator::ListIterator(LinkedList* list)
{
    current = list->head;
}

/**
 * Returns next element. Call HasNext() first. The behavior of this while 
 * HasNext() is false is undefined.
 *
 * Return: The next element in the list.
 */
template <typename T> T LinkedList<T>::ListIterator::Next()
{
    T data = current->GetData();
    current = current->GetNext();
    return data;
}

/**
 * Returns an iterator over this list.
 */
template <typename T> typename LinkedList<T>::ListIterator LinkedList<T>::Iterator()
{
    return ListIterator(this);
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


#endif
