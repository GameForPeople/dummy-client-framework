#pragma once

#include "CustomDefine.hh"

class SendMemoryPool // Singleton
{
public:
	SendMemoryPool();
	~SendMemoryPool();

	_NODISCARD SendMemoryUnit* PopMemory();	// 메모리 제공
	void PushMemory(SendMemoryUnit*);	// 메모리 반납

private:
	Concurrency::concurrent_queue<SendMemoryUnit*> sendMemoryPool;
};