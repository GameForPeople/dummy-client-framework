#pragma once

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
	const std::experimental::filesystem::path logFilePath;
	concurrency::concurrent_vector<std::pair<LOG_TYPE, std::wstring>> logCont;

private:
	static LogManager* instance;

	LogManager();
	~LogManager() = default;

	static std::once_flag instanceFlag;
	std::atomic<bool> saveFlag;

#ifdef _DEBUG
	std::mutex printLock;
#endif

public:
	LogManager(const LogManager&) = delete;
	LogManager& operator=(const LogManager&) = delete;
};