/*
 * passwordgenerator.h
 *
 * Holds the character sets in an unordered_map (the "hash"): each set name maps
 * to its characters. The GUI hands it the names of the checked sets and it
 * matches them against the map, returning the combined character pool.
 *
 */

#ifndef PASSWORDGENERATOR_H_
#define PASSWORDGENERATOR_H_

#include <string>
#include <vector>
#include <unordered_map>


class passwordgenerator {
public:
	passwordgenerator();

	// buildPool is the matching step: for every selected set name it looks the
	// name up in the map and concatenates that set's characters. The result is
	// the pool of characters your generator will later draw from.
	std::string buildPool(const std::vector<std::string>& setnames) const;

	// charsetOf returns the characters mapped to a single set name, or "" if the
	// name is not in the map.
	std::string charsetOf(const std::string& setname) const;

	// generate builds a random password of the given length using only the
	// selected sets, guaranteeing at least one character from each when length
	// allows. Returns "" if nothing is selected.
	std::string generate(const std::vector<std::string>& setnames, int length) const;

private:
	std::unordered_map<std::string, std::string> charsets;
};

#endif /* PASSWORDGENERATOR_H_ */
