#include "Engine/Job.hpp"

Job::Job()
{
	static int s_nexJobID = 1;
	m_jobID = s_nexJobID++;
}

Job::~Job()
{

}

void Job::ClaimJobCallback()
{

}
