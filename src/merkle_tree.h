#ifndef MERKLE_TREE_H_
#define MERKLE_TREE_H_

#include<string>
#include<vector>

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
     * @param hash 
     * @return string 
     */
    virtual string CalculateHash() = 0;

    /**
     * @brief 判断this与other的数据内存是否相等
     * 
     * @param other 
     * @return true 
     * @return false 
     */
    virtual bool Equals(Content* other) = 0;
    
    /**
     * @brief 获取编号，用于定位
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
	Content* content_; /// 原始内容, Node不拥有它的所有权

    /// 叶子节点
    Node(MerkleTree* tree, Content* content, bool is_dup):
        tree_(tree), 
        parent_(nullptr),
        left_(nullptr),
        right_(nullptr),
        is_leaf_(true),
        is_dup_(is_dup),
        content_(content) {
        hash_ = content_->CalculateHash();
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
        hash_(hash_) {}
    /// copy
    Node(Node* n):
        tree_(n->tree_),
        parent_(n->parent_),
        left_(n->left_),
        right_(n->right_),
        is_leaf_(n->is_leaf_),
        is_dup_(true),
        content_(n->content_),
        hash_(n->hash_) {}
    ~Node() {
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

    void DebugString(int depth, string& out);
};

typedef string (*HashFun)(string);

struct MerkleTree {
    Node* root_; /// 树的根节点
    string root_hash_;
    vector<Node*> leafs_;
    HashFun hash_fun_;

    /// 使用指定内容contents创建MerkleTree
    /// HashFun使用默认的函数，即sha256
    MerkleTree(vector<Content*> contents);

    /// 使用指定的hash函数和内容contents创建MerkleTree
    MerkleTree(vector<Content*> contents, HashFun hash_fun);

    ~MerkleTree();
    
    /// 完整性验证
    /// damaged_contents 带回损坏的内容
    bool TreeProof(vector<Content*>* damaged_contents);

    /// 指定内容验证
    bool ContentProof(Content *content);

    string GetRootHash();

    string DebugString();

private:
    Node* buildWithcontents(MerkleTree* tree, vector<Content*> contents, vector<Node*>& leafs);
    Node* buildInner(MerkleTree* tree, vector<Node*>& leafs);
};

#endif