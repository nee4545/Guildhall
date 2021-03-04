#pragma once
class Job
{
public:
	Job();
	virtual ~Job();
	virtual void Execute() = 0;
	virtual void ClaimJobCallback();

protected:
	int m_jobID = 0;
	unsigned int m_jobType = 0;

};