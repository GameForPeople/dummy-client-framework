#include "stdafx.h"

#include <string>
#include "Custom.hh"
#include "NetworkManager.h"
#include "SendMemoryPool.h"

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
	: BaseMemoryUnit(false)
	, dataBuf()
{
	wsaBuf.buf = dataBuf;
}

SendMemoryUnit::~SendMemoryUnit()
{
}

//--------------------------------------
// Base Client Info
//--------------------------------------
BaseClientInfo::BaseClientInfo(const _ClientIndexType key)
	: BaseMemoryUnit(true)
	, key(key)
	, socket()
	, isConnect(false)
	, loadedBuf()
	, loadedSize(0)
{
	wsaBuf.buf = loadedBuf;
}

BaseClientInfo::~BaseClientInfo()
{
}

#pragma endregion

//--------------------------------------
// Client Info
//--------------------------------------
ClientInfo::ClientInfo(const _ClientIndexType key)
	: BaseClientInfo(key)
	, isLogin(false)
	, posX(0)
	, posY(0)
{
}

ClientInfo::~ClientInfo()
{
}


//--------------------------------------
// EXAMPLE
//--------------------------------------

void NetworkManager::ProcessConnect_CUSTOM(_ClientType * pClient)
{
	/*
		�ش� ���������� � �͵� ���� �ʽ��ϴ�.
	*/
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

				if(pClient->isLogin == false) pClient->isLogin = true;
				++connectedClientCount;
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
	for(auto pClient : clientArr)
	{
		if (pClient->isLogin)
		{
			PACKET_EXAMPLE::DATA::CLIENT_TO_SERVER::Move packet(rand() % 4);
			SendPacket(pClient, reinterpret_cast<char*>(&packet));
		}
	}
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
	}
}