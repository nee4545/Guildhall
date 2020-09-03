#pragma once
#include <deque>
#include <mutex>
#include <vector>
#include <thread>
#include <atomic>

class Job;

class JobSystemWorkerThread
{

public:
	void WorkerThreadMain();

	JobSystemWorkerThread();
	~JobSystemWorkerThread();

	std::thread* m_threadObject = nullptr;
};


class JobSystem
{

	friend class Job;
	friend class JobSystemWorkerThread;

public:
	JobSystem();
	~JobSystem();

	void StartUp();
	void ShutDown();
	void BeginFrame();

	void CreateWorkerThread();
	void PostJob( Job* job );
	void ClaimAndDeleteAllCompletedJobs();

public:
	std::deque <Job*> m_jobsQueued;
	std::deque <Job*> m_jobsCompleted;
	std::deque<Job*> m_jobsRunning;
	std::mutex m_jobsQueuedMutex;
	std::mutex m_jobsRunningMutex;
	std::mutex m_jobsCompletedMutex;

	std::vector<JobSystemWorkerThread*> m_workerThreads;

	std::atomic<bool> m_isQuitting;
	
};