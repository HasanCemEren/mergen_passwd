/*
 * bruteforceengine.cpp
 */

#include "bruteforceengine.h"
#include <cmath>
#include <cstdio>

// Upper bound on how many candidates we actually try. Beyond this a strong
// target is declared unbreakable and its time is estimated from the measured
// rate, so the program never runs forever.
static const unsigned long long maxattempts = 50000000ULL;

// powCapped returns base^exp, clamped to cap so it never overflows.
static unsigned long long powCapped(int base, int exp, unsigned long long cap) {
	unsigned long long r = 1;
	for(int i = 0; i < exp; i++) {
		if(r > cap / (unsigned long long)base) {
			return cap;
		}
		r *= (unsigned long long)base;
	}
	return r > cap ? cap : r;
}

static std::string formatSeconds(double s) {
	char buf[64];
	if(s < 1.0) {
		snprintf(buf, sizeof(buf), "%.0f ms", s * 1000.0);
	} else {
		snprintf(buf, sizeof(buf), "%.2f s", s);
	}
	return std::string(buf);
}

// verdictForSeconds turns an estimated crack time into a strength label.
static std::string verdictForSeconds(double secs) {
	const double hour = 3600.0, year = 86400.0 * 365.0;
	if(secs < hour) {
		return "Weak";
	}
	if(secs < year) {
		return "Medium";
	}
	if(secs < year * 1000.0) {
		return "Strong";
	}
	return "Very strong";
}

static std::string formatDuration(double secs) {
	if(secs <= 0.0) {
		return "instant";
	}
	const double minute = 60.0, hour = 3600.0, day = 86400.0, year = day * 365.0;
	char buf[64];
	if(secs < minute) {
		snprintf(buf, sizeof(buf), "%.1f seconds", secs);
		return std::string(buf);
	}
	if(secs < hour) {
		snprintf(buf, sizeof(buf), "%.1f minutes", secs / minute);
		return std::string(buf);
	}
	if(secs < day) {
		snprintf(buf, sizeof(buf), "%.1f hours", secs / hour);
		return std::string(buf);
	}
	if(secs < year) {
		snprintf(buf, sizeof(buf), "%.1f days", secs / day);
		return std::string(buf);
	}
	// Years: spell out the magnitude instead of scientific notation.
	double years = secs / year;
	if(years >= 1.0e12) {
		return "trillions of years";
	}
	if(years >= 1.0e9) {
		return "billions of years";
	}
	if(years >= 1.0e6) {
		return "millions of years";
	}
	if(years >= 1000.0) {
		snprintf(buf, sizeof(buf), "%lld years", (long long)years);
		return std::string(buf);
	}
	snprintf(buf, sizeof(buf), "%.0f years", years);
	return std::string(buf);
}

bruteforceengine::bruteforceengine()
		: found(false), stoprequested(false), running(false), hasstarted(false),
		  attempts(0), finished(false), threadcount(1), length(0), searchspace(0),
		  realkeyspace(0.0) {
}

bruteforceengine::~bruteforceengine() {
	stop();
}

void bruteforceengine::startTest(const std::string& target, const std::string& charset, int threadcount) {
	stop();

	this->target = target;
	this->charset = charset;
	this->threadcount = threadcount < 1 ? 1 : threadcount;
	found.store(false);
	stoprequested.store(false);
	running.store(true);
	hasstarted.store(true);
	attempts.store(0);
	foundpassword.clear();
	{
		std::lock_guard<std::mutex> lock(statemutex);
		finished = false;
		summary = "Running...";
	}

	int base = (int)this->charset.size();
	length = (int)this->target.size();
	starttime = std::chrono::steady_clock::now();
	if(base <= 0 || length <= 0) {
		std::lock_guard<std::mutex> lock(statemutex);
		summary = "Nothing to test.";
		endtime = starttime;
		finished = true;
		running.store(false);
		return;
	}

	realkeyspace = std::pow((double)base, (double)length);
	searchspace = powCapped(base, length, maxattempts);
	controller = std::thread(&bruteforceengine::run, this);
}

void bruteforceengine::run() {
	unsigned long long chunk = searchspace / (unsigned long long)threadcount;
	for(int t = 0; t < threadcount; t++) {
		unsigned long long s = (unsigned long long)t * chunk;
		unsigned long long e = (t == threadcount - 1) ? searchspace : (unsigned long long)(t + 1) * chunk;
		workers.emplace_back(&bruteforceengine::worker, this, s, e);
	}
	for(auto& w : workers) {
		if(w.joinable()) {
			w.join();
		}
	}
	workers.clear();

	auto end = std::chrono::steady_clock::now();
	double secs = std::chrono::duration<double>(end - starttime).count();
	unsigned long long att = attempts.load();
	double gps = secs > 0.0 ? (double)att / secs : 0.0;

	if(found.load()) {
		std::lock_guard<std::mutex> lock(statemutex);
		summary = "Weak - cracked in " + formatSeconds(secs);
		endtime = end;
		finished = true;
		running.store(false);
		return;
	}
	if(stoprequested.load()) {
		std::lock_guard<std::mutex> lock(statemutex);
		summary = "Stopped.";
		endtime = end;
		finished = true;
		running.store(false);
		return;
	}

	// Not found within the cap: estimate the full search from the measured rate.
	bool reachable = true;
	for(char c : target) {
		if(charset.find(c) == std::string::npos) {
			reachable = false;
			break;
		}
	}
	double esttotal = gps > 0.0 ? realkeyspace / gps : 0.0;
	std::lock_guard<std::mutex> lock(statemutex);
	if(!reachable) {
		summary = "Uncrackable here - uses characters outside the selected sets.";
	} else {
		summary = verdictForSeconds(esttotal) + " - about " + formatDuration(esttotal) + " to crack.";
	}
	endtime = end;
	finished = true;
	running.store(false);
}

void bruteforceengine::worker(unsigned long long startindex, unsigned long long endindex) {
	for(unsigned long long g = startindex; g < endindex; g++) {
		// Early exit the moment any thread wins or a stop is requested.
		if(found.load() || stoprequested.load()) {
			return;
		}
		std::string candidate = indexToCandidate(g, length);
		attempts.fetch_add(1, std::memory_order_relaxed);
		if(candidate == target) {
			{
				std::lock_guard<std::mutex> lock(statemutex);
				foundpassword = candidate;
			}
			found.store(true);
			return;
		}
	}
}

std::string bruteforceengine::indexToCandidate(unsigned long long index, int len) const {
	int base = (int)charset.size();
	std::string s(len, charset[0]);
	for(int i = len - 1; i >= 0; i--) {
		s[i] = charset[index % (unsigned long long)base];
		index /= (unsigned long long)base;
	}
	return s;
}

void bruteforceengine::stop() {
	stoprequested.store(true);
	if(controller.joinable()) {
		controller.join();
	}
	running.store(false);
}

testresult bruteforceengine::snapshot() const {
	testresult r;
	r.attempts = attempts.load();
	r.found = found.load();
	r.started = hasstarted.load();

	std::chrono::steady_clock::time_point end;
	{
		std::lock_guard<std::mutex> lock(statemutex);
		r.finished = finished;
		r.summary = summary;
		end = finished ? endtime : std::chrono::steady_clock::now();
	}
	double secs = std::chrono::duration<double>(end - starttime).count();
	if(secs < 0.0) {
		secs = 0.0;
	}
	r.seconds = secs;
	r.guesspersec = secs > 0.0 ? (double)r.attempts / secs : 0.0;
	return r;
}
