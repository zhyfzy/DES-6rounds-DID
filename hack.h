#ifndef DES6_HACK_H
#define DES6_HACK_H
#include <iostream>
#include <random>
#include "const.h"
#include "did_table.h"
#include <ctime>
#include <cassert>
#include <set>
#include <algorithm>

using namespace std;

independent_bits_engine<default_random_engine,64,ull> random_engine;

static DidTable did;

class DesHackHelper{
private:
    ull l_match;
    ull r_match;
    size_t avail_index_num;
    int avail_index[8];

public:
    DesHackHelper(int mode){
        if (mode == 1){
            l_match = 0x40080000;
            r_match = 0x04000000;
            avail_index_num = 5;
            avail_index[0] = 2;
            avail_index[1] = 5;
            avail_index[2] = 6;
            avail_index[3] = 7;
            avail_index[4] = 8;
        }else{
            l_match = 0x00200008;
            r_match = 0x00000400;
            avail_index_num = 2;
            avail_index[0] = 1;
            avail_index[1] = 4;
        }
    }

    bool inputCheck(const ull &inA, const ull &inB){
        ull inD = calcIp(inA) ^ calcIp(inB);
        ull l = left32(inD);
        ull r = right32(inD);
        return (l == l_match && r == r_match);
    }

    bool availCheck(const ull &inA, const ull &inB, const ull &outA, const ull &outB){
        return hack(inA, inB, outA, outB, nullptr);
    }

    bool hack(const ull &inA,
              const ull &inB,
              const ull &outA,
              const ull &outB,
              vector<set<int> > *ret_key=nullptr){
        if (!inputCheck(inA, inB)) return false;
        ull outD = calcInvPi(outA ^ outB);
        ull r6D = right32(outD);
        ull c = calcInvPerm(r6D ^ r_match);

        ull eA = calcExt(left32(calcInvPi(outA)));
        ull eB = calcExt(left32(calcInvPi(outB)));
        ull eD = eA ^ eB;

        for (int i=0; i<avail_index_num;i++){ // each sbox available
            int index = avail_index[i] - 1;

            int shift_e = (7 - index) * 6;
            int sub_eD = (int)((eD & (63ULL << shift_e)) >> shift_e);
            int sub_eA = (int)((eA & (63ULL << shift_e)) >> shift_e);

            int shift_c = (7 - index) * 4;
            int sub_c = (int)((c & (15ULL << shift_c)) >> shift_c);

            vector<int> cdt; // candidate
            did.get(index,sub_eD,sub_c, cdt);
            if (cdt.empty()){
                return false;
            }
            if (ret_key != nullptr){
                set<int> t;
                t.clear();
                for (int j=0;j<cdt.size();j++){
                    int key_maybe = cdt[j] ^ sub_eA;
                    if ((*ret_key)[index].count(key_maybe)){
                        t.insert(key_maybe);
                    }
                }
                (*ret_key)[index].clear();
                for (int j: t){
                    (*ret_key)[index].insert(j);
                }
            }
        }
        return true;
    }

    void constructCipherPlaintexts(const ull &key, ull &inA, ull &inB, ull &outA, ull &outB, ull in=0){// 给定key构造一组明密文对
        while (true) {
            if (in==0)
                inA = random_engine();
            else{
                inA = in; // 使用给定的明文
            }
            ull diff = (l_match << 32) | r_match;
            inB = calcInvIp(calcIp(inA) ^ diff);

            pre_l3 = 0;
            outA = desEncrypt(inA, key);
            outB = desEncrypt(inB, key);

            if (pre_l3 == ((r_match << 32) | l_match)){
                if (availCheck(inA, inB, outA, outB)){
                    // 找到了合适的明密文对
//                    printf("--- new cp ---\n");
//                    print_long_hex(inA);
//                    print_long_hex(inB);
//                    print_long_hex(outA);
//                    print_long_hex(outB);
                    return;
                }
            }
        }
    }
};


class DesHacker {
private:
    struct PairCp{ // a pair of cipher and plaintext 一组明文和密文
        ull inA, inB, outA, outB;
        PairCp(const ull &inA, const ull &inB, const ull &outA, const ull &outB):
                inA(inA), inB(inB), outA(outA), outB(outB){}
    };
    vector<PairCp> cp_m1; // 满足第一种差分特征的明密文对
    vector<PairCp> cp_m2; // 满足第二种差分特征的明密文对
    DesHackHelper hackerA;
    DesHackHelper hackerB;
    vector<set<int> > ret_sub_key; // 输入到每个S盒可能的子密钥。一共有64中可能性
    vector<int> enum_binary_position; // 需要枚举的14个二进制位的位置
    bool found_key; // 是否找到密钥
    ull ret_key; // 存放找到的密钥

    ull invPc(ull pc2_out) {
        ull pc2_in = calcInvPc2(pc2_out);
        ull l = left28(pc2_in);
        ull r = right28(pc2_in);
        for (int j = 0; j < 10; j++) {
            l = (0x0fffffff & (l >> 1)) | ((0x00000001 & l) << 27); // right shift
            r = (0x0fffffff & (r >> 1)) | ((0x00000001 & r) << 27);
        }
        ull pc1_out = (l << 28) | r;
        ull pc1_in = calcInvPc1(pc1_out);
        return pc1_in;
    }

    int cur_process = 0; // 当前进度
    int max_process = 0;

    void binaryEnum(ull num) { // 对其他二进制位进行枚举
        cur_process++;
        if (cur_process % 100 == 0)
            printf("process: %d/%d\n",cur_process, max_process);
        if (cur_process > max_process){
            cur_process = max_process;
        }
        int enum_max = (1 << enum_binary_position.size()) - 1;
//        if ((target_key & invPc(0xfff03fffffff)) == num)
        for (int i = 0; i < enum_max; i++) {
            int bin = i; //枚举的14位二进制数
            ull key = num;
            for (int j: enum_binary_position) {
                if (bin & 1) {
                    key |= (1ULL << j);
                }
                bin >>= 1;
            }
            if (desEncrypt(cp_m1[0].inA, key) == cp_m1[0].outA) {
                ret_key = key;
                found_key = true;
                break;
            }
        }
    }

    void combine(int deep, ull num) { //对每个S盒可能的子密钥进行组合，使用DFS
        if (deep >= 8) {
            ull out = invPc(num);
            binaryEnum(out);
            return;
        }
        if (deep == 2){
            combine(deep + 1, num);
            return;
        }
        for (int i: ret_sub_key[deep]) {
            int shift = (7 - deep) * 6;
            ull cur = num | (((ull)i) << shift);
            combine(deep + 1, cur);
            if (found_key) return;
        }
    }

public:
//    ull target_key;
    DesHacker():hackerA(1),hackerB(2),found_key(false),ret_key(0){
        random_engine.seed((ull)time(nullptr));
        ret_sub_key.resize(8);
        for (int i=0;i<8;i++){
            for (int j=0;j<64;j++){
                ret_sub_key[i].insert(j); // 一开始存放所有可能的情况，随着分析过程逐步筛减。
            }
        }
        cp_m1.clear();
        cp_m2.clear();
        enum_binary_position.clear();
        ull enum_mask = calcInvPc1((1ULL << 56) - 1);
        ull pc_mask = invPc(0xfff03fffffff) ^ (~enum_mask); // 其中等于0的二进制位是需要最后时枚举的，一共14位
        ull t = pc_mask;
        for (int i=0;i<64;i++){
            if (!(t & 1))
                enum_binary_position.push_back(i);
            t >>= 1;
        }
    }

    bool addCipherPlaintexts(ull in1, ull in2, ull out1, ull out2){
        if (hackerA.inputCheck(in1, in2)){
            cp_m1.push_back(PairCp(in1, in2, out1, out2));
            return true;
        } else if (hackerB.inputCheck(in1, in2)){
            cp_m2.push_back(PairCp(in1, in2, out1, out2));
            return true;
        } else
            return false;
    }

    bool hack() {
        found_key = false;
        if (cp_m1.empty() || cp_m2.empty()){
            printf("At least one pair of cipher and plaintexts for each mode...\n");
            return false;
        }

        for (int i=0;i<cp_m1.size();i++){
            if (!hackerA.hack(cp_m1[i].inA, cp_m1[i].inB, cp_m1[i].outA, cp_m1[i].outB, &ret_sub_key))
                return false;
        }
        for (int i=0;i<cp_m2.size();i++){
            if (!hackerB.hack(cp_m2[i].inA, cp_m2[i].inB, cp_m2[i].outA, cp_m2[i].outB, &ret_sub_key))
                return false;
        }

        max_process = 1; // 用于展示计算进度
        for(int i=0;i<8;i++){
            if (i==2) continue;
            max_process *= ret_sub_key[i].size();
        }

        combine(0, 0ULL); // 对每个可能的密钥进行枚举
        return found_key;
    }

    ull getKey(){
        if (found_key)
            return ret_key;
        else{
            printf("Key not found...\n");
            return 0;
        }
    }
};

void constructCipherPlaintexts(const ull &key, int mode,
                               ull &inA, ull &inB, ull &outA, ull &outB,
                               const ull &given_inA=0){
    DesHackHelper hacker(mode);
    hacker.constructCipherPlaintexts(key, inA, inB, outA, outB, given_inA);
}

#endif //DES6_HACK_H
