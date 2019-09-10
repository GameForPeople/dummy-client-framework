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
	if (isRecv == true)
		wsaBuf.len = NETWORK::MAX_RECV_SIZE;
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
	memoryUnit.wsaBuf.buf = loadedBuf;
}

ClientInfo::~ClientInfo()
{
}

#pragma endregion

//--------------------------------------
// Test Integrity
//--------------------------------------

void DummyClientFramework::TestIntegrity()
{
	// ���α׷��� ���� ����
	{
		MEMORY::BaseMemoryUnit tempMemoryUnit(0);
		assert((unsigned long)(&tempMemoryUnit) == (unsigned long)(&(tempMemoryUnit.overlapped)), L"BaseMemoryUnit ����ü�� �ʵ忡��, MemoryUnit(Overlap)�� �ֻ�ܿ� �������� �ʾ� ���������� ����� ������ �� �ֽ��ϴ�. ���� ������ �����Ͽ����ϴ�. ");
	}

	// PACKET_SIZE, 
	{
		static_assert(NETWORK::MAX_PACKET_SIZE < 128, L"NETWORK::MAX_PACKET_SIZE�� 128 �̻��� ���, �ش� �����ӿ�ũ�� �����ؾ��մϴ�. NetworkManager::LoadRecvData()�� �����Ͻð�, �� ���� �ܾ��� �����ϼ���. ");
		static_assert(NETWORK::MAX_SEND_SIZE < 128, L"NETWORK::MAX_SEND_SIZE�� 128 �̻��� ���, �ش� �����ӿ�ũ�� �����ؾ��մϴ�. NetworkManager::SendPacket()�� �����Ͻð�, �� ���� �ܾ��� �����ϼ���. ");
		
		{
			// �翬��~
			static_assert(NETWORK::MAX_RECV_SIZE > 0, L"NETWORK::MAX_RECV_SIZE�� 0�ϸ��� �����ϴ�.");
			static_assert(NETWORK::MAX_PACKET_SIZE > 0, L"NETWORK::MAX_PACKET_SIZE�� 0�ϸ��� �����ϴ�.");
			static_assert(NETWORK::MAX_SEND_SIZE > 0, L"NETWORK::MAX_SEND_SIZE�� 0�ϸ��� �����ϴ�.");
		}
	}
	//
}

//--------------------------------------
// EXAMPLE
//--------------------------------------

void NetworkManager::ProcessConnect_CUSTOM(_ClientType * pClient)
{
#ifdef LOGIN_MODE
	PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Login packet(std::to_wstring(pClient->index).c_str());
	SendPacket(pClient, reinterpret_cast<char*>(&packet));
#elif SIGNUP_MODE
	PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::SignUp packet(std::to_wstring(pClient->index).c_str());
	SendPacket(pClient, reinterpret_cast<char*>(&packet));
#endif
}

void NetworkManager::ProcessPacket_CUSTOM(_ClientType * pClient)
{
	using namespace PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT;
	using namespace PACKET_EXAMPLE::DATA::SERVER_TO_CLIENT;

	switch (pClient->loadedBuf[1])
	{
	case LOGIN_TRUE:
		{
			LoginTrue* packet = reinterpret_cast<LoginTrue*>(pClient->loadedBuf);
			
			//PUT_OBJECT���� �˻� �� �����ϵ��� ����.
			//if(packet->key == pClient->key) pClient->isLogin = true;
			pClient->key = packet->key;

			if (pClient->isLogin == false)
			{
				pClient->isLogin = true;
				std::cout << "LOGIN TRUE!\n";
			}
		}
		break;
	case LOGIN_FAIL:
		{
			std::cout << "LOGIN FAIL!\n";
		}
		break;
	case PUT_OBJECT:
		{
			PutObject* packet = reinterpret_cast<PutObject*>(pClient->loadedBuf);
			
			// �ڽ� ����, �ٸ� �÷��̾ ���� PutObject�� ����.
			if (packet->key == pClient->key)
			{
				pClient->posX = packet->posX;
				pClient->posY = packet->posY;
			}
		}
		break;
	case POSITION:
		{
			Position* packet = reinterpret_cast<Position*>(pClient->loadedBuf);

			if (packet->key == pClient->key)
			{
				pClient->posX = packet->posX;
				pClient->posY = packet->posY;
			}
		}
		break;
	default: 
		// �� ���� ��Ŷ�� ���ؼ��� �����մϴ�.
		break;
	}
}

void NetworkManager::ProcessUpdate_CUSTOM()
{
	// Ŭ���̾�Ʈ�� ���� ƽ����, ������ Connect�� �õ��ϴ� ������
	// NetworkManager::ConnectWithinMaxClient�� �̹� ���ǵǾ� ������,
	// framework�� update �κп���, �� �Լ� ���� ȣ��˴ϴ�.
	// ���� ������Ʈ���� �ؾ��� ������ �����ϸ� �˴ϴ�.
	// ���� 4���� �ϳ� �����ݴϴ�.
	int tempTrue{ 0 };
	int tempFalse{ 0 };

	for(auto pClient : clientArr)
	{
		if (pClient->isLogin)
		{
			PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Move packet(rand() % 4);
			SendPacket(pClient, reinterpret_cast<char*>(&packet));
			++tempTrue;
		}
		else ++tempFalse;
	}
	std::cout << "Update : " << tempTrue << ", " << tempFalse << "\n";
}

void NetworkManager::ProcessDecode_CUSTOM(_ClientType* pClient)
{
	/*
		�ش� ���������� � �͵� ���� �ʽ��ϴ�.
	*/
}

void NetworkManager::ProcessEncode_CUSTOM(SendMemoryUnit* pClient)
{
	/*
		�ش� ���������� � �͵� ���� �ʽ��ϴ�.
	*/
}

void TimerManager::ProcessTimerEvent_CUSTOM(TimerUnit* pTimerUnit)
{
}

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
		{}

		PutObject::PutObject(const _ClientIndexType key, const _PosType posX, const _PosType posY) noexcept
			: BasePacket(sizeof(PutObject), PACKET_EXAMPLE::TYPE::SERVER_TO_CLIENT::PUT_OBJECT)
			, key(key)
			, posX(posX), posY(posY)
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
	}
}