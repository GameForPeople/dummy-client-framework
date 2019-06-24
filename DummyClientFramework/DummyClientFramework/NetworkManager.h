#pragma once

#include "CustomDefine.hh"

class DummyClientFramework;
class SendMemoryPool;

class NetworkManager
{
	friend class DummyClientFramework;
public:
	NetworkManager();
	~NetworkManager();

private:
	void InitNetwork();

	static void RunWorkerThread(LPVOID arg);
	void WorkerThreadFunction();

	void ConnectWithinMaxClient();
	void SetRecv(const _ClientIndexType inRecvClientIndex);

	void LoadRecvData(_ClientType* pClient, int restSize);

private:
	std::array<_ClientType*, FRAMEWORK::MAX_CLIENT> clientArr;
	std::shared_mutex clientArrLock;
	std::atomic<_ClientIndexType> connectedClientCount;

	std::array<std::thread, FRAMEWORK::WORKER_THREAD_COUNT> workerThreadArr;

	std::unique_ptr<SendMemoryPool> sendMemoryPool;

	HANDLE hIOCP;
};