#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include <uv.h>

IEventTask::~IEventTask()
{
	TSharedPtr<FEventLoop> eventLoop = GetEventLoop();
	if (eventLoop.IsNull())
	{
		UM_LOG(Error, "Event task is being destroyed AFTER its event loop!");
		return;
	}

	constexpr TBadge<IEventTask> badge;
	eventLoop->RemoveTask(badge, this);

	m_EventLoop.Reset();
}

TSharedPtr<FEventLoop> IEventTask::GetEventLoop() const
{
	return m_EventLoop.Pin();
}

void IEventTask::SetEventLoop(TBadge<FEventLoop>, const TSharedPtr<FEventLoop>& eventLoop)
{
	m_EventLoop = TWeakPtr<FEventLoop> { eventLoop };
}

void FEventLoop::FLoopDestructor::Delete(uv_loop_t* loop)
{
	uv_loop_close(loop);

	FMemory::Free(loop);
}

FEventLoop::FEventLoop(TBadge<FEventLoop>, FLoopHandle loop)
	: m_Loop { MoveTemp(loop) }
{
	uv_loop_set_data(m_Loop.Get(), this);
}

FEventLoop::~FEventLoop()
{
	if (m_Tasks.IsEmpty())
	{
		return;
	}

	UM_LOG(Warning, "Event loop is being destroyed with tasks still pending!");

	for (const TSharedPtr<IEventTask>& task : m_Tasks)
	{
		constexpr TBadge<FEventLoop> badge;
		task->SetEventLoop(badge, nullptr);
	}

	uv_loop_set_data(m_Loop.Get(), nullptr);
}

TSharedPtr<FEventLoop> FEventLoop::Create()
{
	FLoopHandle loop { FMemory::AllocateObject<uv_loop_t>() };
	uv_loop_init(loop.Get());
	// TODO uv_loop_configure ?

	constexpr TBadge<FEventLoop> badge;
	return MakeShared<FEventLoop>(badge, MoveTemp(loop));
}

void FEventLoop::PollTasks()
{
	if (m_Tasks.IsEmpty())
	{
		return;
	}

	uv_update_time(m_Loop.Get());

	// Run the loop once without waiting for a task to be added if there are none
	uv_run(m_Loop.Get(), UV_RUN_NOWAIT);
}

void FEventLoop::RegisterTask(TSharedPtr<IEventTask> task)
{
	m_Tasks.Add(MoveTemp(task));
}

void FEventLoop::RemoveTask(TBadge<IEventTask>, IEventTask* task)
{
	UM_ENSURE(task != nullptr);

	const int32 taskIndex = m_Tasks.IndexOfByPredicate([task](const TSharedPtr<IEventTask>& existingTask)
	{
		return existingTask.Get() == task;
	});

	if (UM_ENSURE(taskIndex != INDEX_NONE))
	{
		m_Tasks.RemoveAt(taskIndex);
	}
}