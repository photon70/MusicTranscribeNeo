#pragma once

#include <Siv3D.hpp>

#include "BitFlag.hpp"

struct Ng8 {
#define POD
	unsigned int value = 0;

	constexpr Ng8() = default;

	constexpr Ng8(unsigned int value)
		: value(value)
	{}

	constexpr Ng8(unsigned int num, unsigned int decimal, unsigned int resolution = 8)
		: value((num << 8) + (decimal << (8 - resolution)))
	{}

	Ng8(const Ng8& obj) = default;

	Ng8& operator=(const Ng8& obj) = default;

	constexpr operator unsigned int&(){
		return value;
	}

	constexpr operator unsigned int()const {
		return value;
	}

	constexpr unsigned int operator()(unsigned int  resolution = 0)const  {
		return value >> (8 - resolution);
	}

	constexpr Ng8 rounded(unsigned int  resolution = 0)const {
		return Ng8((value >> (8 - resolution)) << (8 - resolution)) + (BitIf(value, Bit(8 - resolution - 1)) ? Bit(8 - resolution) : 0);
	}

	constexpr Ng8 floor(unsigned int  resolution = 0)const {
		return Ng8((value >> (8 - resolution)) << (8 - resolution));
	}

	constexpr Ng8 ceil(unsigned int  resolution = 0)const {
		return Ng8((value >> (8 - resolution)) << (8 - resolution)) + (((value << (24 + resolution)) != 0) ? Bit(8 - resolution) : 0);
	}
};

class Beat {
public:
#define POD
	double overTime = 0;
	Ng8 value;
	bool flag = false;

	Beat() = default;

	explicit Beat(Ng8 value)
		: value(value)
	{}

	Beat(double overTime, Ng8 value, bool flag)
		: overTime(overTime)
		, value(value)
		, flag(flag)
	{}

	Beat(const Beat& obj) = default;

	Beat& operator=(const Beat& obj) = default;

	operator bool()const {
		return flag;
	}

	Ng8& operator ()() {
		return value;
	}

	const Ng8& operator ()()const {
		return value;
	}
};

class BeatCounter {
	double start = 0.;
	unsigned int bpm = 60;

	double lastSec = 0.;
	double currentSec = 0.;

	Ng8 last;
	Ng8 current;
public:
#define POD
	BeatCounter() = default;

	BeatCounter(double start, unsigned int bpm)
		: start(start)
		, bpm(bpm)
		, last(secToNg8(0.0))
		, current(secToNg8(0.0))
	{}

	void update(double nextSec) {
		lastSec = currentSec;
		last = current;

		currentSec = nextSec;
		current = secToNg8(currentSec);
	}

	void reset(double sec) {
		lastSec = sec;
		currentSec = sec;
		last = secToNg8(sec);
		current = last;
	}

	void setStart(double start) {
		this->start = start;
	}

	void setBpm(unsigned int bpm) {
		this->bpm = bpm;
	}

	bool isUpdated(unsigned int resolution = 0) {
		return ((last >> (8 - resolution)) ^ (current >> (8 - resolution))) != 0;
	}

	bool isBeforeUpdate(Ng8 ng8) {
		return ng8 < last;
	}

	bool isAfterUpdate(Ng8 ng8) {
		return ng8 > current;
	}

	Ng8 getCurrentNg8()const {
		return current;
	}

	Array<unsigned int> getAllCurrentNg8(unsigned int resolution = 0) {
		auto l = (last >> (8 - resolution));
		auto c = (current >> (8 - resolution));
		l = l > c ? 0 : l + 1;//オーバーフローなら0
		auto result = Array<unsigned int>();
		for (auto i : Range(l, c)) {
			result.push_back(i << (8 - resolution));
		}
		return result;
	}

	double ng8ToSec(Ng8 ng8)const {
		return start + (static_cast<double>(ng8) * 60.0 / static_cast<double>(bpm)) / 256.0;
	}

	Ng8 secToNg8(double sec)const {
		return Ng8(static_cast<unsigned int>(((sec - start) * static_cast<double>(bpm) / 60.0) * 256.0));
	}

	//if文で使う
	Beat Then(Ng8 time)const {
		if (last < time && time <= current) {
			return Beat(ng8ToSec(time) - lastSec, time, true);
		}
		return Beat();
	}

	//if文で使う
	Beat While(Ng8 time, Ng8 length)const {
		if (time <= current && current <= time + length) {
			return Beat(currentSec - ng8ToSec(time), time, true);
		}
		return Beat();
	}
};
