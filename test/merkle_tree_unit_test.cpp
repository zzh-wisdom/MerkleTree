#include<iostream>
#include<utility>
#include <assert.h>

#include "contents.h"
#include "../src/merkle_tree.h"
#include "../util/hash.h"
#include "../util/coding.h"

using namespace std;

pair<int, string> Contents0[3] = {
    {0, "apple apple"}, {1, "pear pear"}, {2, "orange orange"},
};

pair<int, string> Contents1[4] = {
    {0, "apple apple"}, {1, "pear pear"}, {2, "orange orange"}, {3, "banana banana"},
};

pair<int, string> Contents2[5] = {
    {0, "apple apple"}, {1, "pear pear"}, {2, "orange orange"}, {3, "banana banana"}, {4, "lemon lemon"},
};

vector<SimpleContent*> BuildSimpleContents(pair<int, string>* contents, int n) {
    vector<SimpleContent*> v;
    for(int i=0; i<n; i++) {
        v.push_back(new SimpleContent(contents[i].first, contents[i].second));
    }
    return v;
}


void DestroySimpleContents(vector<Content*> v) {
    for(int i=0; i<v.size(); i++) {
        if(v[i] != nullptr) {
            SimpleContent* tmp = static_cast<SimpleContent*>(v[i]);
            delete tmp;
            v[i] = nullptr;
        }
    }
}

vector<Content*> BuildSHA256Contents(pair<int, string>* contents, int n) {
    vector<Content*> v;
    for(int i=0; i<n; i++) {
        v.push_back(new SHA256Content(contents[i].first, contents[i].second));
    }
    return v;
}

void DestroySHA256Contents(vector<Content*> v) {
    for(int i=0; i<v.size(); i++) {
        if(v[i] != nullptr) {
            SHA256Content* tmp = static_cast<SHA256Content*>(v[i]);
            delete tmp;
            v[i] = nullptr;
        }
    }
}

string CalculateRootHash(vector<Content*>& v, HashFun hash_fun) {
    switch (v.size())
    {
    case 3:{
        string tmp;

        string hash0_0 = v[0]->CalculateHash();
        string hash0_1 = v[1]->CalculateHash();
        string hash0_2 = v[2]->CalculateHash();

        tmp = hash0_0 + hash0_1;
        string hash1_0 = hash_fun(tmp);
        tmp = hash0_2 + hash0_2;
        string hash1_1 = hash_fun(tmp);

        tmp = hash1_0 + hash1_1;
        string hash2_0 = hash_fun(tmp);
        // printf("%s\n"
        //        "  %s\n"
        //        "    %s\n"
        //        "    %s\n"
        //        "  %s\n"
        //        "    %s\n"
        //        "    %s\n"
        //        , HashToHexStr(hash2_0).c_str(), HashToHexStr(hash1_0).c_str()
        //        , HashToHexStr(hash0_0).c_str(), HashToHexStr(hash0_1).c_str()
        //        , HashToHexStr(hash1_1).c_str(), HashToHexStr(hash0_2).c_str()
        //        , HashToHexStr(hash0_2).c_str());
        return hash2_0;
        break;
    } 
    case 4:{
        string tmp;

        string hash0_0 = v[0]->CalculateHash();
        string hash0_1 = v[1]->CalculateHash();
        string hash0_2 = v[2]->CalculateHash();
        string hash0_3 = v[3]->CalculateHash();
        
        tmp = hash0_0 + hash0_1;
        string hash1_0 = hash_fun(tmp);
        tmp = hash0_2 + hash0_3;
        string hash1_1 = hash_fun(tmp);

        tmp = hash1_0 + hash1_1;
        string hash2_0 = hash_fun(tmp);
        return hash2_0;
        break;
    }
    case 5:{
        string tmp;

        string hash0_0 = v[0]->CalculateHash();
        string hash0_1 = v[1]->CalculateHash();
        string hash0_2 = v[2]->CalculateHash();
        string hash0_3 = v[3]->CalculateHash();
        string hash0_4 = v[4]->CalculateHash();

        tmp = hash0_0 + hash0_1;
        string hash1_0 = hash_fun(tmp);
        tmp = hash0_2 + hash0_3;
        string hash1_1 = hash_fun(tmp);
        tmp = hash0_4 + hash0_4;
        string hash1_2 = hash_fun(tmp);

        tmp = hash1_0 + hash1_1;
        string hash2_0 = hash_fun(tmp);
        tmp = hash1_2 + hash1_2;
        string hash2_1 = hash_fun(tmp);

        tmp = hash2_0 + hash2_1;
        string hash3_0 = hash_fun(tmp);

        // printf("%s\n", HashToHexStr(hash3_0).c_str());
        // printf("  %s\n", HashToHexStr(hash2_0).c_str());
        // printf("    %s\n", HashToHexStr(hash1_0).c_str());
        // printf("      %s\n", HashToHexStr(hash0_0).c_str());
        // printf("      %s\n", HashToHexStr(hash0_1).c_str());
        // printf("    %s\n", HashToHexStr(hash1_1).c_str());
        // printf("      %s\n", HashToHexStr(hash0_2).c_str());
        // printf("      %s\n", HashToHexStr(hash0_3).c_str());
        // printf("  %s\n", HashToHexStr(hash2_1).c_str());
        // printf("    %s\n", HashToHexStr(hash1_2).c_str());
        // printf("      %s\n", HashToHexStr(hash0_4).c_str());
        // printf("      %s\n", HashToHexStr(hash0_4).c_str());
        // printf("    %s\n", HashToHexStr(hash1_2).c_str());
        // printf("      %s\n", HashToHexStr(hash0_4).c_str());
        // printf("      %s\n", HashToHexStr(hash0_4).c_str());

        return hash3_0;
        break;
    }
    default:
        break;
    };

    return "";
}

bool TestNewMerkleTreeWithDefaultHash() {
    printf("TestNewMerkleTreeWithDefaultHash..\n");
    MerkleTree *tree = nullptr;
    bool is_ok = true;

    if(is_ok) {
        vector<Content*> v0 = BuildSHA256Contents(Contents0, 3);
        string expected_hash = CalculateRootHash(v0, SHA256);
        tree = new MerkleTree(v0);
        // printf("%s\n", tree->DebugString().data());
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents0 --- fail.\n");
        }
        delete tree;
    }
    if(is_ok) {
        vector<Content*> v1 = BuildSHA256Contents(Contents1, 4);
        string expected_hash = CalculateRootHash(v1, SHA256);
        tree = new MerkleTree(v1);
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents1 --- fail.\n");
        }
        delete tree;
    }
    if(is_ok) {
        vector<Content*> v2 = BuildSHA256Contents(Contents2, 5);
        string expected_hash = CalculateRootHash(v2, SHA256);
        tree = new MerkleTree(v2);
        // printf("%s\n", tree->DebugString().data());
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents2 --- fail.\n");
        }
        delete tree;
    }

    if(is_ok) {
        printf("=== ok.\n");
    }
    else {
        printf("=== fail.\n");
    }
    return is_ok;
}

bool TestNewMerkleTreeWithSpecifiedHash() {
    printf("TestNewMerkleTreeWithSpecifiedHash..\n");
    MerkleTree *tree = nullptr;
    bool is_ok = true;

    if(is_ok) {
        vector<Content*> v0 = BuildSHA256Contents(Contents0, 3);
        string expected_hash = CalculateRootHash(v0, HashWithSeed0);
        tree = new MerkleTree(v0, HashWithSeed0);
        // printf("%s\n", tree->DebugString().data());
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents0 --- fail.\n");
        }
        delete tree;
    }
    if(is_ok) {
        vector<Content*> v1 = BuildSHA256Contents(Contents1, 4);
        string expected_hash = CalculateRootHash(v1, HashWithSeed0);
        tree = new MerkleTree(v1, HashWithSeed0);
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents1 --- fail.\n");
        }
        delete tree;
    }
    if(is_ok) {
        vector<Content*> v2 = BuildSHA256Contents(Contents2, 5);
        string expected_hash = CalculateRootHash(v2, HashWithSeed0);
        tree = new MerkleTree(v2, HashWithSeed0);
        if(expected_hash != tree->GetRootHash()) {
            is_ok = false;
            printf("Contents2 --- fail.\n");
        }
        delete tree;
    }

    if(is_ok) {
        printf("=== ok.\n");
    }
    else {
        printf("=== fail.\n");
    }
    return is_ok;
}

bool TestTreeProof() {
    printf("TestTreeProof..\n");
    MerkleTree *tree = nullptr;
    bool is_ok = true;
    vector<Content*> error_contents;
    SHA256Content* content_tmp;
    vector<Content*> v;

    v = BuildSHA256Contents(Contents0, 3);
    tree = new MerkleTree(v);
    content_tmp = static_cast<SHA256Content*>(v.back());
    if(!(tree->TreeProof(&error_contents) && error_contents.size() == 0)) {
        printf("Contents0 TreeProof step1 fail.\n");
        goto Fail;
    }
    content_tmp->data_[0] += 1;
    if(!( !tree->TreeProof(&error_contents) &&
        error_contents.size() == 1 &&
        error_contents[0]->GetNum() == 2)) {
        printf("Contents0 TreeProof step2 fail.\n");
        goto Fail;
    }
    delete tree;

    v = BuildSHA256Contents(Contents1, 4);
    tree = new MerkleTree(v);
    content_tmp = static_cast<SHA256Content*>(v.back());
    error_contents.clear();
    if(!(tree->TreeProof(&error_contents) && error_contents.size() == 0)) {
        printf("Contents1 TreeProof step1 fail.\n");
        goto Fail;
    }
    content_tmp->data_[0] += 1;
    if(!( !tree->TreeProof(&error_contents) &&
        error_contents.size() == 1 &&
        error_contents[0]->GetNum() == 3)) {
        printf("Contents1 TreeProof step2 fail.\n");
        goto Fail;
    }
    delete tree;

    v = BuildSHA256Contents(Contents2, 5);
    tree = new MerkleTree(v);
    content_tmp = static_cast<SHA256Content*>(v.back());
    error_contents.clear();
    if(!(tree->TreeProof(&error_contents) && error_contents.size() == 0)) {
        printf("Contents2 TreeProof step1 fail.\n");
        goto Fail;
    }
    content_tmp->data_[0] += 1;
    if(!( !tree->TreeProof(&error_contents) &&
        error_contents.size() == 1 &&
        error_contents[0]->GetNum() == 4)) {
        printf("Contents2 TreeProof step2 fail.\n");
        goto Fail;
    }
    delete tree;
    
    printf("=== ok.\n");
    return true;

Fail:
    delete tree;
    printf("=== fail.\n");
    return false;
}

bool TestContentProof() {
    printf("TestContentProof..\n");
    vector<Content*> v = BuildSHA256Contents(Contents2, 5);
    MerkleTree* tree = new MerkleTree(v);
    // printf("%s\n", tree->DebugString().c_str());
    bool is_proof;
    bool is_ok = true;
    for(int i = 0; i < v.size(); i++) {
        is_proof = tree->ContentProof(v[i]);
        if(!is_proof) {
            printf("ContentProof, step1 num:%u fail.\n", i);
            is_ok = false;
            break;
        }
    }
    if(is_ok) {
        SHA256Content* tmp;
        for(int i=0; i<v.size(); i++) {
            tmp = static_cast<SHA256Content*>(v[i]);
            tmp->data_[0] += 1;
            is_proof = tree->ContentProof(tmp);
            if(is_proof) {
                printf("ContentProof, step2 num:%u fail.\n", i);
                is_ok = false;
                break;
            }
        }
    }
    
    delete tree;
    if(is_ok) {
        printf("=== ok.\n");
    }
    else {
        printf("=== fail.\n");
    }
    return is_ok;
}

typedef bool (*TestUnitFun)();
TestUnitFun Tests[] = {
    TestNewMerkleTreeWithDefaultHash,
    TestNewMerkleTreeWithSpecifiedHash,
    TestTreeProof,
    TestContentProof,
};

int main() {
    int test_num = sizeof(Tests)/sizeof(TestUnitFun);
    int success_num = 0;
    for(int i=0; i<test_num; i++) {
        if(Tests[i]()) {
            success_num ++;
        }
    }
    printf("----------------------\n");
    printf("total  : %d\n", test_num);
    printf("success: %d\n", success_num);
    printf("fail   : %d\n", test_num - success_num);

    return 0;
}