#ifndef MERKLE_TREE_HASH_H_
#define MERKLE_TREE_HASH_H_

#include<cstdint>
#include<string>
using namespace std;

uint32_t Hash(const char* data, int n, uint32_t seed);

uint32_t BloomHash(const char* data, int n);

string HashWithSeed0(string data);

string SHA256(string data);

#endif
