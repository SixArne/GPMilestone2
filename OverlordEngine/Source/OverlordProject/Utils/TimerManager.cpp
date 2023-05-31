#include "stdafx.h"
#include "TimerManager.h"

int Timer::s_TimerID{ 0 };

Timer::Timer()
	: m_TimerID{ s_TimerID++ }
{
}

void Timer::Start()
{
	m_IsActive = true;
}

void Timer::Pause()
{
	m_IsActive = false;
}

void Timer::Update(float deltaTime)
{
	if (!m_IsActive) return;
	m_Timer += deltaTime;
	if (m_Timer >= m_MaxTime)
	{
		m_IsDone = true;
		m_OnCompleted();
		Reset();
	}
}

int Timer::GetID() const
{
	return m_TimerID;
}

void Timer::Set(float maxTime, std::function<void()> OnCompleted, bool isPaused)
{
	m_OnCompleted = OnCompleted;
	m_MaxTime = maxTime;
	m_IsActive = !isPaused;
}

void Timer::Reset()
{
	m_IsActive = false;
	m_IsDone = false;
	m_Timer = 0.f;
	m_MaxTime = 2.0f;
	m_OnCompleted = nullptr;
}

Timer* TimerManager::CreateTimer(float maxTime, std::function<void()> OnCompleted, bool isPaused)
{
	if (m_TimerPool.size() > 0)
	{
		std::shared_ptr<Timer> timer = m_TimerPool.back();
		timer->Set(maxTime, OnCompleted, isPaused);

		m_ActiveTimers[timer->GetID()] = timer;
		m_TimerPool.pop_back();

		return timer.get();
	}

	return nullptr;
}

void TimerManager::Update(float deltaTime)
{

	for (auto& timer : m_ActiveTimers)
	{
		timer.second->Update(deltaTime);

		if (timer.second->m_IsDone)
		{
			m_TimerPool.emplace_back(timer.second);
			m_ActiveTimers.erase(timer.first);
		}
	}
}

void TimerManager::Initialize()
{
	for (int i{}; i < 10; i++)
	{
		m_TimerPool.emplace_back(std::make_shared<Timer>());
	}
}
