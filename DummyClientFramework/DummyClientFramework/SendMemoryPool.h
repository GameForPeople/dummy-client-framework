#pragma once

#include "Custom.hh"

class SendMemoryPool // Singleton
{
public:
	SendMemoryPool();
	~SendMemoryPool();

	_NODISCARD SendMemoryUnit* PopMemory();	// �޸� ����
	void PushMemory(SendMemoryUnit*);	// �޸� �ݳ�

private:
	Concurrency::concurrent_queue<SendMemoryUnit*> sendMemoryPool;
};