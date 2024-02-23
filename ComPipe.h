#pragma once

#include <iostream>
#include "hash.h"
#include "abstract.h"
// #include "TowerCU.h"
#include "TowerCM.h"

#define Tower 1

template<uint32_t  COUNTER_PER_BUCKET>
class ComPipe : public Abstract{
public:
    ComPipe(double _MEMORY)
        : Abstract((char *)"ComPipe")
    {
        HEAVY_LENGTH = _MEMORY * HEAVY_RATIO / sizeof(Bucket);
        LIGHT_LENGTH = _MEMORY * LIGHT_RATIO / sizeof(uint8_t);

        buckets = new Bucket[HEAVY_LENGTH];
        memset(buckets, 0, sizeof(Bucket) * HEAVY_LENGTH);
#if Tower
        towerCM = new TowerCM(_MEMORY * LIGHT_RATIO);     
#else
        counters = new uint8_t[LIGHT_LENGTH];  
#endif
    }

    ~ComPipe(){
        delete [] buckets;
#if Tower
        delete towerCM;
#else
        delete [] counters;
#endif
    }

    void Insert(const data_type item) {
        uint32_t pos = 0;

        // data_type ckey, tkey;
        // uint32_t cval, tval;
        uint32_t minVal = UINT32_MAX;
        data_type minID;
        uint32_t minPos = 0;
        uint32_t minIndex = 0;

        for (uint32_t i = 0; i < COUNTER_PER_BUCKET; i++){
            pos = hash(item, i) % HEAVY_LENGTH;
            if(buckets[pos].count[i] == 0){
                buckets[pos].ID[i] = item;
                buckets[pos].count[i] = 1;
                return;
            }

            if(buckets[pos].ID[i] == item){
                buckets[pos].count[i] += 1;
                return;
            }
            
            if(buckets[pos].count[i] < minVal){
                minVal = buckets[pos].count[i];
                minPos = pos;
                minIndex = i;
                minID = buckets[pos].ID[i];
            }
        }
#if Tower
        if (towerCM->Query(item) > minVal+towerCM->Query(minID)){
            towerCM->Insert(minID, minVal);
            buckets[minPos].count[minIndex] = 1;
            buckets[minPos].ID[minIndex] = item;
        }
        else{
            towerCM->Insert(item, 1);
        }
#else
        // TODO
        // Light_Insert(ckey, cval);
#endif
	}

    count_type Query(const data_type item){
        uint32_t pos = 0;
        count_type res = 0;
        for (uint32_t i = 0; i < COUNTER_PER_BUCKET; i++){
            pos = hash(item, i) % HEAVY_LENGTH;
            if(buckets[pos].ID[i] == item){
                res += buckets[pos].count[i];
            }        
        }
#if Tower
        return res + towerCM->Query(item);
#else
        return res + (count_type)counters[hash(item, 101) % LIGHT_LENGTH];
#endif
    }

private:
    double HEAVY_RATIO = 0.25;
    double LIGHT_RATIO = 0.75;

    struct Bucket{
        data_type ID[COUNTER_PER_BUCKET];
        count_type count[COUNTER_PER_BUCKET];

        // count_type Query(const data_type item) {
        //     for(uint32_t i = 0; i < COUNTER_PER_BUCKET; i++) {
        //         if(ID[i] == item) {
        //             return count[i];
        //         }
        //     }
        //     return 0;
        // }
    };


    uint32_t LIGHT_LENGTH;
    uint32_t HEAVY_LENGTH;

    Bucket* buckets;
#if Tower
    TowerCM* towerCM;
#else
    // CMS
    uint8_t* counters;
    const count_type MAXNUM = 255;

    void Light_Insert(const data_type item, count_type val = 1) {
        uint32_t position = hash(item, 101) % LIGHT_LENGTH;
        count_type old_val = (count_type)counters[position];
        count_type new_val = (count_type)counters[position] + val;
        new_val = (uint8_t)(new_val > MAXNUM ? MAXNUM : new_val);
        counters[position] = new_val;
    }
#endif

};
