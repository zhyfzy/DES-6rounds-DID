#ifndef CONST_H
#define CONST_H
#include <cstdio>
#define print_long_hex(x) printf(""#x": %016llX\n",(x))
#define print_hex(x) printf(""#x": %08X\n",(x))

typedef long long ll;
typedef unsigned long long ull;

ull left32(const ull &num){
    return (num >> 32) & ((1ULL << 32) - 1);
}

ull right32(const ull &num){
    return num & ((1ULL << 32) - 1);
}

ull left28(const ull &num){
    return (num >> 28) & ((1ULL << 28) - 1);
}

ull right28(const ull &num){
    return num & ((1ULL << 28) - 1);
}

ull calcBase(const int *table, int input_size, int output_size, const ull &num) {
    ull ret = 0;
    for (int i = 0; i < output_size; i++) {
        ret <<= 1;
        ret |= (num >> (input_size - table[i])) & 1ULL;
    }
    return ret;
}

ull calcInvBase(const int *table, int input_size, int output_size, const ull &num) {
    ull ret = 0;
    ull cur = num;
    for (int i = output_size - 1; i>=0; i--){
        ret |= (cur & 1ULL) << (input_size - table[i]);
        cur >>= 1;
    }
    return ret;
}

const int ip_size = 64;
const int ip[ip_size] = {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9, 1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
};

ull calcIp(const ull &num) {
    return calcBase(ip, ip_size, ip_size, num);
}
ull calcInvIp(const ull &num) {
    return calcInvBase(ip, ip_size, ip_size, num);
}


const int ext_input_size = 32;
const int ext_output_size = 48;
const int ext[48] = {
        32, 1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1
};

ull calcExt(const ull &num) {
    calcBase(ext, ext_input_size, ext_output_size, num);
}

const int sbox_index = 8;
const int sbox_row = 4;
const int sbox_col = 16;
const int sbox_input_size = 1 << 6;
const int sbox_output_size = 1 << 4;

const int sbox[sbox_index][sbox_row][sbox_col] =
        {
                14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13,

                15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9,

                10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12,

                7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14,

                2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3,

                12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13,

                4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12,

                13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
        };

int calcSingleSbox(const int &index, const int &num) {
    int row = (num & 1) | ((num & 32) >> 4);
    int col = (num & 30) >> 1;
    return sbox[index][row][col];
}

ull calcSbox(const ull &num) {
    ull ret = 0;
    /* S-Box Tables */
    for (int i = 0; i < 8; i++) {
        ull row = ((num & (0x0000840000000000 >> (6 * i))) >> (42 - 6 * i));
        row = (row >> 4) | (row & 0x01);
        ull col = ((num & (0x0000780000000000 >> (6 * i))) >> (43 - 6 * i));
        ret <<= 4;
        ret |= (uint32_t) (sbox[i][row][col] & 0x0f);
    }
    return ret;
}

const int perm_size = 32;
const int perm_input_size = perm_size;
const int perm_output_size = perm_size;
const int perm[perm_size] = {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25
};

ull calcPerm(const ull &num) {
    calcBase(perm, perm_input_size, perm_output_size, num);
}

ull calcInvPerm(const ull &num) {
    calcInvBase(perm, perm_input_size, perm_output_size, num);
}

/* Permuted Choice 1 Table */
const int pc1_input_size = 64;
const int pc1_output_size = 56;
const int pc1[pc1_output_size] = {
        57, 49, 41, 33, 25, 17, 9,
        1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27,
        19, 11, 3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29,
        21, 13, 5, 28, 20, 12, 4
};

ull calcPc1(const ull &num) {
    return calcBase(pc1, pc1_input_size, pc1_output_size, num);
}

ull calcInvPc1(const ull &num) {
    return calcInvBase(pc1, pc1_input_size, pc1_output_size, num);
}

/* Permuted Choice 2 Table */
const int pc2_input_size = 56;
const int pc2_output_size = 48;
const int pc2[pc2_output_size] = {
        14, 17, 11, 24, 1, 5,
        3, 28, 15, 6, 21, 10,
        23, 19, 12, 4, 26, 8,
        16, 7, 27, 20, 13, 2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32
};

ull calcPc2(const ull &num) {
    return calcBase(pc2, pc2_input_size, pc2_output_size, num);
}

ull calcInvPc2(const ull &num) {
    return calcInvBase(pc2, pc2_input_size, pc2_output_size, num);
}

/* Iteration Shift Array */
const int iteration_shift[16] = {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

const int pi_size = 64;
const int pi[] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25
};

ull calcPi(const ull &num) {
    calcBase(pi, pi_size, pi_size, num);
}

ull calcInvPi(const ull &num){
    calcInvBase(pi, pi_size, pi_size, num);
}

#endif //CONST_H