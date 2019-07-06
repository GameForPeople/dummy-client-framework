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

	// 1. 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) ERROR_UTIL::ERROR_QUIT(L"WSAStartup()");

	// 2. 입출력 완료 포트 생성
	if (hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)
		; hIOCP == NULL) ERROR_UTIL:: ERROR_QUIT(TEXT("Make_WorkerThread()"));

	// 3. 워커 쓰레드 함수 할당.
	for (auto& thread : workerThreadArr) thread = std::thread{ RunWorkerThread, (LPVOID)this };

#ifdef _DEBUG
	std::cout << "[INIT] 더미 클라이언트가 정상적으로 실행되었습니다." << std::endl;
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
			ERROR_UTIL::ERROR_QUIT(L"retVal 혹은, cbTransferred가 0입니다. 아마 연결된 서버가 이상합니다.");
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
	char *pBuf = pClient->dataBuf; // pBuf -> 처리하는 문자열의 시작 위치
	char packetSize{ 0 }; // 처리해야할 패킷의 크기

	// 이전에 처리를 마치지 못한 버퍼가 있다면, 지금 처리해야할 패킷 사이즈를 알려줘.
	if (0 < pClient->loadedSize) packetSize = pClient->loadedBuf[0];

	// 처리하지 않은 버퍼의 크기가 있으면, 계속 루프문을 돕니다.
	while (recvSize > 0)
	{
		// 이전에 처리를 마치지 못한 버퍼를 처리해야한다면 패스, 아니라면 지금 처리해야할 패킷의 크기를 받음.
		if (packetSize == 0) packetSize = static_cast<int>(pBuf[0]);

		// 처리해야하는 패킷 사이즈 중에서, 이전에 이미 처리한 패킷 사이즈를 빼준다.
		int required = packetSize - pClient->loadedSize;

		// 패킷을 완성할 수 있을 때 (요청해야할 사이즈보다, 남은 사이즈가 크거나 같을 때)
		if (recvSize >= required)
		{
			memcpy(pClient->loadedBuf + pClient->loadedSize, pBuf, required);

			//-------------------------------------------------------------------------------
			ProcessPacket_CUSTOM(pClient); // 패킷처리 가가가가아아아아즈즈즈즞즈아아아아앗!!!!!!
			//-------------------------------------------------------------------------------

			pClient->loadedSize = 0;
			recvSize -= required;
			pBuf += required;
			packetSize = 0;
			packetSize = 0;
		}
		// 패킷을 완성할 수 없을 때
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
	//// 0. 최대 클라이언트 체크.
	//if (connectedClientCount == FRAMEWORK::MAX_CLIENT) return;

	for (int i = 0; i < FRAMEWORK::CONNECTED_CLIENT_COUNT_IN_ONE_FRAME; ++i)
	{
		// 0. 최대 클라이언트 수인지 체크.
		if (_ClientIndexType nowClientIndex = connectedClientCount.load();
			nowClientIndex == FRAMEWORK::MAX_CLIENT) return true; 
		else
		{
			// 1. 해당 인덱스의 소켓 생성.
			if (clientArr[nowClientIndex]->socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)
				; clientArr[nowClientIndex]->socket == INVALID_SOCKET)
			{
				ERROR_UTIL::ERROR_DISPLAY(L"Create_Socket()");
				return false;
			}


			// 2. 클라이언트 정보 구조체 객체 설정.
			SOCKADDR_IN serverAddr;
			ZeroMemory(&serverAddr, sizeof(serverAddr));
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_addr.s_addr = inet_addr(NETWORK::SERVER_IP.c_str());
			serverAddr.sin_port = htons(NETWORK::SERVER_PORT);
			
			// 3. 소켓과 입출력 완료 포트 연결.
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientArr[nowClientIndex]->socket), hIOCP, clientArr[nowClientIndex]->socket, 0);

			// 4. 커넥트!!!!!!!!! 가즈아아아아아앗!!!!!!!
			if (int retVal = connect(clientArr[nowClientIndex]->socket, (SOCKADDR*)& serverAddr, sizeof(serverAddr))
				; retVal == SOCKET_ERROR)
			{
				ERROR_UTIL::ERROR_DISPLAY(L"connect()");
				return false;
			}

			// 5. 해당 클라이언트 상태를 커넥트로 변경함.
			clientArr[nowClientIndex]->isConnect = true;

			// 6. 해당 소켓에 대하여, Recv를 등록해줌.
			SetRecv(nowClientIndex);

			// 7. 접속한 클라이언트 수를 하나 증가시켜줌.
			connectedClientCount.fetch_add(1);

			// 8. Connect에 대한 커스텀 함수 제공.
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
