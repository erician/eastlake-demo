#ifndef BST_H_
#define BST_H_

#ifndef EASTLAKE_H_
#include "eastlake.h"
#endif

#include <iostream>
#include <string>

typedef unsigned long long Value_t;

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
    ~BSTree() {
        if (pod_ != -1)
            po_close(pod_);
    }
    /* if po is not created yet, create po; else, open po */
    int Open();
    int Insert(Value_t val);
    /* if delete successfully, return true, or faluse */
    bool Delete(Value_t val);
    /* if val exists, return true, or faluse */
    bool Search(Value_t val);
    int Size();
    BSTreeNode* Find(Value_t val);
    BSTreeNode* Minimum(BSTreeNode *curr);

    /* 
     * default compare fucntion
     * if val1 == val2, return 0; if val1 > val2, return 1;
     * else return -1.
     */ 
    static int DefaultCmp(Value_t val1, Value_t val2);
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