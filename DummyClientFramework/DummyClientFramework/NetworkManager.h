#pragma once

#include "Custom.hh"

class DummyClientFramework;
class SendMemoryPool;

class NetworkManager
{
	friend class DummyClientFramework;
public:
	NetworkManager(const int controlledClientIndex);
	~NetworkManager();

private:
	void InitNetwork();

	static void RunWorkerThread(LPVOID arg);
	void WorkerThreadFunction();

	bool ConnectWithinMaxClient();
	void SetRecv(const _ClientIndexType inRecvClientIndex);

	void LoadRecvData(_ClientType* pClient, int restSize);

	void SendPacket(const _ClientType* const pClient, const char* const packetData);
	void SendPacket(const int clientIndex, const char* const packetData);

private:
	void ProcessConnect_CUSTOM(_ClientType * pClient);
	void ProcessPacket_CUSTOM(_ClientType * pClient);
	void ProcessUpdate_CUSTOM();
	void ProcessDecode_CUSTOM(_ClientType* pClient);
	void ProcessEncode_CUSTOM(SendMemoryUnit* pClient);

private:
	std::atomic<_ClientIndexType> connectedClientCount;

	using _ClientArrType = std::array<_ClientType*, FRAMEWORK::MAX_CLIENT>;
	_ClientArrType	clientArr;
	std::shared_mutex clientArrLock;

	std::shared_ptr<_ClientType> controlledClient;
	/*_ClientIndexType*/ int controlledClientKey;	// atomic
	bool isFindControlledClient;

	std::array<std::thread, FRAMEWORK::WORKER_THREAD_COUNT> workerThreadArr;
	std::unique_ptr<SendMemoryPool>	sendMemoryPool;

	HANDLE hIOCP;

public:
	/*_NODISCARD */ inline _ClientIndexType GetConnectedClientCount() const noexcept { return connectedClientCount.load(); };

	inline bool GetIsFindControlledClient() const noexcept { return isFindControlledClient; };
	inline std::pair<_PosType, _PosType> GetControlledClientPosition() const noexcept { return std::make_pair(controlledClient->posX, controlledClient->posY); };
	inline void SetControlledMainClientKey(const _ClientIndexType inClientIndex) noexcept { controlledClientKey = inClientIndex; };
};
