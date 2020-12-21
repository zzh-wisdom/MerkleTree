#ifndef MERKLE_TREE_BLOOM_FILTER_H_
#define MERKLE_TREE_BLOOM_FILTER_H_

#include <string>

using namespace std;

class BloomFilter {
public:
    /**
     * @brief Construct a new Bloom Filter object
     * 
     * @param n key 的个数
     * @param p 误判率
     */
    BloomFilter(int n, double p);

    /**
     * @brief 插入一条记录到bloomfilter
     * 
     * @param keys 
     */
    void AddKey(const string& keys);

    /**
     * @brief 根据Bloom Filter查找Key
     * 
     * @param key 
     * @return true 
     * @return false 
     */
    bool KeyMayMatch(const string& key);

    /**
     * @brief 将另外一个BloomFilter合并到自身
     * 
     * @pre
     * 
     * 
     * @param bf 
     */
    void Merge(BloomFilter *bf);
private:
    int k_; /// hash函数个数
    int m_; /// bitset长度
    string bitset_;
};

#endif