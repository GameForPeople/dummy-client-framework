#include "stdafx.h"

#include "Custom.hh"
#include "DummyClientFramework.h"
#include "NetworkManager.h"
#include "TimerManager.h"

#pragma region [FIXED]
//--------------------------------------
// Base Memory Unit
//--------------------------------------
BaseMemoryUnit::BaseMemoryUnit(const bool isRecv)
	: isRecv(isRecv)
	, overlapped()
	, wsaBuf()
{
	if (isRecv == true) { wsaBuf.len = NETWORK::MAX_RECV_SIZE; }
}

BaseMemoryUnit::~BaseMemoryUnit()
{
}

//--------------------------------------
// Send Memory Unit
//--------------------------------------
SendMemoryUnit::SendMemoryUnit()
	: memoryUnit(false)
	, dataBuf()
{
	memoryUnit.wsaBuf.buf = dataBuf;
}

SendMemoryUnit::~SendMemoryUnit()
{
}

#pragma endregion

//--------------------------------------
// Client Info
//--------------------------------------

ClientInfo::ClientInfo(const _ClientIndexType index)
	: memoryUnit(true)
	, key()
	, socket()
	, isConnect(false)
	, loadedBuf()
	, loadedSize(0)
	, dataBuf()
	, isLogin(false)
	, posX(0)
	, posY(0)
	, index(index)
{
	memoryUnit.wsaBuf.buf = dataBuf;
}

ClientInfo::~ClientInfo()
{
}


//--------------------------------------
// Test Integrity
//--------------------------------------

void DummyClientFramework::TestIntegrity()
{
	// 프로그래밍 오류 제한
	{
		MEMORY::BaseMemoryUnit tempMemoryUnit(0);
		assert((unsigned long)(&tempMemoryUnit) == (unsigned long)(&(tempMemoryUnit.overlapped)), L"BaseMemoryUnit 구조체의 필드에서, MemoryUnit(Overlap)이 최상단에 생성되지 않아 비정상적인 결과를 도출할 수 있습니다. 서버 실행을 거절하였습니다. ");
	}

	// PACKET_SIZE, 
	{
		static_assert(NETWORK::MAX_PACKET_SIZE < 128, L"NETWORK::MAX_PACKET_SIZE가 128 이상일 경우, 해당 프레임워크를 수정해야합니다. NetworkManager::LoadRecvData()를 수정하시고, 이 정적 단언문을 제거하세요. ");
		static_assert(NETWORK::MAX_SEND_SIZE < 128, L"NETWORK::MAX_SEND_SIZE가 128 이상일 경우, 해당 프레임워크를 수정해야합니다. NetworkManager::SendPacket()를 수정하시고, 이 정적 단언문을 제거하세요. ");
		
		{
			// 당연쓰~
			static_assert(NETWORK::MAX_RECV_SIZE > 0, L"NETWORK::MAX_RECV_SIZE가 0일리가 없습니다.");
			static_assert(NETWORK::MAX_PACKET_SIZE > 0, L"NETWORK::MAX_PACKET_SIZE가 0일리가 없습니다.");
			static_assert(NETWORK::MAX_SEND_SIZE > 0, L"NETWORK::MAX_SEND_SIZE가 0일리가 없습니다.");
		}
	}
	//
}

//--------------------------------------
// EXAMPLE
//--------------------------------------

void NetworkManager::ProcessConnect_CUSTOM(_DO_NOT_DELETE _ClientType * pClient)
{
#ifdef LOGIN_MODE
	PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Login packet(std::to_wstring(pClient->index).c_str());
#elif SIGNUP_MODE
	PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::SignUp packet(std::to_wstring(pClient->index).c_str());
#endif

	SendPacket(pClient, reinterpret_cast<char*>(&packet));
}

void NetworkManager::ProcessPacket_CUSTOM(_DO_NOT_DELETE _ClientType * pClient)
{
	using namespace PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT;
	using namespace PACKET_EXAMPLE::DATA::SERVER_TO_CLIENT;

	switch (pClient->loadedBuf[1])
	{
	case LOGIN_TRUE:
		{
			LoginTrue* packet = reinterpret_cast<LoginTrue*>(pClient->loadedBuf);
			
			//PUT_OBJECT에서 검사 후 설정하도록 변경.
			//if(packet->key == pClient->key) pClient->isLogin = true;
			pClient->key = packet->key;
			//std::cout << "KEY - "<< pClient->key << "\n";

			if (pClient->isLogin == false) { pClient->isLogin = true; }
		} break;
	case LOGIN_FAIL:
		{
			std::cout << "LOGIN FAIL!\n";
		} break;
	case PUT_OBJECT:
		{
			PutObject* packet = reinterpret_cast<PutObject*>(pClient->loadedBuf);
			
			// 자신 말고, 다른 플레이어에 대한 PutObject는 무시.
			if (packet->key == pClient->key)
			{
				//std::cout << pClient->key << " -  x : " << pClient->posX << " , Y : " << pClient->posY << "\n";
				pClient->posX = packet->posX;
				pClient->posY = packet->posY;
			}
		} break;
	case POSITION:
		{
			Position* packet = reinterpret_cast<Position*>(pClient->loadedBuf);

			if (packet->key == pClient->key)
			{
				//std::cout << pClient->key << " -  x : " << pClient->posX << " , Y : " << pClient->posY << "\n";
				pClient->posX = packet->posX;
				pClient->posY = packet->posY;
			}
			// 동기화 측면에서 문제가 발생할 수 있는데, 치명적이지 않기 때문에 무시합니다.
			else if(packet->key == controlledClientKey)
			{
				isFindControlledClient = true;
				controlledClient->posX = packet->posX;
				controlledClient->posY = packet->posY;
			}
		} break;
	default: 
		// 이 외의 패킷에 대해서는 무시합니다.
		//std::cout << "unknown packet recved" << (int)(pClient->loadedBuf[1]) << std::endl;
		break;
	}
}

void NetworkManager::ProcessUpdate_CUSTOM()
{
	// 클라이언트가 일정 틱마다, 서버로 Connect를 시도하는 행위는
	// NetworkManager::ConnectWithinMaxClient에 이미 정의되어 있으며,
	// framework의 update 부분에서, 이 함수 전에 호출됩니다.
	// 따라서 업데이트마다 해야할 행위만 정의하면 됩니다.
	
	// 랜덤 4방향 하나 보내줍니다.
	for(const auto& pClient : clientArr)
	{
		if (pClient->isLogin)
		{
			PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Move packet(rand() % 4);
			SendPacket(pClient, reinterpret_cast<char*>(&packet));
		}
	}
}

void NetworkManager::ProcessDecode_CUSTOM(_DO_NOT_DELETE _ClientType* pClient)
{
	/*
		해당 예제에서는 어떤 것도 하지 않습니다.
	*/
}

void NetworkManager::ProcessEncode_CUSTOM(_DO_NOT_DELETE SendMemoryUnit* pClient)
{
	/*
		해당 예제에서는 어떤 것도 하지 않습니다.
	*/
}

#ifdef WONSY_TIMER_MANAGER
void TimerManager::ProcessTimerEvent_CUSTOM(_DO_NOT_DELETE TimerUnit* pTimerUnit)
{
	switch (pTimerUnit->timerType)
	{
		case TIMER_TYPE::AWAKE_TYPE:
		{
			switch (rand() % 4)
			{
				case 0:
				{
					TimerManager::GetInstance()->AddTimerEvent
					(
						TIMER_TYPE::ATTACK_TYPE,
						pTimerUnit->ownerKey,
						-1,
						TIME::SECOND
					);
				} break;
			
				case 1:
				{
					TimerManager::GetInstance()->AddTimerEvent
					(
						TIMER_TYPE::SKILL_TYPE,
						pTimerUnit->ownerKey,
						-1,
						TIME::SKILL
					);
				} break;

				case 2:
				{
					TimerManager::GetInstance()->AddTimerEvent
					(
						TIMER_TYPE::USE_ITEM1_TYPE,
						pTimerUnit->ownerKey,
						-1,
						TIME::ITEM1
					);
				} break;

				case 3:
				{
					TimerManager::GetInstance()->AddTimerEvent
					(
						TIMER_TYPE::USE_ITEM2_TYPE,
						pTimerUnit->ownerKey,
						-1,
						TIME::ITEM2
					);
				} break;
			}
		} break;

		case TIMER_TYPE::ATTACK_TYPE:
		{
			PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Attack packet(0);
			SendPacket(pTimerUnit->ownerKey, reinterpret_cast<char*>(&packet));
		} break;

		default:
		{
			assert(false, "뭐여");
		} break;
	}
}
#endif

namespace PACKET_EXAMPLE::DATA
{
	BasePacket::BasePacket(const _PacketSizeType size, const _PacketType type) noexcept
		: size(size)
		, type(type)
	{}

	namespace SERVER_TO_CLIENT
	{
		LoginTrue::LoginTrue(const _ClientIndexType key) noexcept
			: BasePacket(sizeof(LoginTrue), PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT::LOGIN_TRUE)
			, key(key)
			, paddingBuffer()
		{}

		PutObject::PutObject(const _ClientIndexType key, const _PosType posX, const _PosType posY) noexcept
			: BasePacket(sizeof(PutObject), PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT::PUT_OBJECT)
			, key(key)
			, posX(posX), posY(posY)
			, paddingBuffer()
		{}

		Position::Position(const _ClientIndexType key, const _PosType posX, const _PosType posY) noexcept
			: BasePacket(sizeof(Position), PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT::POSITION)
			, key(key)
			, posX(posX), posY(posY)
		{}
	}

	namespace CLIENT_TO_SERVER
	{
		Move::Move(const _DirectionType dir) noexcept
			: BasePacket(sizeof(Move), PACKET_EXAMPLE::TYPE::CLIENT_TO_SERVER::MOVE)
			, dir(dir)
		{}

		Login::Login(const _IDType* const pInId) noexcept
			: BasePacket(sizeof(Login), PACKET_EXAMPLE::TYPE::CLIENT_TO_SERVER::LOGIN)
			, id()
		{
			lstrcpynW(id, pInId, FRAMEWORK::MAX_ID_LENGTH);
		}

		SignUp::SignUp(const _IDType* const pInId) noexcept
			: BasePacket(sizeof(Login), PACKET_EXAMPLE::TYPE::CLIENT_TO_SERVER::SIGN_UP)
			, id()
		{
			lstrcpynW(id, pInId, FRAMEWORK::MAX_ID_LENGTH);
		}

		Attack::Attack(const unsigned char InAttackType) noexcept
			: BasePacket(sizeof(Attack), PACKET_EXAMPLE::TYPE::CLIENT_TO_SERVER::ATTACK)
			, attackType(InAttackType)
		{
		}
	}
}