//
// Created by nikita on 28.12.2020.
//

#ifndef INC_6_NETWORK_NODS_BINTREE_HPP
#define INC_6_NETWORK_NODS_BINTREE_HPP
#include <iostream>
#include <vector>

class BinTree {
private:
    struct Node{
    Node(int id_): id(id_){}
    int id;
    int time = 0;
    Node *left = nullptr;
    Node *right = nullptr;
    };

    Node *head = nullptr;
public:
public:
    BinTree() = default;
    ~BinTree(){
        this->destroy(this->head);
    }

    std::vector<int> get_all_nodes(Node* node=nullptr) {
        std::vector<int> result;
        if (node == nullptr)
            node = this->head;
        this->all_nodes(node, result);
        return result;
    }

    void add_time(){
        add_time_inside(this->head);
    }

    void add_time_inside(Node* node){
        if(node == nullptr){
            return;
        }
        else{
            node->time++;
        }
        add_time_inside(node->left);
        add_time_inside(node->right);
    }

    std::vector<int> unable_nodes(int time){
        std::vector<int> res;
        unable_nodes_inside(this->head, res, time);
        return res;
    }

    void reset(int id){
        reset_inside(this->head, id);
    }

    void reset_inside(Node* node, int id){
        if(node == nullptr){
            return;
        }
        else if(node->id == id){
            node->time = 0;
            return;
        }
        else if(id > node->id) {
            reset_inside(node->right, id);
        }
        else if(id < node->id){
            reset_inside(node->left, id);
        }
    }

    void unable_nodes_inside(Node* node, std::vector<int>&res, int time){
        if(node == nullptr){
            return;
        }
        if(node->time > 4 * time){
            res.push_back(node->id);
        }
        else{
            return;
        }
        unable_nodes_inside(node->left, res, time);
        unable_nodes_inside(node->right, res, time);
    }
    std::vector<int> get_path_to(int& id, Node* node=nullptr) {
        std::vector<int> path;
        if (node == nullptr)
            node = this->head;
        this->find_path(node, id, path);
        return path;
    }

    bool contains(int& id) const{
        Node* tmp = this->head;
        while(tmp != nullptr){
            if(tmp->id == id)
                break;
            else if(id > tmp->id)
                tmp = tmp->right;
            else if(id < tmp->id)
                tmp = tmp->right;
        }
        return tmp != nullptr;
    }

    void insert(int& id){
        if(this->head == nullptr){
            this->head = new Node(id);
            return;
        }
        Node* tmp = this->head;
        while(tmp != nullptr){
            if(tmp->id == id)
                break;
            else if(id < tmp->id){
                if(tmp->left == nullptr){
                    tmp->left = new Node(id);
                    break;
                }
                tmp = tmp->left;
            }
            else if(id > tmp->id){
                if(tmp->right == nullptr){
                    tmp->right = new Node(id);
                    break;
                }
                tmp = tmp->right;
            }
        }
    }

    void erase(int& id){
        Node* prev = nullptr;
        Node* tmp = this->head;
        while(tmp != nullptr){
            if (id == tmp->id) {
                if (prev == nullptr) {
                    this->head = nullptr;
                } else {
                    if (prev->left == tmp) {
                        prev->left = nullptr;
                    } else {
                        prev->right = nullptr;
                    }
                }
                destroy(tmp);
            } else if(id < tmp->id) {
                prev = tmp;
                tmp = tmp->left;
            } else if(id > tmp->id) {
                prev = tmp;
                tmp = tmp->right;
            }
        }
    }

    void all_nodes(Node* node, std::vector<int>& vec) const{
        if(node == nullptr)
            return;
        this->all_nodes(node->left, vec);
        vec.push_back(node->id);
        this->all_nodes(node->right, vec);
    }

    void find_path(Node* node, int& id, std::vector<int>& path) {
        while(node != nullptr){
            path.push_back(node->id);
            if(node->id == id)
                break;
            else if(id > node->id)
                node = node->right;
            else if(id < node->id)
                node = node->left;
        }
    }

    void destroy(Node* node){
        if(node == nullptr)
            return;
        destroy(node->right);
        destroy(node->left);
        delete node;
    }
};
#endif //INC_6_NETWORK_NODS_BINTREE_HPP
