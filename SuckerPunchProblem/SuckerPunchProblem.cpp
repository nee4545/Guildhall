// SuckerPunchProblem.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <string>
#include <set>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // Used for GetCurrentTimeSeconds()
#include <time.h>    // User for rand()
#include <vector>

// Write a function that takes an unsigned integer as input, and returns true if all the digits in the base 10 representation of that number are unique.
/*
bool AllDigitsUnique( unsigned int Value )
{
	if ( Value < 10 )
	{
		return true;
	}

	bool uniqueNums[ 10 ] = { false };

	//std::set<int> uniqueNumbers;

	while ( Value > 0 )
	{
		int digit = Value % 10;

		if ( uniqueNums[ digit ] == false )
		{
			uniqueNums[ digit ] = true;
		}
		else
		{
			return false;
		}

		//auto itr = uniqueNumbers.find( digit );
		//
		//if ( itr == uniqueNumbers.end() )
		//{
		//	uniqueNumbers.insert( digit );
		//}
		//else
		//{
		//	return false;
		//}

		Value = Value / 10;
	}

	return true;
}
*/

bool AllDigitsUnique( unsigned int Value )
{
	// Keeps track of digits we've seen
	unsigned int UsedDigitsBitfield = 0;

	// Value turns to 0 when we hit the last digit
	while ( Value > 0 )
	{
		unsigned int CurrentDigit = ( 1 << ( Value % 10 ) );
		if ( UsedDigitsBitfield & CurrentDigit )
		{
			return false;
		}
		UsedDigitsBitfield |= CurrentDigit;

		// Move to the next digit
		Value /= 10;
	}

	return true;
}

void Check( unsigned int input , bool expected )
{
	bool result = AllDigitsUnique( input );
	if ( result == expected )
	{
		std::cout << "Passed!" << std::endl;
	}
	else
	{
		std::cout << "Failed!" << std::endl;
	}

}

LARGE_INTEGER GetCurrentCount()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter( &Result );
	return Result;
}

double GetSecondsPerCount()
{
	LARGE_INTEGER CountsPerSecond;
	QueryPerformanceFrequency( &CountsPerSecond );
	return 1.0 / static_cast< double >( CountsPerSecond.QuadPart );
}

/*
 * Helper function to get a high precision current time so we can accurately test our functions
 * @return Current elapsed time in seconds
 */
double GetCurrentTimeSeconds()
{
	// Setup our timer "start" reference point
	static LARGE_INTEGER InitialCount = GetCurrentCount();
	static double SecondsPerCount = GetSecondsPerCount();

	// Query current time
	LARGE_INTEGER CurrentCount = GetCurrentCount();
	LONGLONG ElapsedCounts = CurrentCount.QuadPart - InitialCount.QuadPart;
	return static_cast< double >( ElapsedCounts ) * SecondsPerCount;
}

int main()
{
	std::map < std::string , int > counts;

	counts.insert( { "is",0 } );
	counts.insert( { "si",1 } );
	counts.insert( { "ai",1 } );



	Check( 1234567890 , true );
	Check( 987654321 , true );
	Check( 0 , true );
	Check( 4294967295 , false );
	Check( 48778584 , false );
	Check( 17308459 , true );

	//------------------------------------------------------------------------------------------------
	// Question 1 - Efficiency Test Prep
	//------------------------------------------------------------------------------------------------
	unsigned int NumIterations = 5000;
	printf( "\nPreparing test...\n" );

	// Initialize random number generator
	srand( ( unsigned int ) time( NULL ) );

	std::vector<unsigned int> TestNumers( NumIterations );
	for ( unsigned int TestIndex = 0; TestIndex < NumIterations; ++TestIndex )
	{
		TestNumers[ TestIndex ] = rand();
	}

	//------------------------------------------------------------------------------------------------
	// Question 1 - Efficiency Test
	//------------------------------------------------------------------------------------------------
	printf( "Running %d iterations of AllDigitsUnique() with random numbers...\n" , NumIterations );

	// Counting the results so the function isn't optimized out and we know we're actually doing stuff
	int DigitsUnique = 0;
	int DigitsFailed = 0;

	double TestStartTime = GetCurrentTimeSeconds();
	for ( unsigned int TestIndex = 0; TestIndex < NumIterations; ++TestIndex )
	{
		if ( AllDigitsUnique( TestNumers[ TestIndex ] ) )
		{
			DigitsUnique += 1;
		}
		else
		{
			DigitsFailed += 1;
		}
	}
	double TestEndTime = GetCurrentTimeSeconds();
	double TotalTime = TestEndTime - TestStartTime;

	// Results
	printf( "Digits unique: %d\n" , DigitsUnique );
	printf( "Digits failed: %d\n" , DigitsFailed );
	printf( "Total Time: %.6f secs \n" , TotalTime );
	printf( "Avg.  Time: %.6f ms \n" , ( TotalTime * 1000.0 ) / static_cast< double >( NumIterations ) );
}

