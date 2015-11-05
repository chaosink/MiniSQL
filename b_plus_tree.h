#ifndef B_PLUS_TREE_HPP
#define B_PLUS_TREE_HPP

#include "buffer_manager.h"
#include "table.h"
#include <fstream>
#include <vector>
#include <iostream>

#define LAST_POINTER (pointer_num_ - 1)
#define MAX_VALUE_NUM (pointer_num_ - 1)

enum NodeState {EMPTY, NONLEAF, LEAF};

template <class V, class P>
struct Node {
    int *num;
    char *state;
    int *value_num;
    P *pointer;
    V *value;
    Node() {
        num = NULL;
        state = NULL;
        value_num = NULL;
        pointer = NULL;
        value = NULL;
    }
};

template <class V, class P>
class BPlusTree {
    std::string name_;
    int pointer_num_;
    int node_num_;
    int empty_node_num_;
    Node<V, P> swapper_;
    std::vector<int> queue_;
    int root_;
    IndexInfo *index_info_;
    BufferManager &buffer_manager_;
    void AddOneBlock();
    Node<V, P> GetNode(int node_num);
    Node<V, P> GetAnAvailableNode();
    Node<V, P> FindLeafNode(V value);
    void InsertInLeaf(Node<V, P> node, V value, P pointer);
    void InsertInNonleaf(Node<V, P> node, int pointer_left_num, V value, int pointer_right_num);
    void InsertInParent(Node<V, P> node_left, V value, Node<V, P> node_right);
    void DeleteEntry(Node<V, P> node, V value);
    void DeleteInNode(Node<V, P> node, V value);
    bool GetSiblingAndSeperator(Node<V, P> node, P pointer, Node<V, P> &sibling_node, V &seperator);
    void ReplaceSeperator(Node<V, P> node, V value_old, V value_new);
public:
    BPlusTree(IndexInfo *index_info, BufferManager &buffer_manager);
    ~BPlusTree();
    P Find(V value);
    void Insert(V value, P pointer);
    void Delete(V value);
};

template <class V, class P>
BPlusTree<V, P>::BPlusTree(IndexInfo *index_info, BufferManager &buffer_manager) : buffer_manager_(buffer_manager) {
    pointer_num_ = (BLOCK_SIZE - sizeof(int) - sizeof(char) - sizeof(int) - sizeof(P)) / (sizeof(P) + sizeof(V)) + 1;
    swapper_.value_num = new int;
    swapper_.pointer = new P[pointer_num_ + 1];
    swapper_.value = new V[pointer_num_];

    index_info_ = index_info;
    name_ = index_info_->index_name;
    root_ = index_info_->root;
    node_num_ = index_info_->block_num;
    empty_node_num_ = index_info_->empty_block_num;
}

template <class V, class P>
BPlusTree<V, P>::~BPlusTree() {
    delete swapper_.value_num;
    delete[] swapper_.pointer;
    delete[] swapper_.value;
    index_info_->root = root_;
    index_info_->block_num = node_num_;
    index_info_->empty_block_num = empty_node_num_;
}

template <class V, class P>
void BPlusTree<V, P>::AddOneBlock() {
    static char empty_block[BLOCK_SIZE] = {0};
    std::ofstream output((name_ + ".idx").c_str(), std::ofstream::app | std::ofstream::binary);
    output.write(empty_block, BLOCK_SIZE);
    output.close();
}

template <class V, class P>
Node<V, P> BPlusTree<V, P>::GetNode(int node_num) {
    if(node_num >= node_num_) {
        std::cerr << "Node " << node_num << " doesn't exist!" << std::endl;
        return Node<V, P>();
    }
    char *block = buffer_manager_.GetFileBlock(name_ + ".idx", node_num);
    Node<V, P> node;
    node.num = (int *)block;
    node.state = block + sizeof(int);
    node.value_num = (int *)(block + sizeof(int) + sizeof(char));
    node.pointer = (P *)(block + sizeof(int) + sizeof(char) + sizeof(int));
    node.value = (V *)(block + sizeof(int) + sizeof(char) + sizeof(int) + sizeof(P) * pointer_num_);
    return node;
}

template <class V, class P>
Node<V, P> BPlusTree<V, P>::GetAnAvailableNode() {
    if(empty_node_num_)
        for(int i = 0; i < node_num_; i++) {
            Node<V, P> node = GetNode(i);
            if(*node.state == EMPTY) {
                std::cout << "Empty node: " << i << std::endl;
                empty_node_num_--;
                return node;
            }
        }
    AddOneBlock();
    node_num_++;
    Node<V, P> node = GetNode(node_num_ - 1);
    *node.num = node_num_ - 1;
    *node.state = EMPTY;
    *node.value_num = 0;
    return node;
}

template <class V, class P>
Node<V, P> BPlusTree<V, P>::FindLeafNode(V value) {
    if(root_ == -1) return Node<V, P>();
    Node<V, P> node = GetNode(root_);
    queue_.clear();
    while(*node.state != LEAF) {
        queue_.push_back(*node.num);
        int i;
        for(i = 0; i < *node.value_num; i++)
            if(value < node.value[i] || value == node.value[i]) break;
        if(i == *node.value_num)
            node = GetNode(node.pointer[i].num);
        else if(value == node.value[i])
            node = GetNode(node.pointer[i + 1].num);
        else
            node = GetNode(node.pointer[i].num);
    }
    return node;
}

template <class V, class P>
P BPlusTree<V, P>::Find(V value) {
    Node<V, P> node = FindLeafNode(value);
    for(int i = 0; i < *node.value_num; i++)
        if(node.value[i] == value)
            return node.pointer[i];
    return P();
}

template <class V, class P>
void BPlusTree<V, P>::Insert(V value, P pointer) {
    if(root_ == -1) {
        Node<V, P> node = GetAnAvailableNode();
        *node.state = LEAF;
        *node.value_num = 1;
        node.value[0] = value;
        node.pointer[0] = pointer;
        node.pointer[LAST_POINTER].num = -1;
        root_ = *node.num;
        return;
    }
    Node<V, P> leaf_node = FindLeafNode(value);
    if(*leaf_node.value_num < MAX_VALUE_NUM)
        InsertInLeaf(leaf_node, value, pointer);
    else {
        Node<V, P> node = GetAnAvailableNode();
        *node.state = LEAF;
        for(int i = 0; i < MAX_VALUE_NUM; i++) swapper_.value[i] = leaf_node.value[i];
        for(int i = 0; i < MAX_VALUE_NUM; i++) swapper_.pointer[i] = leaf_node.pointer[i];
        *swapper_.value_num = MAX_VALUE_NUM;
        InsertInLeaf(swapper_, value, pointer);
        node.pointer[LAST_POINTER] = leaf_node.pointer[LAST_POINTER];
        leaf_node.pointer[LAST_POINTER].num = *node.num;
        int mid = (pointer_num_ + 1) / 2;
        for(int i = 0; i < mid; i++) leaf_node.value[i] = swapper_.value[i];
        for(int i = 0; i < mid; i++) leaf_node.pointer[i] = swapper_.pointer[i];
        for(int i = mid; i < pointer_num_; i++) node.value[i - mid] = swapper_.value[i];
        for(int i = mid; i < pointer_num_; i++) node.pointer[i - mid] = swapper_.pointer[i];
        *leaf_node.value_num = mid;
        *node.value_num = pointer_num_ - mid;
        InsertInParent(leaf_node, node.value[0], node);
    }
}

template <class V, class P>
void BPlusTree<V, P>::InsertInLeaf(Node<V, P> node, V value, P pointer) {
    if(value < node.value[0]) {
        for(int i = *node.value_num - 1; i >= 0; i--) {
            node.value[i + 1] = node.value[i];
            node.pointer[i + 1] = node.pointer[i];
        }
        node.value[0] = value;
        node.pointer[0] = pointer;
        (*node.value_num)++;
        return;
    }
    for(int i = *node.value_num - 1; i >= 0; i--)
        if(node.value[i] < value) {
            for(int j = *node.value_num - 1; j >= i + 1; j--) {
                node.value[j + 1] = node.value[j];
                node.pointer[j + 1] = node.pointer[j];
            }
            node.value[i + 1] = value;
            node.pointer[i + 1] = pointer;
            (*node.value_num)++;
            return;
        }
}

template <class V, class P>
void BPlusTree<V, P>::InsertInNonleaf(Node<V, P> node, int pointer_left_num, V value, int pointer_right_num) {
    for(int i = 0; i <= *node.value_num; i++)
        if(node.pointer[i].num == pointer_left_num) {
            (*node.value_num)++;
            for(int j = *node.value_num; j > i + 1; j--) {
                node.pointer[j] = node.pointer[j - 1];
                node.value[j - 1] = node.value[j - 2];
            }
            node.pointer[i + 1].num = pointer_right_num;
            node.value[i] = value;
            return;
        }
}

template <class V, class P>
void BPlusTree<V, P>::InsertInParent(Node<V, P> node_left, V value, Node<V, P> node_right) {
    if(*node_left.num == root_) {
        Node<V, P> node = GetAnAvailableNode();
        *node.state = NONLEAF;
        *node.value_num = 1;
        node.pointer[0].num = *node_left.num;
        node.value[0] = value;
        node.pointer[1].num = *node_right.num;
        root_ = *node.num;
        return;
    }
    Node<V, P> parent_node = GetNode(queue_.back());
    queue_.pop_back();
    if(*parent_node.value_num < MAX_VALUE_NUM)
        InsertInNonleaf(parent_node, *node_left.num, value, *node_right.num);
    else {
        Node<V, P> node = GetAnAvailableNode();
        *node.state = NONLEAF;
        for(int i = 0; i < MAX_VALUE_NUM; i++) swapper_.value[i] = parent_node.value[i];
        for(int i = 0; i < pointer_num_; i++) swapper_.pointer[i] = parent_node.pointer[i];
        *swapper_.value_num = MAX_VALUE_NUM;
        InsertInNonleaf(swapper_, *node_left.num, value, *node_right.num);
        int mid = pointer_num_ / 2;
        for(int i = 0; i < mid; i++) parent_node.value[i] = swapper_.value[i];
        for(int i = 0; i < mid + 1; i++) parent_node.pointer[i] = swapper_.pointer[i];
        for(int i = mid + 1; i < pointer_num_; i++) node.value[i - mid - 1] = swapper_.value[i];
        for(int i = mid + 1; i < pointer_num_ + 1; i++) node.pointer[i - mid - 1] = swapper_.pointer[i];
        *parent_node.value_num = mid;
        *node.value_num = pointer_num_ - mid - 1;
        InsertInParent(parent_node, swapper_.value[mid], node);
    }
}

template <class V, class P>
void BPlusTree<V, P>::Delete(V value) {
    Node<V, P> node = FindLeafNode(value);
    if(!node.num) {
        std::cerr <<  " The tree is empty. Nothing to delete." << std::endl;
        return;
    }
    DeleteEntry(node, value);
}

template <class V, class P>
void BPlusTree<V, P>::DeleteEntry(Node<V, P> node, V value) {
    DeleteInNode(node, value);
    if(*node.num == root_) {
        if(*node.value_num == 0) {
            if(*node.state == LEAF)
                root_ = -1;
            else
                root_ = node.pointer[0].num;
            *node.state = EMPTY;
        }
        return;
    }
    if(*node.state == LEAF) {
        if(*node.value_num < pointer_num_ / 2) {
            Node<V, P> sibling_node;
            V seperator;
            bool is_predecessor = GetSiblingAndSeperator(GetNode(queue_.back()), *node.num, sibling_node, seperator);
            if(*sibling_node.value_num + *node.value_num <= MAX_VALUE_NUM) {
                if(!is_predecessor) {
                    Node<V, P> temp;
                    temp = node;
                    node = sibling_node;
                    sibling_node = temp;
                }
                for(int i = 0; i < *node.value_num; i++) {
                    sibling_node.pointer[*sibling_node.value_num + i] = node.pointer[i];
                    sibling_node.value[*sibling_node.value_num + i] = node.value[i];
                }
                *sibling_node.value_num += *node.value_num;
                sibling_node.pointer[LAST_POINTER] = node.pointer[LAST_POINTER];
                Node<V, P> parent = GetNode(queue_.back());
                queue_.pop_back();
                DeleteEntry(parent, seperator);
                *node.state = EMPTY;
                empty_node_num_++;
                return;
            } else {
                if(is_predecessor) {
                    for(int i = *node.value_num; i > 0; i--) {
                        node.pointer[i] = node.pointer[i - 1];
                        node.value[i] = node.value[i - 1];
                    }
                    node.pointer[0] = sibling_node.pointer[*sibling_node.value_num - 1];
                    node.value[0] = sibling_node.value[*sibling_node.value_num - 1];
                    (*node.value_num)++;
                    (*sibling_node.value_num)--;
                    ReplaceSeperator(GetNode(queue_.back()), seperator, node.value[0]);
                } else {
                    node.pointer[*node.value_num] = sibling_node.pointer[0];
                    node.value[*node.value_num] = sibling_node.value[0];
                    (*node.value_num)++;
                    for(int i = 0; i < *sibling_node.value_num - 1; i++) {
                        sibling_node.pointer[i] = sibling_node.pointer[i + 1];
                        sibling_node.value[i] = sibling_node.value[i + 1];
                    }
                    (*sibling_node.value_num)--;
                    ReplaceSeperator(GetNode(queue_.back()), seperator, sibling_node.value[0]);
                }
            }
        }
    } else if(*node.state == NONLEAF) {
        if(*node.value_num < (pointer_num_ - 1) / 2) {
            Node<V, P> sibling_node;
            V seperator;
            bool is_predecessor = GetSiblingAndSeperator(GetNode(queue_.back()), *node.num, sibling_node, seperator);
            if(*sibling_node.value_num + *node.value_num < MAX_VALUE_NUM) {
                if(!is_predecessor) {
                    Node<V, P> temp;
                    temp = node;
                    node = sibling_node;
                    sibling_node = temp;
                }
                sibling_node.value[*sibling_node.value_num] = seperator;
                (*sibling_node.value_num)++;
                for(int i = 0; i < *node.value_num; i++) {
                    sibling_node.pointer[*sibling_node.value_num + i] = node.pointer[i];
                    sibling_node.value[*sibling_node.value_num + i] = node.value[i];
                }
                *sibling_node.value_num += *node.value_num;
                sibling_node.pointer[*sibling_node.value_num] = node.pointer[*node.value_num];
                Node<V, P> parent = GetNode(queue_.back());
                queue_.pop_back();
                DeleteEntry(parent, seperator);
                *node.state = EMPTY;
                empty_node_num_++;
                return;
            } else {
                if(is_predecessor) {
                    node.pointer[*node.value_num + 1] = node.pointer[*node.value_num];
                    for(int i = *node.value_num; i > 0; i--) {
                        node.pointer[i] = node.pointer[i - 1];
                        node.value[i] = node.value[i - 1];
                    }
                    node.pointer[0] = sibling_node.pointer[*sibling_node.value_num];
                    node.value[0] = seperator;
                    (*node.value_num)++;
                    (*sibling_node.value_num)--;
                    ReplaceSeperator(GetNode(queue_.back()), seperator, sibling_node.value[*sibling_node.value_num]);
                } else {
                    ReplaceSeperator(GetNode(queue_.back()), seperator, sibling_node.value[0]);
                    node.pointer[*node.value_num + 1] = sibling_node.pointer[0];
                    node.value[*node.value_num] = seperator;
                    (*node.value_num)++;
                    for(int i = 0; i < *sibling_node.value_num - 1; i++) {
                        sibling_node.pointer[i] = sibling_node.pointer[i + 1];
                        sibling_node.value[i] = sibling_node.value[i + 1];
                    }
                    (*sibling_node.value_num)--;
                    sibling_node.pointer[*sibling_node.value_num] = sibling_node.pointer[*sibling_node.value_num + 1];
                }
            }
        }
    }
}

template <class V, class P>
void BPlusTree<V, P>::DeleteInNode(Node<V, P> node, V value) {
    for(int i = 0; i < *node.value_num; i++)
        if(node.value[i] == value) {
            for(int j = i; j < *node.value_num - 1; j++) {
                if(*node.state == LEAF)
                    node.pointer[j] = node.pointer[j + 1];
                else
                    node.pointer[j + 1] = node.pointer[j + 2];
                node.value[j] = node.value[j + 1];
            }
            (*node.value_num)--;
            return;
        }
}

template <class V, class P>
bool BPlusTree<V, P>::GetSiblingAndSeperator(Node<V, P> node, P pointer, Node<V, P> &sibling_node, V &seperator) {
    int i;
    for(i = 0; i <= *node.value_num; i++)
        if(node.pointer[i] == pointer) break;
    if(i == *node.value_num) {
        sibling_node = GetNode(node.pointer[i - 1].num);
        seperator = node.value[i - 1];
        return true;
    } else {
        sibling_node = GetNode(node.pointer[i + 1].num);
        seperator = node.value[i];
        return false;
    }
}

template <class V, class P>
void BPlusTree<V, P>::ReplaceSeperator(Node<V, P> node, V value_old, V value_new) {
    for(int i = 0; i < *node.value_num; i++)
        if(node.value[i] == value_old) {
            node.value[i] = value_new;
            return;
        }
}

#endif // B_PLUS_TREE_HPP
