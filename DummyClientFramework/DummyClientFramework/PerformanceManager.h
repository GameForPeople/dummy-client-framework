#pragma once

//--
#if USE_PERFORMANCE_MANAGER == 1
#ifdef WONSY_LOG_MANAGER
#define WONSY_PERFORMANCE_MANAGER
#endif
#endif
//--

#ifdef WONSY_PERFORMANCE_MANAGER
namespace WonSY
{
	enum class FUNCTION_NAME : int
	{
		ENUM_SIZE = 1
	};

	struct PerformanceUnit
	{
		using _StartTimeType = std::chrono::steady_clock::time_point;
		using _ResultTimeType = std::chrono::milliseconds;
	public:

		FUNCTION_NAME function;
		_StartTimeType startTime;
		_ResultTimeType resultTime;

		PerformanceUnit(const FUNCTION_NAME);
		~PerformanceUnit();
	};

	class PerformanceManager
	{
		using _ResultContType = std::array<concurrency::concurrent_queue<PerformanceUnit::_ResultTimeType>, static_cast<int>(FUNCTION_NAME::ENUM_SIZE)>;
	public:
		_NODISCARD static inline PerformanceManager* GetInstance() noexcept { return PerformanceManager::instance; }

		static void MakeInstance();
		static void DeleteInstance();

		_NODISCARD std::unique_ptr<PerformanceUnit> StartTimer(const FUNCTION_NAME); //	void EndTimer( _PerformanceUnitType );

		void AddTimeResult(const PerformanceUnit& performanceUnit);
		_ResultContType& GetResultCont();

	private:
		static inline PerformanceManager* instance{ nullptr };
		_ResultContType resultCont;

		PerformanceManager();
		~PerformanceManager() = default;
	public:
		PerformanceManager(const PerformanceManager&) = delete;
		PerformanceManager& operator=(const PerformanceManager&) = delete;
	};
}
#endif