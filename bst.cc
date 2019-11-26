#ifndef BST_H_
#include "bst.h"
#endif

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

BSTree::BSTree(std::string po_name) {
    po_name_ = po_name;
    pod_ = -1;
    meta_ = NULL;
}

int BSTree::Open() {
    /* just the owner has read and write privilege */
    pod_ = po_open(po_name_.c_str(), O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(pod_==-1) {
        std::cout<<"open po failed, errno: "<<errno<<std::endl;
        return -1;
    }
    /* get po state */
    struct po_stat statbuf;
    if(po_fstat(pod_, &statbuf) == -1) {
        std::cout<<"get po state failed, errno: "<<errno<<std::endl;
        return -1;
    }
    /* map po */
    if(statbuf.st_size != 0) { 
        unsigned long reval = po_mmap(0, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, pod_, 0);
        if(reval == -1) {
            meta_ = NULL;
            std::cout<<"map po failed, errno: "<<errno<<std::endl;
            return -1;
        }
        meta_ = (BSTreeMetadata*)reval;
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

int BSTree::Insert(Value_t val) {
    if(meta_ == NULL) {
        return -1;
    }

    BSTreeNode *tmp;
    tmp = (BSTreeNode*)po_malloc(pod_, sizeof(struct BSTreeNode));
    if(tmp == NULL) {
        std::cout<<"malloc from po failed, errno: "<<errno<<std::endl;
        return -1;
    }
    tmp->value_ = val;
    tmp->left_ = NULL;
    tmp->right_ = NULL;
    tmp->parent_ = NULL;

    if(meta_->root_node_ == NULL) {
        meta_->root_node_ = tmp;
        meta_->size_++;
        return 0;
    }
    BSTreeNode *curr, *parent;
    curr = parent = meta_->root_node_;
    while(curr != NULL) {
        parent = curr;
        if(DefaultCmp(curr->value_, val) >= 0) {
            curr = curr->left_;
        }else {
            curr = curr->right_;
        }
    }
    tmp->parent_ = parent;
    if(DefaultCmp(parent->value_, val) >= 0) {
        parent->left_ = tmp;
    }else {
        parent->right_ = tmp;
    }
    meta_->size_++;
    return 0;
}



bool BSTree::Delete(Value_t val) {
    BSTreeNode *found_node = Find(val);
    if(found_node == NULL) {
        return false;
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
    po_free(pod_, found_node);
    return true;
}

bool BSTree::Search(Value_t val) {
    BSTreeNode *found_node = Find(val);
    if (found_node == NULL)
        return false;
    else
        return true; 
}


BSTreeNode* BSTree::Find(Value_t val) {
    if (meta_ == NULL) {
        return NULL;
    }
    if (meta_->root_node_ == NULL) {
        return NULL;
    }
    BSTreeNode *curr = meta_->root_node_;
    int cmp_result;
    while(curr != NULL) {
        cmp_result = DefaultCmp(curr->value_, val);
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

void BSTree::Transplant(BSTreeNode *old_node, BSTreeNode *new_node) {
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

int BSTree::DefaultCmp(Value_t val1, Value_t val2) {
    if(val1 == val2) {
        return 0;
    }else if(val1 > val2) {
        return 1;
    }else {
        return -1;
    }
}

int BSTree::Size() {
    if (meta_ != nullptr)
        return meta_->size_;
    return -1;
}
