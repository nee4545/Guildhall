#include "pch.h"
#include "CppUnitTest.h"

#include "Engine/Core/UDPSocket.hpp"

#include <iostream>
#include <tuple>
#include <mutex>
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

#pragma comment(lib,"Ws2_32.lib")


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
AudioSystem* g_theAudio = nullptr;
InputSystem* g_theInput = nullptr;
Window* g_theWindow = nullptr;
#define TEST_MODE

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD_INITIALIZE( UDPSocketTestInitialize )
		{
			WSAData wsaData;
			WORD wVersion MAKEWORD( 2 , 2 );
			int iResult = WSAStartup( wVersion , &wsaData );
			if ( iResult != 0 )
			{
				std::cout << "Call to WSAStartup Failed " << WSAGetLastError() << std::endl;
			}
		}

		struct  UDPMessageHeader
		{
			std::uint16_t id;
			std::uint16_t length;
			std::uint32_t sequenceNo;
		};

		using TextArray = std::array<std::string , 5 >;
		TextArray text = {
			"Hello",
			"Watch out",
			"UDP message",
			"Thanks",
			"Goodbye"
		};
		std::uint16_t toU16( size_t input )
		{
			return static_cast< std::uint16_t >( input );
		}
		using MessageArray = std::array< std::tuple< std::uint16_t , std::uint16_t , std::uint16_t , std::string > , 5 >;
		MessageArray messages = {
			std::make_tuple( 1, 0, toU16( strlen( text[ 0 ].c_str() ) ) , text[ 0 ] ),
			std::make_tuple( 1, 1, toU16( strlen( text[ 1 ].c_str() ) ) , text[ 1 ] ),
			std::make_tuple( 1, 2, toU16( strlen( text[ 2 ].c_str() ) ) , text[ 2 ] ),
			std::make_tuple( 1, 3, toU16( strlen( text[ 3 ].c_str() ) ) , text[ 3 ] ),
			std::make_tuple( 0, 4, toU16( strlen( text[ 4 ].c_str() ) ) , text[ 4 ] ),
		};

		static void writer( UDPSocket& socket , MessageArray const& messages )
		{
			UDPMessageHeader header;
			for ( auto msg : messages )
			{
				header.id = std::get<0>( msg );
				header.length = std::get<2>( msg );
				header.sequenceNo = std::get<1>( msg );
				auto& buffer = socket.SendBuffer();
				*reinterpret_cast< UDPMessageHeader* >( &buffer[ 0 ] ) = header;
				memcpy( &( socket.SendBuffer()[ sizeof( UDPMessageHeader ) ] ) , std::get<3>( msg ).c_str() , header.length );
				socket.SendBuffer()[ sizeof( UDPMessageHeader ) + header.length ] = NULL;
				socket.Send( sizeof( UDPMessageHeader ) + header.length + 1 );
			}
		}
		static void reader( UDPSocket& socket , TextArray& messages )
		{
			static std::mutex lock;
			UDPMessageHeader const* pMsg = nullptr;
			std::string dataStr;
			size_t length = 0;
			do
			{
				length = socket.Receive();
				dataStr.clear();
				if ( length > 0 )
				{
					auto& buffer = socket.ReceiveBuffer();
					pMsg = reinterpret_cast< UDPMessageHeader const* >( &buffer[ 0 ] );
					if ( pMsg->length > 0 )
					{
						dataStr = &buffer[ sizeof( UDPMessageHeader ) ];
					}
					messages[ pMsg->sequenceNo ] = dataStr;
					{
						std::lock_guard<std::mutex> guard( lock );
						std::ostringstream ostr;
						ostr << "Thread [ " << std::this_thread::get_id() << " ] recieved Message"
							<< " id = " << pMsg->id
							<< " size = " << pMsg->length
							<< " sequence num = " << pMsg->sequenceNo
							<< " data = " << dataStr
							<< std::endl << std::ends;

						Logger::WriteMessage( ostr.str().c_str() );
					}
				}
			} while ( pMsg != nullptr && pMsg->id != 0 );
		}

		TEST_METHOD(UDPSocketUnitTest)
		{
			Logger::WriteMessage( "Starting UDP Socket Test" );
			UDPSocket test1UDPSocket( "127.0.0.1" , 48000 );
			test1UDPSocket.Bind( 48001 );
			UDPSocket test2UDPSocket( "127.0.0.1" , 48001 );
			test2UDPSocket.Bind( 48000 );
			TextArray readMessageOne;
			TextArray readMessageTwo;
			std::thread writerOne( &UnitTest::writer , std::ref( test1UDPSocket ) , std::cref( messages ) );

			std::thread readerOne( &UnitTest::reader , std::ref( test1UDPSocket ) , std::ref( readMessageOne ) );
			std::thread readerTwo( &UnitTest::reader , std::ref( test2UDPSocket ) , std::ref( readMessageTwo ) );
			std::thread writerTwo( &UnitTest::writer , std::ref( test2UDPSocket ) , std::cref( messages ) );

			writerOne.join();
			writerTwo.join();
			readerOne.join();
			readerTwo.join();
			Assert::IsTrue( text == readMessageOne );
			Assert::IsTrue( text == readMessageTwo );
		}

		TEST_METHOD_CLEANUP( UDPSocketTestCleanUp )
		{
			int iResult = WSACleanup();
			if ( iResult == SOCKET_ERROR )
			{
				std::cout << "Call to WSACleanup Failed " << WSAGetLastError() << std::endl;
			}
		}
	};
}
