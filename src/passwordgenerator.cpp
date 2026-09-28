/*
 * passwordgenerator.cpp
 */

#include "passwordgenerator.h"
#include <random>
#include <algorithm>

passwordgenerator::passwordgenerator() {
	// The "hash": set name -> the characters that set contributes.
	charsets["lower"] = "abcdefghijklmnopqrstuvwxyz";
	charsets["upper"] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	charsets["digits"] = "0123456789";
	charsets["symbols"] = "!@#$%^&*()-_=+?";
}

std::string passwordgenerator::charsetOf(const std::string& setname) const {
	auto it = charsets.find(setname);
	if(it == charsets.end()) {
		return "";
	}
	return it->second;
}

std::string passwordgenerator::buildPool(const std::vector<std::string>& setnames) const {
	std::string pool;
	for(const std::string& name : setnames) {
		pool += charsetOf(name);
	}
	return pool;
}

std::string passwordgenerator::generate(const std::vector<std::string>& setnames, int length) const {
	std::string pool = buildPool(setnames);
	if(pool.empty() || length < 1) {
		return "";
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> pooldist(0, (int)pool.size() - 1);
	std::string result;
	result.reserve(length);

	// One character from each selected set first, so every chosen set is present.
	for(const std::string& name : setnames) {
		std::string cs = charsetOf(name);
		if(!cs.empty() && (int)result.size() < length) {
			std::uniform_int_distribution<int> setdist(0, (int)cs.size() - 1);
			result += cs[setdist(gen)];
		}
	}
	while((int)result.size() < length) {
		result += pool[pooldist(gen)];
	}
	std::shuffle(result.begin(), result.end(), gen);
	return result;
}
