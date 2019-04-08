#ifndef LULLABY_BINARY_SEARCH_TREE_H_
#include "lullaby_binary_search_tree.h"
#endif

#ifndef LULLABY_H_
#include "lullaby.h"
#endif

#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

BSTree::BSTree(std::string po_name) {
    po_name_ = po_name;
    pod_ = -1;
    meta_ = NULL;
}

int BSTree::Open() {
    /* just the owner has read and write privilege */
    pod_ = sys_po_open(po_name_.c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(pod_==-1) {
        std::cout<<"open po failed, errno: "<<errno<<std::endl;
        return -1;
    }
    /* get po state */
    struct po_stat statbuf;
    if(sys_po_fstat(pod_, &statbuf) == -1) {
        std::cout<<"get po state failed, errno: "<<errno<<std::endl;
        return -1;
    }
    /* map po */
    if(statbuf.st_size != 0) { 
        meta_ = sys_po_mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, pod_, 0);
        if(meta_ == -1) {
            meta_ = NULL;
            std::cout<<"map po failed, errno: "<<errno<<std::endl;
            return -1;
        }
    }else {
        /* po_malloc is a user level library function, which will call sys_po_mmap, 
        so, we don't need to call it again */
        /* be careful, when you first use po_malloc with a persistent object */
        meta_ = (BSTreeMetadata*)po_malloc(pod_, sizeof(struct BSTreeMetadata));
        if(meta_ == NULL) {
            std::cout<<"malloc from po failed, errno: "<<errno<<std::endl;
            return -1;
        }
        meta_->root_node_ = NULL;
        meta_->size_ = 0;
    }
    return 0;
}

int BSTree::Insert(BSTreeNode *node) {
    if(meta_ == NULL) {
        return -1;
    }

    BSTreeNode *tmp;
    tmp = (BSTreeNode*)po_malloc(pod_, sizeof(struct BSTreeNode));
    if(tmp == NULL) {
        std::cout<<"malloc from po failed, errno: "<<errno<<std::endl;
        return -1;
    }
    tmp->value_ = node->value_;
    tmp->left_ = NULL;
    tmp->right_ = NULL;
    tmp->parent_ = NULL;

    if(meta_->root_node_ == NULL) {
        meta_.root_node_ = tmp;
        return 0;
    }
    BSTreeNode *curr, *parent;
    curr = parent = meta_.root_node_;
    while(curr != NULL) {
        parent = curr;
        if(DefaultCmp(curr, node) >= 0) {
            curr = curr->left_;
        }else {
            curr = curr->left_;
        }
    }
    tmp->parent_ = parent;
    if(DefaultCmp(parent, node) >= 0) {
        parent->left_ = tmp;
    }else {
        parent->right_ = tmp;
    }
    meta_.size_++;
    return 0;
}



int BSTree::Delete(BSTreeNode *node) {
    BSTreeNode *found_node = Find(node);
    if(found_node == NULL) {
        return -1;
    }
    if(found_node->left_ == NULL) {
        Transplant(found_node, found_node->right_);
    }else if(found_node->right_ == NULL) {
        Transplant(found_node, found_node->left_);
    }else {
        BSTreeNode *successor = Minimum((found_node->right_));
        if(found_node->right_ != successor) {
            Transplant(successor, successor->right_);
            successor->right_ = found_node->right_;
            successor->right_->parent_ = successor;
        }
        Transplant(found_node, successor);
        successor->left_ = found_node->left_;
        successor->left_->parent_ = successor;
    }
    meta_->size_--;
    po_free(found_node);
    return 0;
}

BSTreeNode* BSTree::Find(BSTreeNode *node) {
    if(meta_ == NULL || meta_->root_node_ == NULL) {
        return -1;
    }
    BSTreeNode *curr = meta_->root_node_;
    int cmp_result;
    while(curr != NULL) {
        cmp_result = DefaultCmp(curr, node);
        if(cmp_result == 0) {
            return curr;
        }else if(cmp_result > 0) {
            curr = curr->left_;
        }else {
            curr = curr->right_;
        }
    }
    return NULL;
}

BSTreeNode* BSTree::Minimum(BSTreeNode *curr) {
    if(curr == NULL) {
        return NULL;
    }
    while(curr->left_!=NULL) {
        curr = curr->left_;
    }
    return curr;
}

void Transplant(BSTreeNode *old_node, BSTreeNode *new_node) {
    if(old_node->parent_ == NULL) {
        meta_->root_node_ = new_node;
    }else if(old_node->parent_->left_ == old_node) {
        old_node->parent_->left_ = new_node;
    }else {
        old_node->parent_->right_ = new_node;
    }
    if(new_node != NULL) {
        new_node->parent_ = old_node->parent_;
    }
}

int BSTree::DefaultCmp(BSTreeNode *node1, BSTreeNode *node2) {
    if(node1.value_ == node2.value_) {
        return 0;
    }else if(node1.value_ > node2.value_) {
        return 1;
    }else {
        return -1;
    }
}

