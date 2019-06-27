#pragma once

namespace WINDOW
{
	constexpr static int WINDOW_WIDTH = 800;	// 윈도우의 X Size 변경 시, 수정해주세요.
	constexpr static int WINDOW_HEIGHT = 800;	// 윈도우의 Y Size 변경 시, 수정해주세요.

	constexpr static int MAIN_TIMER_INDEX = 1;
	constexpr static int MAIN_TIMER_FRAME = 1000; // WinAPI의 프레임을 변경 시, 수정해주세요. ex) 17(60FPS), 33(30FPS)
}

namespace FRAMEWORK
{
	constexpr static int MAX_CLIENT = 10000;	// 테스트하려는 클라이언트 수를 변경하고자 할 때, 수정해주세요.
	constexpr static int WORKER_THREAD_COUNT = 4;	// 더미 클라이언트에 사용할 워커쓰레드 개수를 설정해주세요.
	constexpr static int CONNECTED_CLIENT_COUNT_IN_ONE_FRAME = 10; // Max Client까지 커넥트 하는 중일 떄, 한 틱에 커넥트 시도할 클라이언트 수.
	
	constexpr static int MAX_COUNT_MEMORY_UNIT_OF_SEND_POOL = 1000000;	// Send Memory Pool의 최초 할당 메모리 유닛 개수입니다.
	constexpr static int ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL = 100000;	// Send Memory Pool의 try_pop가 실패하는 경우(메모리풀이 빔), 메모리풀에 메모리를 추가할당합니다.
}

namespace GAME
{
	constexpr static int ZONE_MIN_X = 0;	// 테스트 하는 서버의 X 최저값 좌표입니다.
	constexpr static int ZONE_MIN_Y = 0;	// 테스트 하는 서버의 Y 최저값 좌표입니다.

	constexpr static int ZONE_MAX_X = 8000;	// 테스트 하는 서버의 X 최대값 좌표입니다.
	constexpr static int ZONE_MAX_Y = 8000;	// 테스트 하는 서버의 Y 최대값 좌표입니다.

	constexpr static int ZONE_X_SIZE = ZONE_MAX_X - ZONE_MIN_X;	// 
	constexpr static int ZONE_Y_SIZE = ZONE_MAX_Y - ZONE_MIN_Y; // 
	constexpr static int ACTOR_X_SIZE = /*WINDOW::WINDOW_WIDTH / ZONE_X_SIZE*/ + 1;
	constexpr static int ACTOR_Y_SIZE = /*WINDOW::WINDOW_WIDTH / ZONE_X_SIZE*/ +1;
}

namespace NETWORK
{
	const static std::string SERVER_IP = "127.0.0.1";
	constexpr static unsigned short SERVER_PORT = 9000;
	constexpr static int MAX_RECV_SIZE = 1024;	// 한번에 RECV가 가능한 최대 크기입니다. RECV하는 프로토콜 - 패킷 중, 1024보다 큰 패킷이 존재할 경우, 수정해주세요.
	constexpr static int MAX_SEND_SIZE = 1024;	// 한번에 SEND가 가능한 최대 크기입니다. Send하는 프로토콜 - 패킷 중, 1024보다 큰 패킷이 존재할 경우, 수정해주세요.
}

namespace USING
{
	using _PosType = unsigned short;
	using _ClientIndexType = unsigned int;
	using _PacketSizeType = unsigned char;
	using _PacketType = unsigned char;
	using _DirectionType = unsigned char;

}using namespace USING;

namespace MEMORY 
{
	struct BaseMemoryUnit // FIXED : 해당 구조체를 수정할 시, 비정상적으로 동작할 수 있습니다.
	{
		BaseMemoryUnit(const bool isRecv);
		virtual ~BaseMemoryUnit();

	public:
		OVERLAPPED overlapped;
		WSABUF wsaBuf;

		const bool isRecv;	// true == recv, false == send
	};

	struct BaseClientInfo : public BaseMemoryUnit	// FIXED : 해당 구조체를 수정할 시, 비정상적으로 동작할 수 있습니다.
	{
		BaseClientInfo(const _ClientIndexType key);
		virtual ~BaseClientInfo() override;

	public:
		const _ClientIndexType key;

		SOCKET socket;
		bool isConnect = false;

		char dataBuf[NETWORK::MAX_SEND_SIZE];

		char loadedBuf[NETWORK::MAX_RECV_SIZE];
		unsigned short loadedSize;
	};

	struct ClientInfo : public BaseClientInfo	// 사용 목적에 따라 해당 구조체를 수정해주세요.
	{
		ClientInfo(const _ClientIndexType key);
		virtual ~ClientInfo() override final;

	public:
		std::atomic<bool> isLogin = false;

		_PosType posX;
		_PosType posY;
	};
	using _ClientType = ClientInfo;	// 새로운 이름의 클라이언트 구조체 사용을 희망 시, 해당 "ClientInfo" 부분을 수정해주세요.

	struct SendMemoryUnit : public BaseMemoryUnit
	{
		SendMemoryUnit();
		virtual ~SendMemoryUnit() override final;

	public:
		char dataBuf[NETWORK::MAX_SEND_SIZE];
	};
}using namespace MEMORY;

namespace PACKET_EXAMPLE
{
	namespace TYPE
	{
		namespace SERVER_TO_CLIENT
		{
			enum
			{
				LOGIN_TRUE = 0,
				PUT_OBJECT = 1,
				POSITION = 2,
				ENUM_SIZE 
			};
		}

		namespace CLIENT_TO_SERVER
		{
			enum
			{
				MOVE = 0,
				ENUM_SIZE
			};
		}
	}

	namespace DATA
	{
		struct BasePacket
		{
			const _PacketSizeType size;
			const _PacketType type;

			BasePacket(const _PacketSizeType size, const _PacketType type) noexcept;
		};

		namespace SERVER_TO_CLIENT
		{
			struct LoginTrue : public BasePacket
			{
				const _ClientIndexType key;

				LoginTrue(const _ClientIndexType key) noexcept;
			};

			struct PutObject : public BasePacket
			{
				const _ClientIndexType key;
				const _PosType posX;
				const _PosType posY;

				PutObject(const _ClientIndexType key, const _PosType posX, const _PosType posY) noexcept;
			};

			struct Position : public BasePacket
			{
				const _ClientIndexType key;
				const _PosType posX;
				const _PosType posY;

				Position(const _ClientIndexType key, const _PosType posX, const _PosType posY) noexcept;
			};
		}

		namespace CLIENT_TO_SERVER
		{
			struct Move : public BasePacket
			{
				const _DirectionType dir;

				Move(const _DirectionType dir) noexcept;
			};
		}
	}
}

namespace NETWORK_EXAMPLE
{
	void ProcessPacket(_ClientType * pClient);
}
