#ifndef DES6_DES_H
#define DES6_DES_H
#include <iostream>
#include "const.h"

void createSubKey(const ull &key, ull *sub_key) {
    ull pc1_out = calcPc1(key);
    ull l = left28(pc1_out);
    ull r = right28(pc1_out);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < iteration_shift[i]; j++) {
            l = (0x0fffffff & (l << 1)) | (0x00000001 & (l >> 27)); // left shift
            r = (0x0fffffff & (r << 1)) | (0x00000001 & (r >> 27));
        }
        ull pc2_in = (l << 28) | r;
        ull pc2_out = calcPc2(pc2_in);
        sub_key[i] = pc2_out;
    }
}

ull pre_l3 = 0;

ull des(const ull &txt, const ull &key, bool encrypt) {
    ull ip_out = calcIp(txt); // 初始置换
    ull l = (ip_out >> 32) & ((1ULL << 32) - 1);
    ull r = ip_out & ((1ULL << 32) - 1); // 分成左半部分和右半部分
    ull sub_key[6];
    createSubKey(key, sub_key); // 创建每一轮使用的子密钥
    for (int i = 0; i < 6; i++) {
        ull ext_out = calcExt(r); // 扩展
        ull sbox_in;
        if (encrypt) {
            sbox_in = ext_out ^ sub_key[i];
        }else
            sbox_in = ext_out ^ sub_key[5 - i];
        ull sbox_out = calcSbox(sbox_in); // s盒
        ull perm_out = calcPerm(sbox_out); // 置换

        ull tmp = r;
        r = l ^ perm_out;
        l = tmp;

        if (i==2){
            pre_l3 ^= (l << 32) | r;
        }
    }
    ull pi_in = l << 32 | r;
    ull pi_out = calcPi(pi_in); // 末置换
    return pi_out;
}

ull desEncrypt(const ull &txt, const ull &key) {
    return des(txt, key, true);
}

ull desDecrypt(const ull &txt, const ull &key) {
    return des(txt, key, false);
}

ull test() {

    ull txtA = 14930861438147399743ULL;
    ull keyA = 5123802368455698ULL;
    ull outA = desEncrypt(txtA, keyA);

    ull txtB = 10321427219600805951ULL;
    ull outB = desEncrypt(txtB, keyA);

    std::cout << outA << std::endl;
    std::cout << outB << std::endl;
}

#endif //DES6_DES_H
