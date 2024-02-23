#pragma once

#include <iostream>
#include <fstream>
#include "hash.h"
#include "abstract.h"
#include "CMSketch.h"

using namespace std;

class MVCM : public Abstract{
private:
    double HEAVY_RATIO = 0.25;
    double LIGHT_RATIO = 0.75;

    uint32_t LIGHT_LENGTH;
    uint32_t HEAVY_LENGTH;

public:
    MVCM(double _MEMORY): Abstract((char *)"MVCM")
    {
        HEAVY_LENGTH = _MEMORY * HEAVY_RATIO / sizeof(Bucket);
        LIGHT_LENGTH = _MEMORY * LIGHT_RATIO / sizeof(uint8_t);
        LIGHT_LENGTH = LIGHT_LENGTH / 2;

        w = _MEMORY / r / sizeof(Bucket);
        sketch = new Bucket*[r];
        for (uint32_t i = 0; i < r; i++)
        {
            sketch[i] = new Bucket[w];
            memset(sketch[i], 0, sizeof(Bucket) * w);
        }
        
    }

    ~MVCM(){
        for (uint32_t i = 0; i < r; i++)
        {
            delete [] sketch[i];
        }
        delete [] sketch;
    }

    void Insert(const data_type key){
        uint32_t res = 0;
        for (uint32_t i = 0; i < r; i++)
        {
            uint32_t pos = hash(key, i) % w;
            sketch[i][pos].V += 1;
            if (sketch[i][pos].V == 1 && sketch[i][pos].C == 0){
                sketch[i][pos].K = key;
                sketch[i][pos].C = 1;
            }else if (sketch[i][pos].K == key){
                sketch[i][pos].C += 1;
            }else{
                sketch[i][pos].C -= 1;
                if (sketch[i][pos].C < 0)
                {
                    CMS->Insert(sketch[i][pos].K, (sketch[i][pos].V - sketch[i][pos].C) / 2);
                    res = CMS->Query(key);
                    CMS->Delete(key, res);

                    sketch[i][pos].K = key;
                    // sketch[i][pos].C = -sketch[i][pos].C;
                    // sketch[i][pos].V = sketch[i][pos].C;
                    sketch[i][pos].C = res;
                    sketch[i][pos].V = res;
                }
                else{
                    CMS->Insert(key);
                }
            }
        }
    }

    count_type Query(const data_type key){
        count_type ret = 0xffffffff;
        for (uint32_t i = 0; i < r; i++)
        {
            uint32_t pos = hash(key, i) % w;
            if (sketch[i][pos].K == key)
            {
                ret = min(ret, (count_type)((sketch[i][pos].V + sketch[i][pos].C) / 2));
            }else{
                // ret = min(ret, towerCM->Query(item));

            }
        }

        return ret;
    }

private:
    struct Bucket{
        data_type K;
        int32_t V;
        int32_t C;
    };

    uint32_t r = 4;
    uint32_t w;
    Bucket** sketch;
    CMSketch *CMS;
};