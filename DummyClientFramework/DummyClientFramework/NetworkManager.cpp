#include "stdafx.h"
#include "DummyClientFramework.h"
#include "Define.h"
#include "SendMemoryPool.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager()
	: clientArr()
	, clientArrLock()
	, connectedClientCount(0)
	, workerThreadArr()
	, hIOCP()
	, sendMemoryPool()
	//, isErrorOccurred(false)
{
	_ClientIndexType tempIndex = 0;
	for (auto& pClient : clientArr)
	{
		pClient = new _ClientType(tempIndex++);
	}

	sendMemoryPool = std::make_unique<SendMemoryPool>();

	InitNetwork();
}

NetworkManager::~NetworkManager()
{
	for (auto& pClient : clientArr)
	{
		if (pClient->isConnect == true)
		{
			pClient->isConnect = false;
			closesocket(pClient->socket);
		}
		
		delete pClient;
	}

	for (auto& thread : workerThreadArr) thread.join();
}

void NetworkManager::InitNetwork()
{
	WSADATA wsa;

	// 1. ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ERROR_UTIL::ERROR_QUIT(L"WSAStartup()");

	// 2. ����� �Ϸ� ��Ʈ ����
	if (hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)
		; hIOCP == NULL) ERROR_UTIL:: ERROR_QUIT(TEXT("Make_WorkerThread()"));

	// 3. ��Ŀ ������ �Լ� �Ҵ�.
	for (auto& thread : workerThreadArr) thread = std::thread{ RunWorkerThread, (LPVOID)this };

#ifdef _DEBUG
	std::cout << "[INIT] ���� Ŭ���̾�Ʈ�� ���������� ����Ǿ����ϴ�." << std::endl;
#endif
}

void NetworkManager::RunWorkerThread(LPVOID arg)
{
	NetworkManager* pNetworkManager = static_cast<NetworkManager*>(arg);
	pNetworkManager->WorkerThreadFunction();
};

void NetworkManager::WorkerThreadFunction()
{
	int retVal{};
	DWORD cbTransferred;
	unsigned long long Key{};
	LPVOID pMemoryUnit{ nullptr };

	while (7)
	{
		retVal = GetQueuedCompletionStatus(
			hIOCP,
			&cbTransferred,
			&Key,
			reinterpret_cast<LPOVERLAPPED*>(&pMemoryUnit),
			INFINITE
		);

		if (retVal == 0 || cbTransferred == 0)
		{
			ERROR_UTIL::ERROR_QUIT(L"retVal Ȥ��, cbTransferred�� 0�Դϴ�. �Ƹ� ����� ������ �̻��մϴ�.");
		}

		if (reinterpret_cast<BaseMemoryUnit*>(pMemoryUnit)->isRecv)
		{
			LoadRecvData(reinterpret_cast<_ClientType*>(pMemoryUnit), cbTransferred);
			SetRecv(reinterpret_cast<_ClientType*>(pMemoryUnit)->key);
		}
		else sendMemoryPool->PushMemory(reinterpret_cast<SendMemoryUnit*>(pMemoryUnit));
	}
}

void NetworkManager::SetRecv(const _ClientIndexType inClientIndex)
{
	DWORD flag{};
	ZeroMemory(&(clientArr[inClientIndex]->overlapped), sizeof(clientArr[inClientIndex]->overlapped));

	if (SOCKET_ERROR == WSARecv(clientArr[inClientIndex]->socket, &(clientArr[inClientIndex]->wsaBuf), 1, NULL, &flag /* NULL*/, &(clientArr[inClientIndex]->overlapped), NULL))
	{
		ERROR_UTIL::HandleRecvError();
	}
}

void NetworkManager::LoadRecvData(_ClientType* pClient, int recvSize)
{
	char *pBuf = pClient->dataBuf; // pBuf -> ó���ϴ� ���ڿ��� ���� ��ġ
	char packetSize{ 0 }; // ó���ؾ��� ��Ŷ�� ũ��

	// ������ ó���� ��ġ�� ���� ���۰� �ִٸ�, ���� ó���ؾ��� ��Ŷ ����� �˷���.
	if (0 < pClient->loadedSize) packetSize = pClient->loadedBuf[0];

	// ó������ ���� ������ ũ�Ⱑ ������, ��� �������� ���ϴ�.
	while (recvSize > 0)
	{
		// ������ ó���� ��ġ�� ���� ���۸� ó���ؾ��Ѵٸ� �н�, �ƴ϶�� ���� ó���ؾ��� ��Ŷ�� ũ�⸦ ����.
		if (packetSize == 0) packetSize = static_cast<int>(pBuf[0]);

		// ó���ؾ��ϴ� ��Ŷ ������ �߿���, ������ �̹� ó���� ��Ŷ ����� ���ش�.
		int required = packetSize - pClient->loadedSize;

		// ��Ŷ�� �ϼ��� �� ���� �� (��û�ؾ��� �������, ���� ����� ũ�ų� ���� ��)
		if (recvSize >= required)
		{
			memcpy(pClient->loadedBuf + pClient->loadedSize, pBuf, required);

			//-------------------------------------------------------------------------------
			ProcessPacket_CUSTOM(pClient); // ��Ŷó�� ���������ƾƾƾ�������j��ƾƾƾƾ�!!!!!!
			//-------------------------------------------------------------------------------

			pClient->loadedSize = 0;
			recvSize -= required;
			pBuf += required;
			packetSize = 0;
			packetSize = 0;
		}
		// ��Ŷ�� �ϼ��� �� ���� ��
		else
		{
			memcpy(pClient->loadedBuf + pClient->loadedSize, pBuf, recvSize);
			pClient->loadedSize += recvSize;
			break;
			//restSize = 0; 
		}
	}
}

bool NetworkManager::ConnectWithinMaxClient()
{
	//// 0. �ִ� Ŭ���̾�Ʈ üũ.
	//if (connectedClientCount == FRAMEWORK::MAX_CLIENT) return;

	for (int i = 0; i < FRAMEWORK::CONNECTED_CLIENT_COUNT_IN_ONE_FRAME; ++i)
	{
		// 0. �ִ� Ŭ���̾�Ʈ ������ üũ.
		if (_ClientIndexType nowClientIndex = connectedClientCount.load();
			nowClientIndex == FRAMEWORK::MAX_CLIENT) return true; 
		else
		{
			// 1. �ش� �ε����� ���� ����.
			if (clientArr[nowClientIndex]->socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)
				; clientArr[nowClientIndex]->socket == INVALID_SOCKET)
			{
				ERROR_UTIL::ERROR_DISPLAY(L"Create_Socket()");
				return false;
			}


			// 2. Ŭ���̾�Ʈ ���� ����ü ��ü ����.
			SOCKADDR_IN serverAddr;
			ZeroMemory(&serverAddr, sizeof(serverAddr));
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_addr.s_addr = inet_addr(NETWORK::SERVER_IP.c_str());
			serverAddr.sin_port = htons(NETWORK::SERVER_PORT);
			
			// 3. ���ϰ� ����� �Ϸ� ��Ʈ ����.
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientArr[nowClientIndex]->socket), hIOCP, clientArr[nowClientIndex]->socket, 0);

			// 4. Ŀ��Ʈ!!!!!!!!! ����ƾƾƾƾƾ�!!!!!!!
			if (int retVal = connect(clientArr[nowClientIndex]->socket, (SOCKADDR*)& serverAddr, sizeof(serverAddr))
				; retVal == SOCKET_ERROR)
			{
				ERROR_UTIL::ERROR_DISPLAY(L"connect()");
				return false;
			}

			// 5. �ش� Ŭ���̾�Ʈ ���¸� Ŀ��Ʈ�� ������.
			clientArr[nowClientIndex]->isConnect = true;

			// 6. �ش� ���Ͽ� ���Ͽ�, Recv�� �������.
			SetRecv(nowClientIndex);

			// 7. ������ Ŭ���̾�Ʈ ���� �ϳ� ����������.
			connectedClientCount.fetch_add(1);

			// 8. Connect�� ���� Ŀ���� �Լ� ����.
			ProcessConnect_CUSTOM(clientArr[nowClientIndex]);
		}

		return true;
	}
}

void NetworkManager::SendPacket(const _ClientType* const pClient, const char* const packetData)
{
	auto sendMemoryUnit = sendMemoryPool->PopMemory();
	memcpy(sendMemoryUnit->dataBuf, packetData, packetData[0]);
	sendMemoryUnit->wsaBuf.len = packetData[0];

	DWORD flag{};
	ZeroMemory(&sendMemoryUnit->overlapped, sizeof(sendMemoryUnit->overlapped));

	WSASend(pClient->socket, &sendMemoryUnit->wsaBuf, 1, NULL, 0, &sendMemoryUnit->overlapped, NULL);
}
