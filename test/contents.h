#ifndef MERKLE_TREE_CONTENTS_H_
#define MERKLE_TREE_CONTENTS_H_

#include<iostream>
#include"../src/merkle_tree.h"
#include"../util/hash.h"
using namespace std;

class SimpleContent: public Content {
public:
    int num_;
    string hash_; // 暂存初始的正确的hash值，可以减少计算
    string data_;

    SimpleContent(int num, string data):num_(num), data_(data) {
        hash_ = HashWithSeed0(data_);
    }

    string CalculateHash(bool is_cache) override {
        if(is_cache) {
            return hash_;
        }
        else {
            return HashWithSeed0(data_);
        }  
    }

    bool EqualsExact(Content* other) override {
        SimpleContent* s2 = static_cast<SimpleContent*>(other);
        return s2->data_ == this->data_;
    }

    bool EqualsByHash(Content* other, bool is_cache) override {
        SimpleContent* s2 = static_cast<SimpleContent*>(other);
        return s2->CalculateHash(is_cache) == this->CalculateHash(is_cache);
    }

    bool EqualsByHashAndNum(Content* other, bool is_cache) override {
        SimpleContent* s2 = static_cast<SimpleContent*>(other);
        return s2->CalculateHash(is_cache) == this->CalculateHash(is_cache) 
            && s2->GetNum() == this->num_;
    }

    int GetNum() override{
        return num_;
    }
};

class SHA256Content: public Content {
public:
    int num_;
    string hash_; // 暂存初始的正确的hash值，可以减少计算
    string data_;

    SHA256Content(int num, string data):num_(num), data_(data) {
        hash_ = SHA256(data_);
    }

    string CalculateHash(bool is_cache) override {
        if(is_cache) {
            return hash_;
        }
        else {
            return SHA256(data_);
        }   
    }

    bool EqualsExact(Content* other) override {
        SHA256Content* s2 = static_cast<SHA256Content*>(other);
        return s2->data_ == this->data_;
    }

    bool EqualsByHash(Content* other, bool is_cache) override {
        SHA256Content* s2 = static_cast<SHA256Content*>(other);
        return s2->CalculateHash(is_cache) == this->CalculateHash(is_cache);
    }

    bool EqualsByHashAndNum(Content* other, bool is_cache) override {
        SHA256Content* s2 = static_cast<SHA256Content*>(other);
        return s2->CalculateHash(is_cache) == this->CalculateHash(is_cache) 
            && s2->GetNum() == this->num_;
    }

    int GetNum() override{
        return num_;
    }
};

#endif