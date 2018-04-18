#include "des.h"
#include "hack.h"
using namespace std;

int main(){
    ull inA=0, inB=0, outA=0, outB=0;
    ull inC=0, inD=0, outC=0, outD=0;
    ull key = 0xF0F0F0F0F0F0F0F0;

    constructCipherPlaintexts(key, 1, inA, inB, outA, outB);
    constructCipherPlaintexts(key, 2, inC, inD, outC, outD);

    DesHacker hacker;
    hacker.addCipherPlaintexts(inA, inB, outA, outB);
    hacker.addCipherPlaintexts(inC, inD, outC, outD);

    if (hacker.hack()) {
        ull result_key = hacker.getKey();
        printf("Success\n");
        print_long_hex(result_key);
    } else{
        printf("Failed...\n");
    }

    return 0;
}