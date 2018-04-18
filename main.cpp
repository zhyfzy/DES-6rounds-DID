#include "des.h"
#include "hack.h"
using namespace std;

int main(){
    ull inA=0, inB=0, outA=0, outB=0;
    ull inC=0, inD=0, outC=0, outD=0;
    ull key = 0xF0F0F0F0F0F0F0F0;

    DesHacker hacker;
    for (int i=0;i<2;i++){
        constructCipherPlaintexts(key, 1, inA, inB, outA, outB); // 随机生成满足第一种差分特征的两个明密文对
        hacker.addCipherPlaintexts(inA, inB, outA, outB); // 将构造好的明密文对传入DesHacker中
    }
    for (int i=0;i<2;i++){
        constructCipherPlaintexts(key, 2, inC, inD, outC, outD); // 随机生成满足第二种差分特征的两个明密文对
        hacker.addCipherPlaintexts(inC, inD, outC, outD);
    }

    if (hacker.hack()) { // 差分分析
        ull result_key = hacker.getKey();
        printf("Success\n");
        print_long_hex(result_key);
    } else{
        printf("Failed...\n");
    }

    return 0;
}