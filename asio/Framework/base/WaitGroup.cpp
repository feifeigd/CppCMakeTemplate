#include "WaitGroup.h"

WaitGroup::~WaitGroup()
{
	wait();
}

void WaitGroup::add(int i)
{
	count_ += i;
}

void WaitGroup::done()
{
	if (--count_ <= 0)
	{
		cond_.notify_all();
	}
}

void WaitGroup::wait()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this] { return count_ <= 0; });
}
