#include<openssl/sha.h>
#include<string>
#include<iostream>

#include "../src/merkletree.h"
#include "../util/hash.h"
#include "../util/coding.h"

using namespace std;

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

    return 0;
}