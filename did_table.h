#ifndef DID_TABLE_H
#define DID_TABLE_H

#include <cstdio>
#include <iostream>
#include <cstring>
#include <vector>
#include "const.h"
using namespace std;

class DidTableSingleSbox{
private:
    int index;
    vector<int> table[sbox_input_size][sbox_output_size];

public:
    DidTableSingleSbox() = delete;
    DidTableSingleSbox(const int &_index):index(_index){
        // index: sbox index
        for (int i=0;i<sbox_input_size;i++){
            for (int j=0;j<sbox_output_size;j++){
                table[i][j].clear();
            }
        }
        for (int x=0;x<sbox_input_size;x++){
            for (int y=0;y<sbox_input_size;y++){
                int in_d = x ^ y;
                int out_d = calcSingleSbox(index,x) ^ calcSingleSbox(index,y);
                table[in_d][out_d].push_back(x);
//                table[in_d][out_d].push_back(y);
            }
        }
    }
    void get(int i,int j, vector<int> &ret){
        ret.clear();
        for (int k=0;k<table[i][j].size();k++){
            ret.push_back(table[i][j][k]);
        }
    }
    size_t getSize(int i, int j){
        return table[i][j].size();
    }
};

class DidTable{
private:
    DidTableSingleSbox *box[sbox_index];
public:
    DidTable(){
        for (int i=0;i<8;i++){
            box[i] = new DidTableSingleSbox(i);
        }
    }
    ~DidTable(){
        for (int i=0;i<8;i++){
            delete box[i];
        }
    }
//    void test(){
//        for (int i=0;i<8;i++){
//            printf("---- Box %d ----\n",i);
//            for (int j=0;j<64;j++){
//                for (int k=0;k<16;k++){
//                    printf("%d ", (int)box[i]->getSize(j,k));
//                }
//                printf("\n");
//            }
//        }
//    }
//    void test(int index){
//        printf("---- Box %d ----\n",index);
//        for (int j=0;j<64;j++){
//            for (int k=0;k<16;k++){
//                printf("%d ", (int)box[index]->getSize(j,k));
//            }
//            printf("\n");
//        }
//    }
    void get(int index, int i, int j, vector<int> &ret){
        box[index]->get(i,j,ret);
    }
};

#endif