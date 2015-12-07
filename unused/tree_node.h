//
// Created by Evgeny Tanhilevich on 22/11/15.
//

#ifndef COOL_COMPILER_TREE_NODE_H
#define COOL_COMPILER_TREE_NODE_H

#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::make_shared;
using std::make_unique;

class IncorrectParentException{};


template <class T>
class TreeNode: public std::enable_shared_from_this<TreeNode<T>> {
public:
    typedef shared_ptr<T> PData;
    typedef shared_ptr<TreeNode<T>> PNode;
private:
    PData m_pData;
    PNode m_pParent;
    vector<PNode> m_pChildren;
public:

    TreeNode(const TreeNode&) = delete;
    TreeNode(TreeNode&&) = default;

    TreeNode(PData _pData) :
            m_pData(_pData),
            m_pParent()
    {}
    TreeNode(PData _pData, PNode _pParent) :
            m_pData(_pData),
            m_pParent(_pParent)
    {}
    void add_child(PNode _pChild){
        if (!_pChild->m_pParent) {
            _pChild->m_pParent = this->shared_from_this();
        } else if (_pChild->m_pParent.get() != this) {
            throw IncorrectParentException();
        }
        m_pChildren.push_back(_pChild);
    }
    void traverse_top(void(f)(PData)){
        f(m_pData);
        for (auto& child: m_pChildren) {
            child->traverse_top(f);
        }
    }
    bool traverse_top_if_true(bool(f)(PData)){
        bool b;
        if ((b = f(m_pData))) {
            for (auto& child: m_pChildren) {
                child->traverse_top_if_true(f);
            }
        }
        return b;
    }
    void traverse_top_bottom(void(before)(PData), void(after)(PData)){
        before(m_pData);
        for (auto& child: m_pChildren) {
            child->traverse_top_bottom(before, after);
        }
        after(m_pData);
    }

    PNode get_parent() { return m_pParent; }
    PData get_data() { return m_pData; }
};



#endif //COOL_COMPILER_TREE_NODE_H
