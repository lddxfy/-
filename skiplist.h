#include <iostream>
#include <mutex>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>
#include <time.h>
using namespace std;

mutex mtx; // 互斥锁
const char *STORE_ADDRESS = "store/storefile";
const string delimiter = ":";

template <typename K, typename V>
class Node
{
public:
    Node(){};
    Node(K k, V v, int level) : key(k), value(v), node_level(level), forward(level, nullptr){};
    ~Node(){};
    K getKey() const { return this->key; };
    V getValue() const { return this->value; };
    void setValue(V v) { this->value = v; };
    // 数组保存指向不同级别的下一个节点的指针
    vector<shared_ptr<Node<K, V>>> forward;
    int node_level;

private:
    K key;
    V value;
};

template <typename K, typename V>
class SkipList
{
public:
    SkipList(int);
    ~SkipList();
    int get_random_level() const;
    bool search_element(K) const;
    int insert_element(K, V);
    // void clear(Node<K, V> *cur);
    shared_ptr<Node<K, V>> create_Node(K, V, int);
    void delete_node(K);
    void display_list() const;
    void dump_file();
    void load_file();
    int size() const;

private:
    void getKeyValueFromString(const string &str, string &key, string &value) const;
    bool isValid(const string &str) const;
    int max_level;                 // 最大等级
    int skip_list_level;           // 当前等级
    shared_ptr<Node<K, V>> header; // 头结点
    int element_count;
    ofstream file_write;
    ifstream file_read;
};
template <typename K, typename V>
int SkipList<K, V>::insert_element(K k, V v)
{
    unique_lock<mutex> lck(mtx);
    auto curnode = header;
    // 相当于待插入节点的前一节点
    // 同时也保存了每一层要插入节点的前一位置
    vector<shared_ptr<Node<K, V>>> update(max_level + 1, nullptr);
    for (int i = skip_list_level; i >= 0; i--)
    {
        while (curnode->forward[i] != nullptr && curnode->forward[i]->getKey() < k)
        {
            curnode = curnode->forward[i];
        }

        update[i] = curnode;
    }
    // 要插入位置的下一节点
    curnode = curnode->forward[0];
    // 插入节点已经存在
    if (curnode != nullptr && curnode->getKey() == k)
    {
        cout << "要插入节点" << k << "已经存在" << endl;
        return 1;
    }
    if (curnode == nullptr || curnode->getKey() != k)
    {
        // 获取插入节点的随机级别
        int random_level = get_random_level();
        //cout << random_level << endl;
        if (random_level > skip_list_level)
        {
            for (int i = skip_list_level + 1; i < random_level; i++)
            {
                update[i] = header;
            }
            skip_list_level = random_level - 1;
        }

        auto insert_node = create_Node(k, v, random_level);
        for (int i = 0; i < random_level; i++)
        {
            insert_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insert_node;
        }
        cout << "插入成功,key= " << k << " value= " << v << endl;
        element_count++;
    }
    return 0;
}
// 返回节点个数
template <typename K, typename V>
int SkipList<K, V>::size() const
{
    return this->element_count;
}

template <typename K, typename V>
SkipList<K, V>::SkipList(int maxlevel) : max_level(maxlevel), skip_list_level(0), element_count(0)
{
    header = make_shared<Node<K, V>>(K(), V(), maxlevel);
}

template <typename K, typename V>
SkipList<K, V>::~SkipList()
{
    if (file_write.is_open())
    {
        file_write.close();
    }
    if (file_read.is_open())
    {
        file_read.close();
    }
}

template <typename K, typename V>
bool SkipList<K, V>::search_element(K k) const
{
    auto curnode = header;
    for (int i = skip_list_level; i >= 0; i--)
    {
        while (curnode->forward[i] && curnode->forward[i]->getKey() < k)
        {
            curnode = curnode->forward[i];
        }
    }
    curnode = curnode->forward[0];
    if (curnode && curnode->getKey() == k)
    {
        cout << "找到节点 key: " << k << ", value: " << curnode->getValue() << endl;
        return true;
    }
    cout << "没有找到节点" << k << endl;
    return false;
}

template <typename K, typename V>
void SkipList<K, V>::delete_node(K k)
{
    unique_lock<mutex> lck(mtx);
    vector<shared_ptr<Node<K, V>>> update(max_level + 1, nullptr);
    auto curnode = header;
    for (int i = skip_list_level; i >= 0; i--)
    {
        while (curnode->forward[i] && curnode->forward[i]->getKey() < k)
        {
            curnode = curnode->forward[i];
        }
        update[i] = curnode;
    }
    // 目标节点
    curnode = curnode->forward[0];
    if (curnode && curnode->getKey() == k)
    {

        for (int i = 0; i <= skip_list_level; i++)
        {
            // 前一个节点的下一个节点
            if (update[i]->forward[i] != curnode)
            {
                break;
            }
            update[i]->forward[i] = curnode->forward[i];
        }

        // 删除没有元素的等级
        while (skip_list_level > 0 && header->forward[skip_list_level] == 0)
        {
            skip_list_level--;
        }
        cout << "成功删除元素key: " << k << ", value: " << curnode->getValue() << endl;
        element_count--;
    }
    return;
}

template <typename K, typename V>
shared_ptr<Node<K, V>> SkipList<K, V>::create_Node(K k, V v, int level)
{
    return make_shared<Node<K, V>>(k, v, level);
}

template <typename K, typename V>
void SkipList<K, V>::display_list() const
{
    cout << "*******Skip List********" << endl;
    for (int i = 0; i <= skip_list_level; i++)
    {
        auto curnode = this->header->forward[i];
        cout << "Level:" << i << ":";
        while (curnode != nullptr)
        {
            cout << curnode->getKey() << ":" << curnode->getValue() << ";";
            curnode = curnode->forward[i];
        }
        cout << endl;
    }
}
// 从读取的行中获得key-value
template <typename K, typename V>
void SkipList<K, V>::getKeyValueFromString(const string &str, string &key, string &value) const
{
    if (!isValid(str))
        return;
    key = str.substr(0, str.find(delimiter));
    value = str.substr(str.find(delimiter) + 1, str.length());
}
// 判断读取的字符串是否合法
template <typename K, typename V>
bool SkipList<K, V>::isValid(const string &str) const
{
    if (str.empty())
        return false;
    if (str.find(delimiter) == string::npos)
        return false;
    return true;
}

// 保存数据至指定目录下
template <typename K, typename V>
void SkipList<K, V>::dump_file()
{
    cout << "***********dump_file***********" << endl;
    auto node = this->header->forward[0];
    file_write.open(STORE_ADDRESS);
    while (node != nullptr)
    {
        file_write << node->getKey() << ":" << node->getValue() << endl;
        cout << node->getKey() << ":" << node->getValue() << endl;
        node = node->forward[0];
    }
    cout << "dump_file成功" << endl;
    file_write.flush();
    file_write.close();
    return;
}
// 读取文件数据
template <typename K, typename V>
void SkipList<K, V>::load_file()
{
    cout << "********load_file*********" << endl;
    file_read.open(STORE_ADDRESS);
    string str;
    string key;
    string value;
    while (getline(file_read, str))
    {
        getKeyValueFromString(str, key, value);
        if (key.empty() || value.empty())
            continue;
        insert_element(stoi(key), value);
        cout << "key:" << key << "value:" << value << endl;
    }
    file_read.close();
}

// 有 1/2的概率返回 1、1/4的概率返回 2、1/8的概率返回 3 依此类推....
// 返回1时不建立索引，返回2时建立一级索引，依次类推...
// 当建立二级索引时当时也要建立一级索引，更高级索引也同样如此
template <typename K, typename V>
int SkipList<K, V>::get_random_level() const
{
    int level = 1;
    while (rand() % 2)
    {
        level++;
    }
    level = level > max_level ? max_level : level;
    return level;
};
