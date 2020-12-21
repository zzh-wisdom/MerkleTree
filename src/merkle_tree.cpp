#include <assert.h>
#include"merkle_tree.h"
#include"../util/hash.h"
#include"../util/coding.h"

string Node::VerifyNode(vector<Content*>* damaged_contents) {
    string tmp_hash;
    if(this->is_leaf_) {
        tmp_hash = this->content_->CalculateHash();
        if (tmp_hash != this->hash_ && !is_dup_) {
            if(damaged_contents != nullptr) {
                (*damaged_contents).push_back(this->content_);
            }
        }
		return tmp_hash;
	}

    vector<Content*>* tmp = damaged_contents;
    if(this->left_->is_dup_) {
        tmp = nullptr;
    }
    string left_hash = this->left_->VerifyNode(tmp);
    tmp = damaged_contents;
    if(this->right_->is_dup_) {
        tmp = nullptr;
    }
	string right_hash = this->right_->VerifyNode(tmp);

    string combo_hash = left_hash + right_hash;
	return this->tree_->hash_fun_(combo_hash);
}

string Node::CalculateNodeHash() {
    if(this->is_leaf_) {
        return this->content_->CalculateHash();
    }

    string combo_hash = this->left_->hash_ + this->right_->hash_;
    return this->tree_->hash_fun_(combo_hash);
}

void Node::DebugString(int depth, string& out) {
    for(int i=0; i<depth; i++) {
        out += "  ";
    }
    out += HashToHexStr(hash_) + "\n";
    if(left_ != nullptr) {
        left_->DebugString(depth+1, out);
    }
    if(right_ != nullptr) {
        right_->DebugString(depth+1, out);
    }
}

MerkleTree::MerkleTree(vector<Content*> contents):hash_fun_(SHA256) {
    this->root_ = buildWithcontents(this, contents, leafs_);
    this->root_hash_ = this->root_->hash_;
}

MerkleTree::MerkleTree(vector<Content*> contents, HashFun hash_fun):hash_fun_(hash_fun) {
    this->root_ = buildWithcontents(this, contents, leafs_);
    this->root_hash_ = this->root_->hash_;
}

MerkleTree::~MerkleTree() {
    if(root_ != nullptr) {
        delete root_;
    }
}

Node* MerkleTree::buildWithcontents(MerkleTree* tree, vector<Content*> contents, vector<Node*>& leafs) {
    Node* tmp;
    for(int i = 0; i < contents.size(); i++) {
        tmp = new Node(tree, contents[i], false);
        leafs.push_back(tmp);
    }
    // 单数个数据元素，最后一个需要重复
    if(contents.size()%2 == 1) {
        tmp = new Node(tree, contents[contents.size() - 1], true);
        leafs.push_back(tmp);
    }

    return buildInner(tree, leafs);
}

Node* MerkleTree::buildInner(MerkleTree* tree, vector<Node*>& childs) {
    if(childs.size() == 1) {
        return childs[0];
    }
    assert(!childs.empty());
    vector<Node*> nodes;
    Node* tmp, *left, *right;

    string hash_tmp;
    for(int i = 0; i < childs.size(); i += 2) {
        left = childs[i];
        if(i+1 == childs.size()) {
            right = new Node(left);
            // right = left;
        }
        else {
            right = childs[i+1];
        }
        string combo_hash = left->hash_ + right->hash_;
        hash_tmp = tree->hash_fun_(combo_hash);
        tmp = new Node(tree, left, right, hash_tmp);
        nodes.push_back(tmp);
        left->parent_ = tmp;
        right->parent_ = tmp;
    }
    return buildInner(tree, nodes);
}

bool MerkleTree::TreeProof(vector<Content*>* damaged_contents) {
    string calculated_root_hash = this->root_->VerifyNode(damaged_contents);
    return calculated_root_hash == this->root_hash_;
}

bool MerkleTree::ContentProof(Content *content) {
    for(int i = 0; i < this->leafs_.size(); i++) {
        if(this->leafs_[i]->content_->GetNum() == content->GetNum()) {
            Node* child = this->leafs_[i];
            Node* current_parent = this->leafs_[i]->parent_;
            string root_hash = this->leafs_[i]->CalculateNodeHash();
            // printf("%s\n", HashToHexStr(root_hash).c_str());
            string combo_hash;
            while(current_parent != nullptr) {
                if(current_parent->left_ == child) {
                    combo_hash = root_hash + current_parent->right_->hash_;
                }
                else {
                    combo_hash = current_parent->left_->hash_ + root_hash;
                }
                root_hash = this->hash_fun_(combo_hash);
                // printf("%s\n", HashToHexStr(root_hash).c_str());
                child = current_parent;
                current_parent = current_parent->parent_;
            }
            // 只比较最终的root hash是否相等
            if(root_hash == this->GetRootHash()) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

string MerkleTree::GetRootHash() {
    return root_hash_;
}

string MerkleTree::DebugString() {
    string tmp;
    root_->DebugString(0, tmp);
    return tmp;
}