#include "Engine/JobSystem.hpp"
#include "Engine/Job.hpp"
#include "Engine/Core/EngineCommon.hpp"

JobSystem::JobSystem()
{
	m_isQuitting = false;
}

JobSystem::~JobSystem()
{

}

void JobSystem::StartUp()
{

	int numThreads = std::thread::hardware_concurrency();
	for ( int i = 0; i < numThreads; i++ )
	{
		CreateWorkerThread();
	}
}

void JobSystem::ShutDown()
{
	m_isQuitting = true;

	for ( int i = 0; i < m_workerThreads.size(); i++ )
	{
		m_workerThreads[ i ]->m_threadObject->join();
		delete m_workerThreads[ i ];
	}
}

void JobSystem::BeginFrame()
{
	ClaimAndDeleteAllCompletedJobs();
}

void JobSystem::CreateWorkerThread()
{
	JobSystemWorkerThread* newThread = new JobSystemWorkerThread();
	m_workerThreads.push_back( newThread );
}

void JobSystem::PostJob( Job* job )
{
	//m_jobsQueuedMutex.lock();
	m_jobsQueued.push_back( job );
	//m_jobsQueuedMutex.unlock();
}

void JobSystem::ClaimAndDeleteAllCompletedJobs()
{
	std::deque<Job*> claimedJobs;

	g_theJobSystem->m_jobsCompletedMutex.lock();
	g_theJobSystem->m_jobsCompleted.swap( claimedJobs );
	g_theJobSystem->m_jobsCompletedMutex.unlock();

	for ( auto iter = claimedJobs.begin(); iter != claimedJobs.end(); ++iter )
	{
		Job* job = *iter;
		job->ClaimJobCallback();
		delete job;
	}
}

void JobSystemWorkerThread::WorkerThreadMain()
{
	while ( !g_theJobSystem->m_isQuitting )
	{
		g_theJobSystem->m_jobsQueuedMutex.lock();
		if ( !g_theJobSystem->m_jobsQueued.empty() )
		{
			Job* jobAtFront = g_theJobSystem->m_jobsQueued.front();
			g_theJobSystem->m_jobsQueued.pop_front();
			g_theJobSystem->m_jobsQueuedMutex.unlock();
			g_theJobSystem->m_jobsRunningMutex.lock();
			g_theJobSystem->m_jobsRunning.push_back( jobAtFront );
			g_theJobSystem->m_jobsRunningMutex.unlock();
			jobAtFront->Execute();

			g_theJobSystem->m_jobsCompleted.push_back( jobAtFront );

		}
		else
		{
			g_theJobSystem->m_jobsQueuedMutex.unlock();
			std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
		}
	}
}

JobSystemWorkerThread::JobSystemWorkerThread()
{
	m_threadObject = new std::thread( &JobSystemWorkerThread::WorkerThreadMain , this);
}

JobSystemWorkerThread::~JobSystemWorkerThread()
{
	delete m_threadObject;
}

