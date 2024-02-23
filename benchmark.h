#pragma once

#include <chrono>
#include <climits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "util.h"
#include "Elastic.h"
#include "CMSketch.h"
#include "CMHeap.h"
#include "CUSketch.h"
#include "CUHeap.h"
#include "HeavyGuardian.h"
#include "USS.h"
#include "SS.h"
#include "ASketch.h"
#include "Salsa.h"
#include "LogLogFilter.h"
#include "MVSketch.h"
#include "ComPipe.h"
typedef std::chrono::high_resolution_clock::time_point TP;

inline TP now() { return std::chrono::high_resolution_clock::now(); }

data_type *read_data(const char *PATH, const count_type length,
                     count_type *cnt) {
	data_type *items = new data_type[length];
	data_type *it = items;

	TIMESTAMP *timestamps = new TIMESTAMP[length];
	TIMESTAMP *timestamp = timestamps;

	FILE *data = fopen(PATH, "rb");
	*cnt = 0;
	while (fread(it++, sizeof(data_type), 1, data) > 0 && fread(timestamp++, sizeof(TIMESTAMP), 1, data) > 0) 
  	{
		(*cnt)++;
	}

	fclose(data);

	return items;
}

bool *rebuild_data(data_type *items, count_type cnt, HashMap mp, double elephant_flow_threshold, double drop_probability){
	// drop_probability is precise to two decimal places
	bool *flags = new bool[cnt];
	count_type threshold = (count_type)(cnt * elephant_flow_threshold);
	for (uint32_t i = 0; i < cnt; i++)
	{
		if ((mp[items[i]] >= threshold) && (rand() % 100 < 100 * drop_probability))
		{
			flags[i] = false;
		}else{
			flags[i] = true;
		}
	}

	return flags;
	
}


void BenchAllFlowSize(const char *PATH) {
	std::cout << "All Flow Size Estimation Benchmark" << std::endl << std::endl;
	count_type cnt;
	data_type *items = read_data(PATH, 10000000000, &cnt);
	std::cout << "The number of packet:" << cnt << std::endl;
	constexpr int32_t mem_base = 0;
	constexpr int32_t mem_inc = 100000;
	constexpr int32_t mem_var = 10;
	constexpr int32_t cmp_num = 1;
	constexpr int32_t COUNTER_PER_BUCKET = 8;

	Abstract *sketches[mem_var][cmp_num];

	for (int i = 0; i < mem_var; ++i) {
		sketches[i][0] = new Elastic<COUNTER_PER_BUCKET>((i + 1) * mem_inc);
		sketches[i][1] = new MVSketch((i + 1) * mem_inc);
		sketches[i][2] = new USS((i + 1) * mem_inc / 100); 
		sketches[i][3] = new SS((i + 1) * mem_inc / 100); 
		sketches[i][4] = new CMHeap((i + 1) * mem_inc);
		sketches[i][5] = new CUHeap((i + 1) * mem_inc);
		sketches[i][6] = new ASketch((i + 1) * mem_inc);
		sketches[i][7] = new SalsaCM((i + 1) * mem_inc);
		sketches[i][8] = new LogLogFilter((i + 1) * mem_inc);
		sketches[i][9] = new ComPipe<COUNTER_PER_BUCKET>((i + 1) * mem_inc);
	}

	// Ground truth
	HashMap mp;
	for (int l = 0; l < cnt; ++l) {
		if (mp.find(items[l]) == mp.end())
			mp[items[l]] = 1;
		else
			mp[items[l]] += 1;
	}
	std::cout << "The number of flow: " << mp.size() << std::endl;
	for (int i = 0; i < mem_var; ++i) {
		int memory = (mem_base + mem_inc * (i + 1)) / 1000;
		std::cout << "Memory size: " << memory
		          << "KB" << std::endl
		          << std::endl;
		for (int l = 0; l < cnt; ++l) {
			for (int j = 0; j < cmp_num; ++j) {
				sketches[i][j]->Insert(items[l]);
			}
		}
		
		ofstream out;
		out.open("../result/All_flow_size_estimation_"+to_string(memory)+"KB.txt",ios::out | ios::trunc);
		for (int j = 0; j < cmp_num; ++j) {
			sketches[i][j]->CompareFlowSize(mp, mp.size(),out); 
			delete sketches[i][j];
		}
		out.close();
		std::cout << std::endl;
	}

	delete items;
}

void BenchHH(const char *PATH) {
	std::cout << "Heavy Hitter Benchmark" << std::endl << std::endl;
	count_type cnt;
	data_type *items = read_data(PATH, 10000000000, &cnt);
	std::cout << "The number of packet:" << cnt << std::endl;
	constexpr int32_t mem_base = 0;
	constexpr int32_t mem_inc = 100000;
	constexpr int32_t mem_var = 10;
	constexpr int32_t cmp_num = 1;
	constexpr int32_t COUNTER_PER_BUCKET = 8;

	Abstract *sketches[mem_var][cmp_num];

	for (int i = 0; i < mem_var; ++i) {
		sketches[i][0] = new Elastic<COUNTER_PER_BUCKET>((i + 1) * mem_inc);
		sketches[i][1] = new MVSketch((i + 1) * mem_inc);
		sketches[i][2] = new USS((i + 1) * mem_inc / 100); 
		sketches[i][3] = new SS((i + 1) * mem_inc / 100); 
		sketches[i][4] = new CMHeap((i + 1) * mem_inc);
		sketches[i][5] = new CUHeap((i + 1) * mem_inc);
		sketches[i][6] = new ASketch((i + 1) * mem_inc);
		sketches[i][7] = new SalsaCM((i + 1) * mem_inc);
		sketches[i][8] = new LogLogFilter((i + 1) * mem_inc);
		sketches[i][9] = new ComPipe<COUNTER_PER_BUCKET>((i + 1) * mem_inc);
	}

	// Ground truth
	HashMap mp;
	for (int l = 0; l < cnt; ++l) {
		if (mp.find(items[l]) == mp.end())
			mp[items[l]] = 1;
		else
			mp[items[l]] += 1;
	}
	std::cout << "The number of flow: " << mp.size() << std::endl;
	for (int i = 0; i < mem_var; ++i) {
		int memory = (mem_base + mem_inc * (i + 1)) / 1000;
		std::cout << "Memory size: " << memory
		          << "KB" << std::endl
		          << std::endl;
		for (int l = 0; l < cnt; ++l) {
			for (int j = 0; j < cmp_num; ++j) {
				sketches[i][j]->Insert(items[l]);
			}
		}
		
		ofstream out;
		out.open("../result/Heavy_hitter_"+to_string(memory)+"KB.txt",ios::out | ios::trunc);
		for (int j = 0; j < cmp_num; ++j) {
			sketches[i][j]->CompareHH(mp, cnt, out);
			delete sketches[i][j];
		}
		out.close();
		std::cout << std::endl;
	}

	delete items;
}
