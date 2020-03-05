#pragma once

#include <vector>

class Clock
{
public:

	Clock();                   // defaults to being owned by master
	Clock( Clock* parent );    
	~Clock();                  

	void Update( double deltaSeconds );    // usually do not need to call unless you create a new root clock
	void Reset();                       // set total time back to 0.0, does not reset children

	// Controls
	void Pause();
	void Resume();
	void SetScale( double scale );

	// accessors
	double GetTotalElapsedSeconds() const;
	double GetLastDeltaSeconds() const;
	double GetScale() const;
	bool IsPaused() const;


	void SetParent( Clock* clock );
	void AddChild( Clock* clock );

public:
	Clock* m_parent = nullptr;
	std::vector<Clock*> m_children;
	double m_currentTimeSeconds = 0.0;
	double m_frameDeltaSeconds = 0.0;
	double m_scale = 1.0;
	bool isPaused = false;


	static void SystemStartup();  
	static void SystemShutdown();
	static void BeginFrame();     

	static Clock* GetMaster();


	static Clock gMasterClock;

};