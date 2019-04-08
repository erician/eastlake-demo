#ifndef LULLABY_BINARY_SEARCH_TREE_H_
#define LULLABY_BINARY_SEARCH_TREE_H_

#ifndef LULLABY_H_
#include "lullaby.h"
#endif

#include<string>

struct BSTreeNode {
    int value_;
    BSTreeNode *left_, *right_, *parent_;
};
struct BSTreeMetadata {
    BSTreeNode *root_node_;
    int size_;
};

class BSTree {
public:
    /* init BinTree */
    BSTree(std::string po_name);   
    /* if po is not created yet, create po; else, open po */
    int Open();
    int Insert(BSTreeNode *node);
    int Delete(BSTreeNode *node);
    BSTreeNode* Find(BSTreeNode *node);
    BSTreeNode* Minimum(BSTreeNode *curr);

    /* default compare fucntion
     if node1.value_ == node2.value_, return 0;
     if node1.value_ > node2.value_, return 1;
     else return -1 */ 
    static int DefaultCmp(BSTreeNode *node1, BSTreeNode *node2);
private:
    /* the name of persistent object to this binary tree */
    std::string po_name_;   
    /* the persistent object descriptor */
    int pod_;          
    /* binary search tree root node */
    BSTreeMetadata *meta_;
    /* reference: introduction to algorithm, chapter 12 */
    void Transplant(BSTreeNode *old_node, BSTreeNode *new_node);
    

    

};

#endif