#pragma once

#include "noncopyable.hpp"

#include <condition_variable>
#include <mutex>

// 模拟 go 的 WaitGroup
class WaitGroup : private noncopyable
{
public:
	~WaitGroup();

	void add(int i = 1);
	void done();
	void wait();

private:
	std::mutex mutex_;
	std::condition_variable cond_;
	std::atomic<int> count_ = 0;
};
