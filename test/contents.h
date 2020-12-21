#ifndef MERKLE_TREE_CONTENTS_H_
#define MERKLE_TREE_CONTENTS_H_

#include<iostream>
#include"../src/merkle_tree.h"
#include"../util/hash.h"
using namespace std;

class SimpleContent: public Content {
public:
    int num_;
    string data_;

    SimpleContent(int num, string data):num_(num), data_(data) {}

    string CalculateHash() override {
        return HashWithSeed0(data_);
    }

    bool Equals(Content* other) override {
        SimpleContent* s2 = static_cast<SimpleContent*>(other);
        return s2->num_ == this->num_ && s2->data_ == this->data_;
    }

    int GetNum() override {
        return num_;
    }
};

class SHA256Content: public Content {
public:
    int num_;
    string data_;

    SHA256Content(int num, string data):num_(num), data_(data) {}

    string CalculateHash() override {
        return SHA256(data_);
    }

    bool Equals(Content* other) override {
        SHA256Content* s2 = static_cast<SHA256Content*>(other);
        return s2->num_ == this->num_ && s2->data_ == this->data_;
    }

    int GetNum() override {
        return num_;
    }
};

#endif