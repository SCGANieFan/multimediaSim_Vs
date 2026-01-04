#include <stdexcept>
#include <iostream>
#include <random>
#include "MTF.Porting.h"

void AssertPorting(bool isAssert){
    throw std::runtime_error("memory be steped");
}


bool RandomPorting(uint32_t parent) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1, 100);
	int random_num = dist(gen);
	if (random_num < parent) return true;
	return false;
}

