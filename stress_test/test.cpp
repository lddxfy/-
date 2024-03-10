#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <functional>
#include "../skiplist.h"
#include <string>
using namespace std;

const int NUM_THREADS = 1;
const int TEST_COUNT = 100000;
SkipList<int, string> test_skip_list(18);
// 梅森旋转算法（Mersenne twister）是一个伪随机数发生算法。
int GetRandomNumber(mt19937 &gen)
{
    // 用于生成随机数
    uniform_int_distribution<int> distribute(0, 99);
    return distribute(gen);
}

void InsertElement_test(int tid, mt19937 &gen)
{
    cout << tid << endl;
    int thread_count = TEST_COUNT / NUM_THREADS;
    for (int i = tid * thread_count, count = 0; count < thread_count; i++)
    {
        count++;
        test_skip_list.insert_element(GetRandomNumber(gen) % TEST_COUNT, "cjq");
    }
}

void TestGetElement(int tid, std::mt19937 &gen)
{
    cout << tid << endl;
    int thread_count = TEST_COUNT / NUM_THREADS;
    for (int i = tid * thread_count, count = 0; count < thread_count; i++)
    {
        count++;
        test_skip_list.search_element(GetRandomNumber(gen) % TEST_COUNT);
    }
}

int main()
{
    // 用于生成真随机数的种子
    unsigned seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    int res;

    vector<thread> threads(NUM_THREADS);
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; i++)
    {
        cout << "main() : creating thread, " << i << endl;
        threads.emplace_back(InsertElement_test,i,ref(gen));
    }

    for(auto& thread : threads){
        if(thread.joinable()){
            thread.join();
        }
    }

    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "插入时间:" << elapsed.count() << endl;
    cout << "插入元素测试结束"  << endl;
}
