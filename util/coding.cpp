#include "coding.h"

string HashToHexStr(string hash) {
    string tmp;
    char buf[3];
    for(int i=0; i<hash.size(); i++) {
        sprintf(buf, "%02x", (unsigned char)hash[i]);
        tmp += buf;
    }
    return tmp;
}