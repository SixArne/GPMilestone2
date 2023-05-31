#pragma once
#include <vector>
#include <memory>

class Timer
{
public:
	Timer();

	void Start();
	void Pause();
	void Update(float deltaTime);
	int GetID() const;

private:
	void Set(float maxTime, std::function<void()> OnCompleted, bool isPaused);
	void Reset();

	int m_TimerID{};
	static int s_TimerID;

	std::function<void()> m_OnCompleted{};

	float m_Timer{};
	float m_MaxTime{2.f};
	bool m_IsActive{ false };
	bool m_IsDone{ false };

	friend class TimerManager;
};

class TimerManager : public Singleton<TimerManager>
{
public:
	Timer* CreateTimer(float maxTime, std::function<void()> OnCompleted, bool isPaused = false);

	void Update(float deltaTime);

private:
	std::unordered_map<int, std::shared_ptr<Timer>> m_ActiveTimers{};
	std::vector<std::shared_ptr<Timer>> m_TimerPool{};

	virtual void Initialize() override;

	friend class Singleton<TimerManager>;
	friend class Timer;

	TimerManager() = default;
	~TimerManager() = default;
};

