/*
 * bruteforceengine.h
 *
 * Multithreaded benchmark engine. It brute-forces a target string that lives
 * only in memory for this session, purely to measure how long the search takes.
 * Candidates are compared directly against the target (no hashing, no files, no
 * network) - the only output is a search duration.
 *
 * The GUI calls startTest() (returns immediately), reads snapshot() every frame,
 * and never blocks: progress lives in atomics and the verdict behind a short
 * mutex.
 */

#ifndef BRUTEFORCEENGINE_H_
#define BRUTEFORCEENGINE_H_

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>


// testresult is the plain snapshot the GUI reads each frame.
struct testresult {
	bool started = false;
	bool finished = false;
	bool found = false;
	unsigned long long attempts = 0;
	double seconds = 0.0;
	double guesspersec = 0.0;
	std::string summary;
};


class bruteforceengine {
public:
	bruteforceengine();
	~bruteforceengine();

	// startTest brute-forces target over the given charset (the pool of allowed
	// characters) using threadcount worker threads. Returns immediately.
	void startTest(const std::string& target, const std::string& charset, int threadcount);

	// stop signals the workers to quit and joins them.
	void stop();

	// snapshot is thread-safe and cheap; call it from the GUI every frame.
	testresult snapshot() const;

private:
	// run is the controller thread: it launches the workers, joins them and then
	// writes the final verdict.
	void run();

	// worker searches the half-open keyspace range [startindex, endindex).
	void worker(unsigned long long startindex, unsigned long long endindex);

	// indexToCandidate turns a keyspace index into a candidate of the fixed
	// length, treating the charset as the digits of a base-(charset.size()) number.
	std::string indexToCandidate(unsigned long long index, int len) const;

	std::thread controller;
	std::vector<std::thread> workers;

	std::atomic<bool> found;
	std::atomic<bool> stoprequested;
	std::atomic<bool> running;
	std::atomic<bool> hasstarted;
	std::atomic<unsigned long long> attempts;

	mutable std::mutex statemutex;
	std::string summary;
	bool finished;
	std::string foundpassword;
	std::chrono::steady_clock::time_point starttime;
	std::chrono::steady_clock::time_point endtime;

	std::string target;
	std::string charset;
	int threadcount;
	int length;
	unsigned long long searchspace;
	double realkeyspace;
};

#endif /* BRUTEFORCEENGINE_H_ */
