#include <iostream>
#include <map>
using namespace std;

// LRU 
// 取数的时候，使用map
// 存数的时候，key在就改变数值移动到头，key不在就删除尾部并添加新值到头

struct ListNode {
    ListNode():key(0),val(0),head(nullptr),tail(nullptr) {}
    ListNode(int key, int value):key(key),val(value),head(nullptr),tail(nullptr) {}

    ListNode *head;
    int key;
    int val;
    ListNode *tail;
};

class LRUCache {
 public:
    ~LRUCache() {
        while (head_ != nullptr)
        {
            ListNode *tmp = head_;
            delete tmp;
            head_ = head_->tail;
        }
    }
    LRUCache(int capacity) {
        capacity_ = capacity;
    }
    int get(int key) {
        map<int, ListNode*>::iterator it = cache_mp_.find(key);
        if (it != cache_mp_.end()) {
            ListNode *tmp = it->second;
            DeleteNode(tmp);
            InsertHead(tmp);
            return tmp->val;
        }
        return -1;
    }
    void InsertHead(ListNode *now) {
        now->tail = head_;
        now->head = nullptr;
        if (head_ != nullptr) head_->head = now;
        head_ = now;
        if (tail_ == nullptr) tail_ = head_;
    }
    void DeleteNode(ListNode *node) {
        if (node->head == nullptr) {head_ = node->tail;}
        else node->head->tail = node->tail;
        if(node->tail == nullptr) {tail_ = node->head;}
        else node->tail->head = node->head;
    }
    void put(int key, int value) {
        map<int, ListNode*>::iterator it = cache_mp_.find(key);
        if (it != cache_mp_.end()) {
            ListNode *tmp = it->second;
            DeleteNode(tmp);
            InsertHead(tmp);
        } else {
            ListNode *tmp = tail_;
            if (cache_mp_.size() >= capacity_) {
                cache_mp_.erase(tail_->key);
                DeleteNode(tail_);
                tmp->key = key;
                tmp->val = value;
                // tmp->head = nullptr;
                // tmp->tail = nullptr;
            } else {
                tmp = new ListNode(key, value);
            }
            InsertHead(tmp);
            cache_mp_[key] = tmp;
        }
    }

    void showLRU() {
        ListNode *tmp = head_;
        cout << "LRU:";
        while (tmp != nullptr)
        {
            cout << "[" << tmp->key << "," << tmp->val << "],";
            tmp = tmp->tail;
        }
        cout << endl;
        
    }

 public:
    int capacity_ = 0;
    int now_length_ = 0;
    ListNode* head_ = nullptr;
    ListNode* tail_ = nullptr;
    map<int, ListNode*> cache_mp_;
};

int main() {
    LRUCache lru(2);
    lru.put(1,1);
    lru.showLRU();
    lru.put(2,2);
    lru.showLRU();
    cout << lru.get(1) << endl;
    lru.put(3,3);
    lru.showLRU();
    cout << lru.get(2) << endl;
    lru.put(4,4);
    lru.showLRU();
    cout << lru.get(1) << endl;
    cout << lru.get(3) << endl;
    cout << lru.get(4) << endl;
    return 0;
}