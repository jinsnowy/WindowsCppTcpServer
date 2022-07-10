#pragma once
#ifndef  _RANDOM_H_
#define _RANDOM_H_

#include <iostream>
#include <cstdio>
#include <random>
#include <array>
#include <assert.h>
#include <memory.h>
#include <unordered_set>

class RandomDevice
{
private:
	std::random_device device;
	std::unique_ptr<std::seed_seq> seeds;

private:
	RandomDevice()
	{
		auto seed_data = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(device));
		seeds = std::make_unique<std::seed_seq>(std::begin(seed_data), std::end(seed_data));
	}

public:
	std::mt19937 Generate()
	{
		return std::mt19937(*seeds);
	}

	static RandomDevice& Get()
	{
		static RandomDevice device;
		return device;
	}
};

template<int minValue, int maxValue>
class Random
{
private:
	std::mt19937 engine;
	std::uniform_int_distribution<int> int_dist;

public:
	Random()
		:
		engine(RandomDevice::Get().Generate()),
		int_dist(std::uniform_int_distribution<int>(minValue, maxValue))
	{
	}

	int Next()
	{
		return int_dist(engine);
	}
};

#endif // ! _RANDOM_H_
