#ifndef MERKLE_TREE_H_
#define MERKLE_TREE_H_

#include<string>
#include<vector>
#include<assert.h>
#include"../util/bloom_filter.h"

using namespace std;

struct MerkleTree;

/**
 * @brief 数据块内容-接口类
 * 
 */
class Content {
public:
    /**
     * @brief 计算数据块的hash值
     * 
     * @param is_cache 是否取已经计算好的Hash
     * @return string 
     */
    virtual string CalculateHash(bool is_cache) = 0;

    /**
     * @brief 判断this与other的数据内容是否相等
     * 
     * @param other 
     * @return true 
     * @return false 
     */
    virtual bool EqualsExact(Content* other) = 0;

    /**
     * @brief 判断this与other的hash值是否相等
     * 
     * @param other 
     * @param is_cache 是否取已经计算好的Hash
     * @return true 
     * @return false 
     */
    virtual bool EqualsByHash(Content* other, bool is_cache) = 0;

    /**
     * @brief 同时根据hash和num来判断是否相等
     * 
     * @param other 
     * @param is_cache 是否取已经计算好的Hash
     * @return true 
     * @return false 
     */
    virtual bool EqualsByHashAndNum(Content* other, bool is_cache) = 0;

    /**
     * @brief 获取该数据块的编号（只是为了方便测试）
     * 
     * @return int 
     */
    virtual int GetNum() = 0;
};

/**
 * @brief 树的节点
 * 
 */
struct Node {
    MerkleTree* tree_;
    Node* parent_;
	Node* left_;
    Node* right_;
	bool  is_leaf_;    /// 当前节点是否为叶子节点
    bool  is_dup_;     /// 是否为复制节点
	string hash_;      /// hash值
	Content* content_; /// 原始内容, Node不拥有它的所有权，只有leaf节点才有

    int depth_; /// 深度，以该节点为根的树的深度
    int left_idx_;    /// 以该节点为根的树中，叶子节点范围最左端的索引值
    int right_idx_;   /// 以该节点为根的树中，叶子节点范围最右端的索引值
    BloomFilter *bf_;

    /// 叶子节点
    Node(MerkleTree* tree, Content* content, int content_idx, bool is_dup):
        tree_(tree), 
        parent_(nullptr),
        left_(nullptr),
        right_(nullptr),
        is_leaf_(true),
        is_dup_(is_dup),
        content_(content),
        depth_(1),
        left_idx_(content_idx),
        right_idx_(content_idx),
        bf_(nullptr) {
            hash_ = content_->CalculateHash(false);
    }

    /// 中间节点
    Node(MerkleTree* tree, Node* left, Node* right, string hash_):
        tree_(tree), 
        parent_(nullptr),
        left_(left),
        right_(right),
        is_leaf_(false),
        is_dup_(false),
        content_(nullptr),
        hash_(hash_),
        depth_(left->depth_+1),
        left_idx_(left->left_idx_),
        right_idx_(right->right_idx_),
        bf_(nullptr) {
            assert(!left->is_dup_);
            assert(left->depth_ == right->depth_);
            if(!right->is_dup_ || right->is_leaf_) {
                assert(left->right_idx_+1 == right->left_idx_);
                assert(left->left_idx_ <= left->right_idx_);
                assert(right->left_idx_ <= right->right_idx_);
            } 
            else { // 复制的情况
                assert(left->left_idx_ == right->left_idx_);
                assert(left->right_idx_ == right->right_idx_);
                assert(left->left_idx_ < left->right_idx_);
            }

            buildBF();
        }
    /// copy
    Node(Node* n):
        tree_(n->tree_),
        parent_(n->parent_),
        left_(n->left_),
        right_(n->right_),
        is_leaf_(n->is_leaf_),
        is_dup_(true),
        content_(n->content_),
        hash_(n->hash_),
        depth_(n->depth_),
        left_idx_(n->left_idx_),
        right_idx_(n->right_idx_),
        bf_(nullptr) {}
    ~Node() {
        if(bf_ != nullptr) {
            delete bf_;
        }
        if(is_dup_) {
            return;
        }
        if(left_ != nullptr) {
            delete left_;
        }
        if(right_ != nullptr /*&& right_ != left_*/) {
            delete right_;
        }
    }
    /// 递归计算原始数据的hash值，用于验证
    /// damaged_contents 带回损坏的内容
    string VerifyNode(vector<Content*>* damaged_contents);
    
    /// 根据左右孩子的hash值计算得到该Node的hash值
    /// 如果自身已经是叶子节点，则返回content的hash值
    string CalculateNodeHash();

    /// 递归查找content所在的叶子节点
    /// min_depth_for_bf 表示树的深度大于等于该值时，才使用bf辅助搜索
    /// 不存在时返回nullptr指针
    Node* FindLeafMayUsingBF(Content *content, int min_depth_for_bf);

    void DebugString(int depth, string& out);

private:
    // void setIndex();
    void buildBF();
};

typedef string (*HashFun)(string);

struct MerkleTree {
    Node* root_; /// 树的根节点
    string root_hash_;
    vector<Node*> leafs_;
    HashFun hash_fun_;

    bool enable_bf_;  /// 是否开启bloom filter
    double p_;  /// bloom filter 的误判率

    /// 使用指定内容contents创建MerkleTree
    /// HashFun使用默认的函数，即sha256
    MerkleTree(vector<Content*> contents, bool enable_bf);

    /// 使用指定的hash函数和内容contents创建MerkleTree
    MerkleTree(vector<Content*> contents, HashFun hash_fun, bool enable_bf);

    ~MerkleTree();
    
    /// 完整性验证
    /// damaged_contents 带回损坏的内容
    bool TreeProof(vector<Content*>* damaged_contents);

    /// 数据块验证
    bool ContentProof(Content *content, int min_depth_for_bf);

    string GetRootHash();

    int GetRootDepth();

    string DebugString();

private:
    Node* buildWithcontents(MerkleTree* tree, vector<Content*> contents, vector<Node*>& leafs);
    Node* buildInner(MerkleTree* tree, vector<Node*>& leafs);
    Node* findLeaf(Content *content, int min_depth_for_bf);
};

#endif