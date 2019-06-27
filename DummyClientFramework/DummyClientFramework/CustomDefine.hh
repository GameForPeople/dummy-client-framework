#pragma once

namespace WINDOW
{
	constexpr static int WINDOW_WIDTH = 800;	// �������� X Size ���� ��, �������ּ���.
	constexpr static int WINDOW_HEIGHT = 800;	// �������� Y Size ���� ��, �������ּ���.

	constexpr static int MAIN_TIMER_INDEX = 1;
	constexpr static int MAIN_TIMER_FRAME = 1000; // WinAPI�� �������� ���� ��, �������ּ���. ex) 17(60FPS), 33(30FPS)
}

namespace FRAMEWORK
{
	constexpr static int MAX_CLIENT = 10000;	// �׽�Ʈ�Ϸ��� Ŭ���̾�Ʈ ���� �����ϰ��� �� ��, �������ּ���.
	constexpr static int WORKER_THREAD_COUNT = 4;	// ���� Ŭ���̾�Ʈ�� ����� ��Ŀ������ ������ �������ּ���.
	constexpr static int CONNECTED_CLIENT_COUNT_IN_ONE_FRAME = 10; // Max Client���� Ŀ��Ʈ �ϴ� ���� ��, �� ƽ�� Ŀ��Ʈ �õ��� Ŭ���̾�Ʈ ��.
	
	constexpr static int MAX_COUNT_MEMORY_UNIT_OF_SEND_POOL = 1000000;	// Send Memory Pool�� ���� �Ҵ� �޸� ���� �����Դϴ�.
	constexpr static int ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL = 100000;	// Send Memory Pool�� try_pop�� �����ϴ� ���(�޸�Ǯ�� ��), �޸�Ǯ�� �޸𸮸� �߰��Ҵ��մϴ�.
}

namespace GAME
{
	constexpr static int ZONE_MIN_X = 0;	// �׽�Ʈ �ϴ� ������ X ������ ��ǥ�Դϴ�.
	constexpr static int ZONE_MIN_Y = 0;	// �׽�Ʈ �ϴ� ������ Y ������ ��ǥ�Դϴ�.

	constexpr static int ZONE_MAX_X = 8000;	// �׽�Ʈ �ϴ� ������ X �ִ밪 ��ǥ�Դϴ�.
	constexpr static int ZONE_MAX_Y = 8000;	// �׽�Ʈ �ϴ� ������ Y �ִ밪 ��ǥ�Դϴ�.

	constexpr static int ZONE_X_SIZE = ZONE_MAX_X - ZONE_MIN_X;	// 
	constexpr static int ZONE_Y_SIZE = ZONE_MAX_Y - ZONE_MIN_Y; // 
	constexpr static int ACTOR_X_SIZE = /*WINDOW::WINDOW_WIDTH / ZONE_X_SIZE*/ + 1;
	constexpr static int ACTOR_Y_SIZE = /*WINDOW::WINDOW_WIDTH / ZONE_X_SIZE*/ +1;
}

namespace NETWORK
{
	const static std::string SERVER_IP = "127.0.0.1";
	constexpr static unsigned short SERVER_PORT = 9000;
	constexpr static int MAX_RECV_SIZE = 1024;	// �ѹ��� RECV�� ������ �ִ� ũ���Դϴ�. RECV�ϴ� �������� - ��Ŷ ��, 1024���� ū ��Ŷ�� ������ ���, �������ּ���.
	constexpr static int MAX_SEND_SIZE = 1024;	// �ѹ��� SEND�� ������ �ִ� ũ���Դϴ�. Send�ϴ� �������� - ��Ŷ ��, 1024���� ū ��Ŷ�� ������ ���, �������ּ���.
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
	struct BaseMemoryUnit // FIXED : �ش� ����ü�� ������ ��, ������������ ������ �� �ֽ��ϴ�.
	{
		BaseMemoryUnit(const bool isRecv);
		virtual ~BaseMemoryUnit();

	public:
		OVERLAPPED overlapped;
		WSABUF wsaBuf;

		const bool isRecv;	// true == recv, false == send
	};

	struct BaseClientInfo : public BaseMemoryUnit	// FIXED : �ش� ����ü�� ������ ��, ������������ ������ �� �ֽ��ϴ�.
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

	struct ClientInfo : public BaseClientInfo	// ��� ������ ���� �ش� ����ü�� �������ּ���.
	{
		ClientInfo(const _ClientIndexType key);
		virtual ~ClientInfo() override final;

	public:
		std::atomic<bool> isLogin = false;

		_PosType posX;
		_PosType posY;
	};
	using _ClientType = ClientInfo;	// ���ο� �̸��� Ŭ���̾�Ʈ ����ü ����� ��� ��, �ش� "ClientInfo" �κ��� �������ּ���.

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
