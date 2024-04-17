#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <ranges>
#include "Definition.hpp"




template <typename I>
class LZTrie {
public:
    struct Node {
        Node(size_t p_code): code(p_code) {}
        std::unordered_map<I, std::shared_ptr<Node>> children;
        size_t code;
    };

    std::shared_ptr<Node> m_root;
    std::shared_ptr<Node> m_current;
    int64_t m_last_code = -1;

    LZTrie(): m_root(std::make_shared<Node>(m_last_code)), m_current(m_root) {}

    ~LZTrie() = default;

    std::shared_ptr<Node> traverse(const I c) {
        if (m_current->children.find(c) == m_current->children.end()) {
            return nullptr;
        }
        m_current = m_current->children[c];
        return m_current;
    }

    void insert_back(const I c) {
        m_last_code++;
        m_current->children[c] = std::make_shared<Node>(m_last_code);
    }

    void reset_traversal() {
        m_current = m_root;
    }

    void reset_trie() {
        m_last_code = 0;
        m_root = std::make_shared<Node>(m_last_code);       
        m_current = m_root;
    }
};