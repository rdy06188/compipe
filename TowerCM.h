#pragma once

#include <iostream>

#define GetFourBit(x, pos) ((x >> (4*((pos+1)%2))) & 0xF)

using namespace std;

// 4bit+8bit
class TowerCM {
public:
    TowerCM(double _MEMORY) {
        LENGTH_2bit = _MEMORY / 2 / sizeof(uint8_t) * 4;
        LENGTH_4bit = _MEMORY / 2 / sizeof(uint8_t) * 2;
        // LENGTH_8bit = _MEMORY / 2 / sizeof(uint8_t);
        LENGTH =      _MEMORY / 2 / sizeof(uint8_t);

        counters_2bit = new uint8_t[LENGTH];
        counters_4bit = new uint8_t[LENGTH];
        // counters_8bit = new uint8_t[LENGTH];

        memset(counters_2bit, 0, LENGTH * sizeof(uint8_t));
        memset(counters_4bit, 0, LENGTH * sizeof(uint8_t));
        // memset(counters_8bit, 0, LENGTH * sizeof(uint8_t));
    }

    ~TowerCM() {
        delete[] counters_2bit;
        delete[] counters_4bit;
        // delete[] counters_8bit;
    }

    void InsertTwoBit(const data_type item, count_type val=1) {
        uint32_t counter_pos, index_in_counter;
        count_type value, set_value;

        // 2bit insert
        counter_pos = hash(item, 0) % LENGTH_2bit / 4;
        index_in_counter = hash(item, 0) % LENGTH_2bit % 4;
        value = GetTwoBit(counters_2bit[counter_pos], index_in_counter);
        set_value = (count_type)min((uint32_t)value + val, (uint32_t)3);

        switch (index_in_counter) {
        case 0:
            counters_2bit[counter_pos] = (counters_2bit[counter_pos] & 0x3f) + (set_value << 6);
            break;
        case 1:
            counters_2bit[counter_pos] = (counters_2bit[counter_pos] & 0xcf) + (set_value << 4);
            break;
        case 2:
            counters_2bit[counter_pos] = (counters_2bit[counter_pos] & 0xf3) + (set_value << 2);
            break;
        case 3:
            counters_2bit[counter_pos] = (counters_2bit[counter_pos] & 0xfc) + set_value;
            break;
        }
    }

    void InsertFourBit(const data_type item, count_type val=1) {
        uint32_t counter_pos, index_in_counter;
        count_type value, set_value;

        // 4bit insert
        counter_pos = hash(item, 1) % LENGTH_4bit / 2;
        index_in_counter = hash(item, 1) % LENGTH_4bit % 2;
        value = GetFourBit(counters_4bit[counter_pos], index_in_counter);
        set_value = (count_type)min((uint32_t)value + val, (uint32_t)15);

        switch (index_in_counter) {
        case 0:
            counters_4bit[counter_pos] = (counters_4bit[counter_pos] & 0xf) + (set_value << 4);
            break;
        case 1:
            counters_4bit[counter_pos] = (counters_4bit[counter_pos] & 0xf0) + set_value;
            break;
        }
    }

    void InsertEightBit(const data_type item, count_type val=1) {
        uint32_t counter_pos, index_in_counter;
        count_type value, set_value;

        // 8bit insert
        counter_pos = hash(item, 2) % LENGTH_8bit;
        value = counters_8bit[counter_pos];
        set_value = (count_type)min((uint32_t)value + val, (uint32_t)255);
        counters_8bit[counter_pos] = set_value;
    }


    void Insert(const data_type item, count_type val=1) {
        InsertTwoBit(item, val);
        InsertFourBit(item, val);
        // InsertEightBit(item, val);
	}

    count_type QueryTwoBit(const data_type item) {
        uint32_t h = hash(item, 0);
        return GetTwoBit(counters_2bit[h % LENGTH_2bit / 4], h % LENGTH_2bit % 4);
    }
    
    count_type QueryFourBit(const data_type item) {
        uint32_t h = hash(item, 1);
        return GetFourBit(counters_4bit[h % LENGTH_4bit / 2], h % LENGTH_4bit % 2);
    }
    
    count_type QueryEightBit(const data_type item) {
        return counters_8bit[hash(item, 2) % LENGTH_8bit];
    }

    count_type Query(const data_type item) {
        count_type minv = 15;
        count_type two = QueryTwoBit(item);
        count_type four = QueryFourBit(item);
        // count_type eight = QueryEightBit(item);

        if (two<3){
            minv = std::min(minv, two);
        }
        if (four<15){
            minv = std::min(minv, four);
        }
        // if (eight<255){
        //     minv = std::min(minv, eight);
        // }
        return minv;
    }

private:
    uint32_t LENGTH_2bit;
    uint32_t LENGTH_4bit;
    uint32_t LENGTH_8bit;

    uint32_t LENGTH;

    uint8_t* counters_2bit;
    uint8_t* counters_4bit;
    uint8_t* counters_8bit;

    count_type GetTwoBit(uint8_t x, int pos) {
        if (pos == 0) {
            return (x >> 6) & 0x3;
        }
        else if (pos == 1) {
            return (x >> 4) & 0x3;
        }
        else if (pos == 2) {
            return (x >> 2) & 0x3;
        }
        else {
            return x & 0x3;
        }
    }
};