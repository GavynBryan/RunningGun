#pragma once

#include <core/Timer.h>
#include <core/engine/IService.h>
#include <functional>
#include <memory>
#include <vector>

class TimerService final : public IService
{
public:
	void Update() override;

	TimerHandle ScheduleTimer(float delay, std::function<void()> callback);
	void CancelTimer(TimerHandle handle);
	void Reset();

private:
	std::vector<std::unique_ptr<Timer>> Timers;
	TimerHandle NextTimerHandle = 1;
};
