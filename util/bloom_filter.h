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
     * @brief 获取bitset的长度，单位为byte
     * 
     * @return int 
     */
    int GetBitsetLen() {
      return bitset_.size();
    }
private:
    int k_; /// hash函数个数
    int m_; /// bitset长度
    string bitset_;
};

#endif