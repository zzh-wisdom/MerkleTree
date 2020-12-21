#include<math.h>
#include"bloom_filter.h"
#include"hash.h"

/**
 * @brief 估计bloom filter需要的hash函数个数
 * 
 * @param m bitset 的大小
 * @param n key 的个数
 * @return int 
 */
int EstimateHashFunNum(int m, int n) {
    return (int)ceil(log(2) * (double)m / (double)(n));
}

/**
 * @brief 估计bloom filter的 bitset 大小
 * 
 * @param n key 的个数
 * @param p 误判率
 * @return int 
 */
int EstimateBitSetSize(int n, double p) {
    return (int)ceil(-1 * (double)n * log(p) / pow(log(2), 2));
}

BloomFilter::BloomFilter(int n, double p) {
    m_ = EstimateBitSetSize(n, p);
    k_ = EstimateHashFunNum(m_, n);
    int bits = m_;
    if (bits < 64) bits = 64;
    int bytes = (bits + 7) / 8;  // 向上取整到字节
    // printf("bytes:%d\n", bytes);
    bitset_.resize(bytes);
    // printf("m_:%d, k_:%d, bitset bytes:%lu\n", m_, k_, bitset_.size());
}

void BloomFilter::AddKey(const string& keys) {
    char* array = &bitset_[0];
    uint32_t bits = bitset_.size() * 8;
    uint32_t h = BloomHash(keys.data(), keys.size());
    const uint32_t delta = (h >> 17) | (h << 15);  // Rotate right 17 bits
    for (size_t j = 0; j < k_; j++) {
        const uint32_t bitpos = h % bits;
        array[bitpos / 8] |= (1 << (bitpos % 8));
        h += delta;
    }
}

bool BloomFilter::KeyMayMatch(const string& key) {
    int len = bitset_.size();
    if (len < 2) return false;

    const char* array = bitset_.data();
    const size_t bits = len * 8;

    uint32_t h = BloomHash(key.data(), key.size());
    const uint32_t delta = (h >> 17) | (h << 15);  // Rotate right 17 bits
    for (size_t j = 0; j < k_; j++) {
      const uint32_t bitpos = h % bits;
      if ((array[bitpos / 8] & (1 << (bitpos % 8))) == 0) return false;
      h += delta;
    }
    return true;
}
