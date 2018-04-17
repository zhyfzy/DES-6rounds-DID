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

static DidTable did;

class DesHackHelper{
private:
    ull l_match;
    ull r_match;
    size_t avail_index_num;
    int avail_index[8];
    independent_bits_engine<default_random_engine,64,ull> random_engine;

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
        random_engine.seed(2018);
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
                for (int j=0;j<cdt.size();j++){
                    (*ret_key)[index].insert(cdt[j] ^ sub_eA);
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
                inA = in;
            }
            ull diff = (l_match << 32) | r_match;
            inB = calcInvIp(calcIp(inA) ^ diff);

            pre_l3 = 0;
            outA = desEncrypt(inA, key);
            outB = desEncrypt(inB, key);

            if (pre_l3 == ((r_match << 32) | l_match)){
                if (availCheck(inA, inB, outA, outB)){
                    // get appropriate cipher-plaintexts
                    printf("--- new cp ---\n");
                    print_long_hex(inA);
                    print_long_hex(inB);
                    print_long_hex(outA);
                    print_long_hex(outB);
                    return;
                }
            }
        }
    }
};


class DesHacker {
private:
    vector<set<int> > ret_sub_key; // 输入到每个S盒可能的子密钥。
    ull pc_mask; //二进制中的0表示需要二进制枚举的位
    ull enum_mask;
    ull inA, inB, outA, outB; // 一组明文和密文
    ull inC, inD, outC, outD; // 一组明文和密文
    DesHackHelper hackerA;
    DesHackHelper hackerB;
    vector<int> enum_binary_position;
    bool found_key;
    ull ret_key;

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
            if (desEncrypt(inA, key) == outA) {
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
        if (deep == 2)
            combine(deep + 1, num);
        for (int i: ret_sub_key[deep]) {
            int shift = (7 - deep) * 6;
            ull cur = num | (((ull)i) << shift);
            combine(deep + 1, cur);
            if (found_key) return;
        }
    }

public:
    DesHacker():hackerA(1),hackerB(2),found_key(false),ret_key(0){
        ret_sub_key.resize(8);
        for (int i=0;i<8;i++){
            ret_sub_key.clear();
        }
        inA = inB = outA = outB = 0;
        inC = inD = outC = outD = 0;
        enum_binary_position.clear();
        enum_mask = calcInvPc1((1ULL << 56) - 1);
        pc_mask = invPc(0xfff03fffffff) ^ (~enum_mask); // 其中等于0的二进制位是需要最后时枚举的，一共14位
        ull t = pc_mask;
        for (int i=0;i<64;i++){
            if (!(t & 1))
                enum_binary_position.push_back(i);
            t >>= 1;
        }
    }

    bool addCipherPlaintexts(ull in1, ull in2, ull out1, ull out2){
        if (hackerA.inputCheck(in1, in2)){
            inA = in1; inB = in2; outA = out1; outB = out2;
            return true;
        } else if (hackerB.inputCheck(in1, in2)){
            inC = in1; inD = in2; outC = out1; outD = out2;
            return true;
        } else
            return false;
    }

    bool hack(vector<int> &_ret_key) {
        found_key = false;
        if (!hackerA.hack(inA, inB, outA, outB, &ret_sub_key))
            return false;

        if (!hackerB.hack(inC, inD, outC, outD, &ret_sub_key))
            return false;

        max_process = 1;
        for(int i=0;i<8;i++){
            if (i==2) continue;
            max_process *= ret_sub_key[i].size();
        }

        combine(0, 0ULL);
        return found_key;
    }

    ull getKey(){
        return ret_key;
    }
};

void constructCipherPlaintexts(const ull &key, int mode,
                               ull &inA, ull &inB, ull &outA, ull &outB,
                               const ull &given_inA=0){
    DesHackHelper hacker(mode);
    hacker.constructCipherPlaintexts(key, inA, inB, outA, outB, given_inA);
}

#endif //DES6_HACK_H
