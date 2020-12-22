#include <assert.h>
#include"merkle_tree.h"
#include"../util/hash.h"
#include"../util/coding.h"

double BLOOM_FILTER_P = 0.01;

void Node::buildBF() {
    if(is_dup_) {
        bf_ = nullptr;
        return;
    }
    assert(!is_leaf_);
    int n = right_idx_ - left_idx_ + 1;
    double p = tree_->p_;
    bf_ = new BloomFilter(n, p);
    for(int i=left_idx_; i<=right_idx_; i++) {
        bf_->AddKey(tree_->leafs_[i]->content_->CalculateHash(false));
    }
}

string Node::VerifyNode(vector<Content*>* damaged_contents) {
    string tmp_hash;
    if(this->is_leaf_) {
        tmp_hash = this->content_->CalculateHash(false);
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
        return this->content_->CalculateHash(false);
    }

    string combo_hash = this->left_->hash_ + this->right_->hash_;
    return this->tree_->hash_fun_(combo_hash);
}

Node* Node::FindLeafMayUsingBF(Content *content, int min_depth_for_bf) {
    if(is_dup_) {  // 如果是复制的节点，则不用查找
        return nullptr;
    }
    if(depth_<min_depth_for_bf) {  // 小于，直接循环查找
        for(int i=left_idx_; i<=right_idx_; i++) {
            if(tree_->leafs_[i]->content_->EqualsByHashAndNum(content, true)) {
                return tree_->leafs_[i];
            }
        }
        return nullptr;
    }
    else { // 使用bloom filter
        if(!bf_->KeyMayMatch(content->CalculateHash(true))) { // 不存在
            return nullptr;
        }
        else {  // 可能存在
            Node* left = left_->FindLeafMayUsingBF(content, min_depth_for_bf);
            Node* right = right_->FindLeafMayUsingBF(content, min_depth_for_bf);
            if(left) {
                assert(right == nullptr);
                // if(right != nullptr) {
                //     printf("left num:%d, right num:%d", left->content_->GetNum(), right->content_->GetNum());
                // }
                return left;
            }
            else {
                return right;
            }
        }
    }
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

MerkleTree::MerkleTree(vector<Content*> contents, bool enable_bf):
    hash_fun_(SHA256), enable_bf_(enable_bf), p_(BLOOM_FILTER_P) {
    this->root_ = buildWithcontents(this, contents, leafs_);
    this->root_hash_ = this->root_->hash_;

    // printf("root_->right_idx_:%d, leafs_ size:%lu\n", root_->right_idx_, leafs_.size());
    assert(!root_->is_dup_);
    assert(root_->left_idx_ == 0);
    assert(root_->right_idx_ == leafs_.size()-1);
    printf("root depth:%d, keys num:%lu, root bf bitset(bytes):%d\n", 
        root_->depth_, leafs_.size(), root_->bf_->GetBitsetLen());
}

MerkleTree::MerkleTree(vector<Content*> contents, HashFun hash_fun, bool enable_bf):
    hash_fun_(hash_fun), enable_bf_(enable_bf), p_(BLOOM_FILTER_P) {
    this->root_ = buildWithcontents(this, contents, leafs_);
    this->root_hash_ = this->root_->hash_;

    assert(!root_->is_dup_);
    assert(root_->left_idx_ == 0);
    assert(root_->right_idx_ == leafs_.size()-1);
}

MerkleTree::~MerkleTree() {
    if(root_ != nullptr) {
        delete root_;
    }
}

Node* MerkleTree::buildWithcontents(MerkleTree* tree, vector<Content*> contents, vector<Node*>& leafs) {
    Node* tmp;
    for(int i = 0; i < contents.size(); i++) {
        tmp = new Node(tree, contents[i], i, false);
        leafs.push_back(tmp);
    }
    // 单数个数据元素，最后一个需要重复
    if(contents.size()%2 == 1) {
        tmp = new Node(tree, contents[contents.size() - 1], contents.size(), true);
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

// min_depth_for_bf 表示树的深度大于等于该值时，才使用bf辅助搜索
Node* MerkleTree::findLeaf(Content *content, int min_depth_for_bf) {
    if(!enable_bf_) {
        min_depth_for_bf = root_->depth_+1; // 以强制使用线性查找
    }
    else if(min_depth_for_bf <= 1) { // 叶子节点无法使用bf
        min_depth_for_bf = 2; 
    }
    return root_->FindLeafMayUsingBF(content, min_depth_for_bf);
}

bool MerkleTree::ContentProof(Content *content, int min_depth_for_bf) {
    Node* child = findLeaf(content, min_depth_for_bf);
    if(child == nullptr) {
        return false;
    }
    // printf("num: %d", child->content_->GetNum());
    Node* current_parent = child->parent_;
    string root_hash = child->CalculateNodeHash();
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

    return false;
}

string MerkleTree::GetRootHash() {
    return root_hash_;
}

int MerkleTree::GetRootDepth() {
    return root_->depth_;
}

string MerkleTree::DebugString() {
    string tmp;
    root_->DebugString(0, tmp);
    return tmp;
}