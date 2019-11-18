#pragma once

//-----
#if USE_LOG_MANAGER == 1
#define WONSY_LOG_MANAGER // 비활성화를 희망할시 주석처리해주세요.
#endif
//-----

#ifdef WONSY_LOG_MANAGER
namespace WonSY
{
	enum class LOG_TYPE
	{
		WARNING_LOG
		, ERROR_LOG
		, DEV_LOG
	};

	class LogManager
	{
	public:
		_NODISCARD static inline LogManager* GetInstance() noexcept { return LogManager::instance; };

		static void MakeInstance();
		static void DeleteInstance();

		void AddLog(const LOG_TYPE, const std::wstring_view& logString);
		void SaveLog();

	private:
		const std::filesystem::path logFilePath;
		concurrency::concurrent_vector<std::pair<LOG_TYPE, std::wstring>> logCont;

	private:
		static inline LogManager* instance{ nullptr };

		LogManager();
		~LogManager() = default;

		static inline std::once_flag instanceFlag{};
		std::atomic<bool> saveFlag;

#ifdef _DEBUG
		std::mutex printLock;
#endif

	public:
		LogManager(const LogManager&) = delete;
		LogManager& operator=(const LogManager&) = delete;
	};
}
#endif