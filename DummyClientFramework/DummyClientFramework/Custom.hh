#pragma once

#pragma region [더미 클라이언트 정의]

#define DUMMY_CLIENT_TEST_MODE 0 // 0 이면 디폴트 테스트 모드, 1이면 핫스팟 테스트 모드
#define DUMMY_CLIENT_SIGN_MODE 0 // 0 이면 로그인 처리, 1이면 계정 생성 처리

#if DUMMY_CLIENT_TEST_MODE == 0
	#define DEFAULT_TEST_MODE 
#else
	#define HOTSPOT_TEST_MODE	 // 핫 스팟 테스트 모드. 
#endif

#if DUMMY_CLIENT_SIGN_MODE == 0
	#define LOGIN_MODE   // 계정 로그인 모드
#else
	#define SIGNUP_MODE	 // 계정 생성 모드
#endif

#define USE_TIMER_MANAGER 1
#define USE_LOG_MANAGER	0	// 현재 정상적으로 동작하지 않습니다.
#define USE_PERFORMANCE_MANAGER 0 // 현재 동작하지 않습니다.
#define USE_RENDERMODEL_MANAGER 0 // 현재 동작하지 않습니다.

#pragma endregion

namespace WINDOW
{
	constexpr static int WINDOW_WIDTH = 800;	// 윈도우의 X Size 변경 시, 수정해주세요.
	constexpr static int WINDOW_HEIGHT = 800;	// 윈도우의 Y Size 변경 시, 수정해주세요.

	constexpr static int MAIN_TIMER_INDEX = 1;
	constexpr static int MAIN_TIMER_FRAME = 1000; // WinAPI의 프레임을 변경 시, 수정해주세요. ex) 17(60FPS), 33(30FPS)
}

namespace FRAMEWORK
{
	constexpr static int MAX_CLIENT = 1000;	// 테스트하려는 클라이언트 수를 변경하고자 할 때, 수정해주세요.
	constexpr static int WORKER_THREAD_COUNT = 3;	// 더미 클라이언트에 사용할 워커쓰레드 개수를 설정해주세요.
	constexpr static int CONNECTED_CLIENT_COUNT_IN_ONE_FRAME = 10; // Max Client까지 커넥트 하는 중일 떄, 한 틱에 커넥트 시도할 클라이언트 수.
	
	constexpr static int MAX_COUNT_MEMORY_UNIT_OF_SEND_POOL = 1000000;	// Send Memory Pool의 최초 할당 메모리 유닛 개수입니다.
	constexpr static int ALLOCATE_COUNT_MEMORY_UNIT_OF_SEND_POOL = 100000;	// Send Memory Pool의 try_pop가 실패하는 경우(메모리풀이 빔), 메모리풀에 메모리를 추가할당하는 사이즈입니다.

	constexpr static int PRE_ALLOCATION_TIMER_UNIT_COUNT = 100000; // Timer Unit Pool의 사이즈!

	constexpr static int MAX_ID_LENGTH = 10;
}

namespace GAME
{
	constexpr static int ZONE_MIN_X = 0;	// 테스트 하는 서버 Zone의 X 최저값 좌표입니다.
	constexpr static int ZONE_MIN_Y = 0;	// 테스트 하는 서버 Zone의 Y 최저값 좌표입니다.

	constexpr static int ZONE_MAX_X = 300;	// 테스트 하는 서버 Zone의 X 최대값 좌표입니다.
	constexpr static int ZONE_MAX_Y = 300;	// 테스트 하는 서버 Zone의 Y 최대값 좌표입니다.

	constexpr static int ZONE_X_SIZE = ZONE_MAX_X - ZONE_MIN_X;	// 
	constexpr static int ZONE_Y_SIZE = ZONE_MAX_Y - ZONE_MIN_Y; // 
	constexpr static int ACTOR_X_SIZE = ZONE_X_SIZE / 60;
	constexpr static int ACTOR_Y_SIZE = ZONE_Y_SIZE / 60;
}

namespace NETWORK
{
	const static std::string SERVER_IP = "127.0.0.1";
	constexpr static unsigned short SERVER_PORT = 9000;
	constexpr static int MAX_RECV_SIZE = 1000;	// 한번에 RECV가 가능한 최대 크기입니다.
	constexpr static int MAX_SEND_SIZE = 127;	// 한번에 SEND가 가능한 최대 크기입니다. Send하는 프로토콜 - 패킷 중, sizeof(char)보다 큰 사이즈의 패킷이 존재할 경우 해당 프레임워크를 사용할 수 없습니다.
	constexpr static int MAX_PACKET_SIZE = 127;	// Packet의 최대 크기입니다.  sizeof(char)보다 큰 사이즈의 패킷이 존재할 경우 해당 프레임워크를 사용할 수 없습니다.
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
	  NONE_TYPE	// 기본생성자.
	, AWAKE_TYPE // 로그인 true 패킷 전달시 처리하게되는 인자.
	, ATTACK_TYPE // 기본 공격
	, SKILL_TYPE // 스킬 타입
	, USE_ITEM1_TYPE // 아이템 타입
	, USE_ITEM2_TYPE // 아이템 타입
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
				unsigned char attackType;							//0이면 기본공격, 1이면 스킬1, 2이면 스킬2

				Attack(const unsigned char InAttackType ) noexcept;
			};
		}
	}
#pragma pack(pop)
}

