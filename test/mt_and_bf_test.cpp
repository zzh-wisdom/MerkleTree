#include <sys/time.h>
#include "contents.h"
#include "../src/merkle_tree.h"

vector<Content*> BuildSHA256Contents(int n) {
    vector<Content*> v;
    string tmp;
    char buf[65];
    for(int i=0; i<n; i++) {
        tmp = to_string(i);
        snprintf(buf, sizeof(buf), "%8s%8s%8s%8s%8s%8s%8s%8s", 
            tmp.c_str(), tmp.c_str(), tmp.c_str(), tmp.c_str(),
            tmp.c_str(), tmp.c_str(), tmp.c_str(), tmp.c_str());
        tmp = string(buf);
        v.push_back(new SHA256Content(i, tmp));
    }
    return v;
}

vector<Content*> DeepCopySHA256Contents(vector<Content*> contents) {
    vector<Content*> v;
    SHA256Content *tmp;
    for(int i=0; i<contents.size(); i++) {
        tmp = static_cast<SHA256Content*>(contents[i]);
        v.push_back(new SHA256Content(*tmp));
    }
    return v;
}

vector<Content*> BuildSHA256ContentsWithMissRate(vector<Content*> contents, double miss_rate) {
    vector<Content*> v = DeepCopySHA256Contents(contents);
    SHA256Content *tmp;

    int total = v.size();
    int miss_num = (double)total * miss_rate;
    printf("miss_num:%d\n", miss_num);
    int i = total - miss_num;

    for(; i<total; i++) {
        tmp = static_cast<SHA256Content*>(v[i]);
        tmp->data_[0] += 1;
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

static inline uint64_t get_now_micros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000000 + tv.tv_usec;
}

bool TestContentProofWithoutBF(MerkleTree *tree, vector<Content*> contents, double miss_rate, int min_depth_for_bf) {
    int total = contents.size();
    int miss_num = (double)total * miss_rate;
    int un_miss_num = total - miss_num;
    int i = 0;
    min_depth_for_bf = tree->GetRootDepth() + 1;
    printf("TestContentProofWithoutBF\t..miss:%0.2f, miss_num:%d", miss_rate, miss_num);

    uint64_t begin = get_now_micros();
    for(; i<un_miss_num; i++) {
        assert(tree->ContentProof(contents[i] ,min_depth_for_bf));
    }
    for(; i<total; i++) {
        assert(!tree->ContentProof(contents[i] , min_depth_for_bf));
    }
    uint64_t end = get_now_micros();
    printf(", time:%0.2fms\n", (double)(end-begin)/1000.0);
    return true;
}

bool TestContentProofWithBF(MerkleTree *tree, vector<Content*> contents, double miss_rate, int min_depth_for_bf){
    int total = contents.size();
    int miss_num = (double)total * miss_rate;
    int un_miss_num = total - miss_num;
    int i = 0;
    printf("TestContentProofWithBF\t\t..miss:%0.2f, miss_num:%d", miss_rate, miss_num);

    uint64_t begin = get_now_micros();
    for(; i<un_miss_num; i++) {
        assert(tree->ContentProof(contents[i] ,min_depth_for_bf));
    }
    for(; i<total; i++) {
        assert(!tree->ContentProof(contents[i] , min_depth_for_bf));
    }
    uint64_t end = get_now_micros();
    printf(", time:%0.2fms depth:%d\n", (double)(end-begin)/1000.0, min_depth_for_bf);
    return true;
}


typedef bool (*TestUnitFun)(MerkleTree *tree, vector<Content*> contents, double miss_rate, int min_depth_for_bf);
TestUnitFun Tests[] = {
    TestContentProofWithoutBF,
    TestContentProofWithBF,
};

int main() {
    int entry_num = 4000;
    vector<Content*> contents;
    contents = BuildSHA256Contents(entry_num);

    MerkleTree tree(contents, true);
    int test_num = sizeof(Tests)/sizeof(TestUnitFun);
    int total_test_num = 0;
    int success_num = 0;
    double miss_rates[] = {0.0, 0.1, 0.2, 0.3, 0.4 ,0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    int rates_num = sizeof(miss_rates)/sizeof(double);
    vector< vector<Content*> > vv;
    for(int i = 0; i < rates_num; i++) {
        vv.push_back(BuildSHA256ContentsWithMissRate(contents, miss_rates[i]));
    }

    printf("----------------------\n");
    printf("* entry_num:%d\n", entry_num);

    for(int i = 0; i < rates_num; i++) {
        vector<Content*>& tmp_v = vv[i];
        double tmp_miss_rate = miss_rates[i];
        printf("----------------------\n");
        for(int j=0; j<test_num; j++) {
            total_test_num++;
            if(Tests[j](&tree, tmp_v, tmp_miss_rate, 2)) {
                success_num ++;
            }
        }
    }

    // for(int i = 0; i < rates_num; i++) {
    //     vector<Content*>& tmp_v = vv[i];
    //     double tmp_miss_rate = miss_rates[i];
    //     printf("----------------------\n");
    //     total_test_num += 2;
    //     if(TestContentProofWithBF(&tree, tmp_v, tmp_miss_rate, tree.GetRootDepth())){
    //         success_num ++;
    //     }
    //     if(TestContentProofWithBF(&tree, tmp_v, tmp_miss_rate, 2)) {
    //         success_num ++;
    //     }
    // }

    // 不同深度
    // int max_depth = 13;
    // for(int i = 0; i < rates_num; i++) {
    //     vector<Content*>& tmp_v = vv[i];
    //     double tmp_miss_rate = miss_rates[i];
    //     printf("----------------------\n");
    //     for(int j=2; j<=max_depth; j++) {
    //         total_test_num ++;
    //         if(TestContentProofWithBF(&tree, tmp_v, tmp_miss_rate, j)){
    //             success_num ++;
    //         }
    //     }
    // }

    printf("----------------------\n");
    printf("total  : %d\n", total_test_num);
    printf("success: %d\n", success_num);
    printf("fail   : %d\n", total_test_num - success_num);

    DestroySHA256Contents(contents);
    for(int i = 0; i < rates_num; i++) {
        DestroySHA256Contents(vv[i]);
    }
    return 0;
}