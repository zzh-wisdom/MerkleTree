#include<openssl/sha.h>
#include<string>
#include<math.h>
#include<iostream>

#include "../src/merkle_tree.h"
#include "../util/hash.h"
#include "../util/coding.h"

using namespace std;

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

int main() {
    string tmp;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
///////////////
    tmp = "apple apple";
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, tmp.data(), tmp.size());
    SHA256_Final(hash, &sha256);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
    string hash1 = SHA256(tmp);
    printf("%s\n", HashToHexStr(hash1).c_str());

    tmp = "pear pear";
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, tmp.data(), tmp.size());
    SHA256_Final(hash, &sha256);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
    string hash2 = SHA256(tmp);
    printf("%s\n", HashToHexStr(hash2).c_str());

    tmp = hash1 + hash2;
    string hash1_2 = SHA256(tmp);
    printf("\n%s\n", HashToHexStr(hash1_2).c_str());

    int n = 10000;
    double p = 0.01;
    int m = EstimateBitSetSize(n, p);
    int k = EstimateHashFunNum(m, n);
    printf("m:%d, k:%d\n", m, k);
    
    return 0;
}