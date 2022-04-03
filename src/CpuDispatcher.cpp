#include "cpuDispatcher.h"

#include <Thread>
#include <stack>
#include <chrono>


using namespace physx;
using namespace std;


static int nThreads = thread::hardware_concurrency();
static vector<thread> threads;
static stack<PxBaseTask*> tasks;

static volatile bool workersShouldTerminate = false;

static void workerProc() {
	while (not workersShouldTerminate) {
		if (!tasks.empty()) {
			auto task = tasks.top();
			tasks.pop();
			task->run();
		}
		else {
			this_thread::sleep_for(chrono::seconds(1));
		}
	}
}

CpuDispatcher::CpuDispatcher() {
	for (uint32_t i = 0; i < nThreads; i++) {
		threads.push_back(thread(workerProc));
	}
}
CpuDispatcher::~CpuDispatcher() {
	workersShouldTerminate = true;
	for (auto& thread : threads) {
		if (thread.joinable()) thread.join();
	}
}

void CpuDispatcher::submitTask(physx::PxBaseTask& task) {
	tasks.push(&task);
}
uint32_t CpuDispatcher::getWorkerCount() const {
	return nThreads;
}
