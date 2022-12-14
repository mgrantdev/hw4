#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor and setting
 * the color to red since every new node will be red when it is first inserted.
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key &key); // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    AVLNode<Key, Value> *rightRotation(AVLNode<Key, Value> *n);
    AVLNode<Key, Value> *leftRotation(AVLNode<Key, Value> *n);
    int calculateBalance(AVLNode<Key, Value> *n);
    int getHeight(AVLNode<Key, Value> *n);
    void updateBalances(AVLNode<Key, Value> *n);
    void rebalanceTree(AVLNode<Key, Value> *ancestor, AVLNode<Key, Value> *newNode, AVLNode<Key, Value> *root);
};

// @summary Helper function to calculate the ancestor nodes of the tree
template <class Key, class Value>
void AVLTree<Key, Value>::updateBalances(AVLNode<Key, Value> *n)
{
    int8_t lHeight = 0, rHeight = 0;
    // std::cout << "UPDATING" << std::endl;
    while (n != nullptr)
    {
        if (n->getLeft() == nullptr)
            lHeight = 0;
        if (n->getLeft() != nullptr)
            lHeight = n->getLeft()->getBalance();
        if (n->getRight() == nullptr)
            rHeight = 0;
        if (n->getRight() != nullptr)
            rHeight = n->getRight()->getBalance();
        n->setBalance(std::max(lHeight, rHeight) + 1);
        /*std::cout << std::max(lHeight, rHeight)+1 << std::endl;
        std::cout << "L " << lHeight << std::endl;
        std::cout << "R" << rHeight << std::endl;
        std::cout << "----" << std::endl;*/
        n = n->getParent();
    }
}

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // @summary Insert using BST insert method
    // @condition Create new root node if it doesn't exist
    if (this->root_ == nullptr)
    {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    // @summary Search for appropiate key location
    AVLNode<Key, Value> *p = nullptr;
    AVLNode<Key, Value> *newNode = static_cast<AVLNode<Key, Value> *>(this->root_);
    AVLNode<Key, Value> *ancestor = NULL;
    AVLNode<Key, Value> *root = static_cast<AVLNode<Key, Value> *>(this->root_);
    bool setLeftChild = false;

    while (newNode != nullptr)
    {
        p = newNode; // will become the parent

        // @condition If key is smaller, traverse left subtree
        if (new_item.first < newNode->getKey())
        {
            newNode = newNode->getLeft();
            setLeftChild = true;
        }
        else if (new_item.first > newNode->getKey())
        {

            newNode = newNode->getRight();
            setLeftChild = false;

        } // @condition If key is the same, update value
        else if (new_item.first == newNode->getKey())
        {
            newNode->setValue(new_item.second);
            ancestor = newNode;
            return;
        }
    }

    newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, p);

    // @condition Determine direction of child and set new parent
    if (!setLeftChild)
    {
        p->setRight(newNode);
    }
    else
    {
        p->setLeft(newNode);
    }

    // @summary Rebalance tree
    if (newNode == nullptr)
        return;
    updateBalances(newNode->getParent());

    AVLNode<Key, Value> *cursorNode = newNode->getParent();
    rebalanceTree(ancestor, newNode, root);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rebalanceTree(AVLNode<Key, Value> *ancestor, AVLNode<Key, Value> *newNode, AVLNode<Key, Value> *root)
{

    // Case 1: All subtrees are balanced
    if (ancestor == NULL)
    {
        if (newNode->getKey() < root->getKey())
        {
            root->setBalance(-1);
        }
        else
            root->setBalance(1);
        updateBalances(newNode);
    }

    // Case 2: Insertion in opposite subtree of ancestor's BF
    else if ((ancestor->getBalance() < 0) && (newNode->getKey() > ancestor->getKey()) ||
             ((ancestor->getBalance() > 0) && (newNode->getKey() < ancestor->getKey())))
    {
        ancestor->setBalance(0);
        updateBalances(newNode);
    }

    // Case 3: ancestor is right heavy and node inserted is in the right subtree of ancestor's left child
    else if ((ancestor->getBalance() > 0) && (newNode->getKey() > ancestor->getRight()->getKey()))
    {
        ancestor->setBalance(0);
        leftRotation(ancestor);
        updateBalances(newNode);
    }
    // Case 4: ancestor is left heavy and node inserted is in the left subtree of ancestor's left child
    else if ((ancestor->getBalance() < 0) && (newNode->getKey() < ancestor->getLeft()->getKey()))
    {
        ancestor->setBalance(0);
        rightRotation(ancestor->getLeft());
        updateBalances(newNode);
    }

    // Case 5: ancestor is left heavy and node inserted is in the right subtree of ancestor's left child
    else if ((ancestor->getBalance() < 0) && (newNode->getKey() > ancestor->getLeft()->getKey()))
    {
        // Perform double right rotation (actually a left followed by a right)
        rightRotation(ancestor->getLeft());
        rightRotation(ancestor);
        updateBalances(newNode);
    }

    //--------------------------------------------------------------------------------
    // Case 6: ancestor.balanceFactor is 'R' and the node inserted is
    //      in the left subtree of ancestor's right child
    //--------------------------------------------------------------------------------
    else
    {
        // Perform double left rotation (actually a right followed by a left)
        rightRotation(ancestor->getRight());
        leftRotation(ancestor);
        updateBalances(newNode);
    }
}

// @summary Retrieve the height of the tree from node n
template <class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value> *n)
{
    int lSubtreeHeight, rSubstreeHeight;
    int finalHeight = 0;

    // @summary Traverse up to right and keep track of subtree heights
    if (n != nullptr)
    {
        lSubtreeHeight = getHeight(n->getLeft());
        rSubstreeHeight = getHeight(n->getRight());
        finalHeight = std::max(lSubtreeHeight, rSubstreeHeight) + 1;
    }
    return finalHeight;
}

// @summary Calculate the balance of subtrees at node n
template <class Key, class Value>
int AVLTree<Key, Value>::calculateBalance(AVLNode<Key, Value> *n)
{
    // If no subtree, that subtree's height is 0
    int lHeight = n->getLeft() != nullptr ? this->getHeight(n->getLeft()) : 0;
    int rHeight = n->getRight() != nullptr ? this->getHeight(n->getRight()) : 0;
    return (rHeight - lHeight);
}

// @summary Helper function to rotate right
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::rightRotation(AVLNode<Key, Value> *n)
{
    AVLNode<Key, Value> *currLeft = n->getLeft();
    AVLNode<Key, Value> *currLeft_RightChild = currLeft->getRight();

    // @summary Move current node down, set left node equal to the right child of the child node (could be null)
    currLeft->setRight(n);
    n->setLeft(currLeft_RightChild);

    // @summary Rebalance
    n->setBalance(std::max(this->getHeight(n->getLeft()), this->getHeight(n->getRight())) + 1);
    currLeft->setBalance(std::max(this->getHeight(currLeft->getLeft()), this->getHeight(currLeft->getRight())) + 1);

    // Return new root
    return n;
}

// @summary Helper function to rotate left
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::leftRotation(AVLNode<Key, Value> *n)
{
    AVLNode<Key, Value> *currRight = n->getRight();
    AVLNode<Key, Value> *currRight_LeftChild = currRight->getLeft();

    // @summary Move current node down, set right node equal to the left child of the child node (could be null)
    currRight->setLeft(n);
    n->setRight(currRight_LeftChild);

    // @summary Rebalance
    n->setBalance(std::max(this->getHeight(n->getLeft()), this->getHeight(n->getRight())) + 1);
    currRight->setBalance(std::max(this->getHeight(currRight->getLeft()), this->getHeight(currRight->getRight())) + 1);

    // Return new root
    return n;
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    // TODO
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
