#pragma once

#pragma region [���� Ŭ���̾�Ʈ ����]

#define DUMMY_CLIENT_TEST_MODE 0 // 0 �̸� ����Ʈ �׽�Ʈ ���, 1�̸� �ֽ��� �׽�Ʈ ���
#define DUMMY_CLIENT_SIGN_MODE 0 // 0 �̸� �α��� ó��, 1�̸� ���� ���� ó��

#if DUMMY_CLIENT_TEST_MODE == 0
	#define DEFAULT_TEST_MODE 
#else
	#define HOTSPOT_TEST_MODE	 // �� ���� �׽�Ʈ ���. 
#endif

#if DUMMY_CLIENT_SIGN_MODE == 0
	#define LOGIN_MODE   // ���� �α��� ���
#else
	#define SIGNUP_MODE	 // ���� ���� ���
#endif

#define USE_TIMER_MANAGER 1
#define USE_LOG_MANAGER	0	// ���� ���������� �������� �ʽ��ϴ�.
#define USE_PERFORMANCE_MANAGER 0 // ���� �������� �ʽ��ϴ�.
#define USE_RENDERMODEL_MANAGER 0 // ���� �������� �ʽ��ϴ�.

#pragma endregion

namespace WINDOW
{
	constexpr static int WINDOW_WIDTH = 800;	// �������� X Size ���� ��, �������ּ���.
	constexpr static int WINDOW_HEIGHT = 800;	// �������� Y Size ���� ��, �������ּ���.

	constexpr static int MAIN_TIMER_INDEX = 1;
	constexpr static int MAIN_TIMER_FRAME = 1000; // WinAPI�� �������� ���� ��, �������ּ���. ex) 17(60FPS), 33(30FPS)
}

namespace FRAMEWORK
{
	constexpr static int MAX_CLIENT = 1000;	// �׽�Ʈ�Ϸ��� Ŭ���̾�Ʈ ���� �����ϰ��� �� ��, �������ּ���.
	constexpr static int WORKER_THREAD_COUNT = 3;	// ���� Ŭ���̾�Ʈ�� ����� ��Ŀ������ ������ �������ּ���.
	constexpr static int CONNECTED_CLIENT_COUNT_IN_ONE_FRAME = 10; // Max Client���� Ŀ��Ʈ �ϴ� ���� ��, �� ƽ�� Ŀ��Ʈ �õ��� Ŭ���̾�Ʈ ��.
	
	constexpr static int MAX_COUNT_MEMORY_UNIT_OF_SEND_POOL = 1000000;	// Send Memory Pool�� ���� �Ҵ� �޸� ���� �����Դϴ�.
	constexpr static int ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL = 100000;	// Send Memory Pool�� try_pop�� �����ϴ� ���(�޸�Ǯ�� ��), �޸�Ǯ�� �޸𸮸� �߰��Ҵ��ϴ� �������Դϴ�.

	constexpr static int PRE_ALLOCATION_TIMER_UNIT_COUNT = 100000; // Timer Unit Pool�� ������!

	constexpr static int MAX_ID_LENGTH = 10;
}

namespace GAME
{
	constexpr static int ZONE_MIN_X = 0;	// �׽�Ʈ �ϴ� ���� Zone�� X ������ ��ǥ�Դϴ�.
	constexpr static int ZONE_MIN_Y = 0;	// �׽�Ʈ �ϴ� ���� Zone�� Y ������ ��ǥ�Դϴ�.

	constexpr static int ZONE_MAX_X = 300;	// �׽�Ʈ �ϴ� ���� Zone�� X �ִ밪 ��ǥ�Դϴ�.
	constexpr static int ZONE_MAX_Y = 300;	// �׽�Ʈ �ϴ� ���� Zone�� Y �ִ밪 ��ǥ�Դϴ�.

	constexpr static int ZONE_X_SIZE = ZONE_MAX_X - ZONE_MIN_X;	// 
	constexpr static int ZONE_Y_SIZE = ZONE_MAX_Y - ZONE_MIN_Y; // 
	constexpr static int ACTOR_X_SIZE = ZONE_X_SIZE / 60;
	constexpr static int ACTOR_Y_SIZE = ZONE_Y_SIZE / 60;
}

namespace NETWORK
{
	const static std::string SERVER_IP = "127.0.0.1";
	constexpr static unsigned short SERVER_PORT = 9000;
	constexpr static int MAX_RECV_SIZE = 1000;	// �ѹ��� RECV�� ������ �ִ� ũ���Դϴ�.
	constexpr static int MAX_SEND_SIZE = 127;	// �ѹ��� SEND�� ������ �ִ� ũ���Դϴ�. Send�ϴ� �������� - ��Ŷ ��, sizeof(char)���� ū �������� ��Ŷ�� ������ ��� �ش� �����ӿ�ũ�� ����� �� �����ϴ�.
	constexpr static int MAX_PACKET_SIZE = 127;	// Packet�� �ִ� ũ���Դϴ�.  sizeof(char)���� ū �������� ��Ŷ�� ������ ��� �ش� �����ӿ�ũ�� ����� �� �����ϴ�.
}

namespace USING_DEFINE
{
	using _PosType = unsigned short;
	using _ClientIndexType = unsigned int;
	using _PacketSizeType = unsigned char;
	using _PacketType = unsigned char;
	using _DirectionType = unsigned char;
	using _TimeType = unsigned long long;
	using _IDType = wchar_t;

}using namespace USING_DEFINE;

enum class TIME : _TimeType
{
	  SECOND = 1000
	, MINUTE = 60000
	, ITEM1 = 2500
	, ITEM2 = 5000
	, SKILL = 1000
};

enum class TIMER_TYPE : unsigned char
{
	  NONE_TYPE	// �⺻������.
	, AWAKE_TYPE // �α��� true ��Ŷ ���޽� ó���ϰԵǴ� ����.
	, ATTACK_TYPE // �⺻ ����
	, SKILL_TYPE // ��ų Ÿ��
	, USE_ITEM1_TYPE // ������ Ÿ��
	, USE_ITEM2_TYPE // ������ Ÿ��
};

namespace MEMORY 
{
#pragma region [FIXED]
	struct BaseMemoryUnit
	{
		BaseMemoryUnit(const bool isRecv);
		~BaseMemoryUnit();

	public:
		OVERLAPPED overlapped;
		WSABUF wsaBuf;

		const bool isRecv;	// true == recv, false == send
	};

	struct SendMemoryUnit /*: public BaseMemoryUnit*/
	{
		SendMemoryUnit();
		~SendMemoryUnit();
		//virtual ~SendMemoryUnit() override final;

	public:
		BaseMemoryUnit memoryUnit;
		char dataBuf[NETWORK::MAX_SEND_SIZE];
	};

	struct ClientInfo /* : public BaseMemoryUnit */
	{
		ClientInfo(const _ClientIndexType key);
		~ClientInfo();
		//virtual ~BaseClientInfo() override;

	public: // Fixed
		BaseMemoryUnit memoryUnit;
		_ClientIndexType key;

		SOCKET socket;
		bool isConnect = false;
		const _ClientIndexType index;

		char dataBuf[NETWORK::MAX_RECV_SIZE];
		char loadedBuf[NETWORK::MAX_PACKET_SIZE];
		_PacketSizeType loadedSize;

#pragma endregion
	public: // Custom
		std::atomic<bool> isLogin = false;

		_PosType posX;
		_PosType posY;
	};
	using _ClientType = ClientInfo;	// 

}using namespace MEMORY;

namespace PACKET_EXAMPLE
{
	namespace TYPE
	{
		namespace SERVER_TO_CLIENT
		{
			enum
			{
				POSITION = 0,
				LOGIN_TRUE = 1,
				LOGIN_FAIL = 2,
				PUT_OBJECT = 3,
				ENUM_SIZE 
			};
		}

		namespace CLIENT_TO_SERVER
		{
			enum
			{
				MOVE = 0,
				LOGIN = 1,
				SIGN_UP = 2,
				ATTACK = 3,
				USE_ITEM = 4,
				ENUM_SIZE
			};
		}
	}

#pragma pack (push, 1)
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
				char paddingBuffer[40];

				LoginTrue(const _ClientIndexType key) noexcept;
			};

			struct PutObject : public BasePacket
			{
				const _ClientIndexType key;
				const _PosType posX;
				const _PosType posY;

				unsigned int paddingBuffer;

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

			struct Login : public BasePacket
			{
				_IDType id[FRAMEWORK::MAX_ID_LENGTH];

				Login(const _IDType* const pInNickname) noexcept;
			};

			struct SignUp : public BasePacket
			{
				_IDType id[FRAMEWORK::MAX_ID_LENGTH];
				int pb0;

				SignUp(const _IDType* const pInNickname) noexcept;
			};

			struct Attack : public BasePacket 
			{
				unsigned char attackType;							//0�̸� �⺻����, 1�̸� ��ų1, 2�̸� ��ų2

				Attack(const unsigned char InAttackType ) noexcept;
			};
		}
	}
#pragma pack(pop)
}

