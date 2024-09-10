#include "Containers/Optional.h"
#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "HAL/EventLoop.h"
#include <uv.h>

namespace libuv
{
	void* Malloc(const size_t size)
	{
		return FMemory::Allocate(static_cast<FMemory::SizeType>(size));
	}

	void* Realloc(void* ptr, const size_t size)
	{
		return FMemory::Reallocate(ptr, static_cast<FMemory::SizeType>(size));
	}

	void* Calloc(const size_t count, const size_t size)
	{
		return FMemory::AllocateArray(static_cast<FMemory::SizeType>(count), static_cast<FMemory::SizeType>(size));
	}

	void Free(void* ptr)
	{
		FMemory::Free(ptr);
	}

	class FLifetimeHandler final
	{
	public:

		FLifetimeHandler()
			: m_Arguments { FCommandLine::GetMutableArguments() }
		{
			uv_replace_allocator(libuv::Malloc, libuv::Realloc, libuv::Calloc, libuv::Free);
			uv_setup_args(m_Arguments.GetArgc(), m_Arguments.GetArgv());
		}

		~FLifetimeHandler()
		{
			uv_library_shutdown();
		}

	private:

		FCommandLineArguments m_Arguments;
	};

	static TOptional<FLifetimeHandler> GLifetimeHandler;
}

IEventTask::~IEventTask()
{
	TSharedPtr<FEventLoop> eventLoop = GetEventLoop();
	if (eventLoop.IsNull())
	{
		UM_LOG(Error, "Event task is being destroyed AFTER its event loop!");
	}
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
	if (libuv::GLifetimeHandler.IsEmpty())
	{
		(void)libuv::GLifetimeHandler.EmplaceValue();
	}

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

	// Run the loop once without waiting for a task to be added if there are none
	uv_update_time(m_Loop.Get());
	uv_run(m_Loop.Get(), UV_RUN_NOWAIT);

	// TODO Need some kind of mechanism to automatically remove tasks when they're done, especially
	//      since not every task will be guaranteed to run inside of libuv
	m_Tasks.RemoveByPredicate([](const TSharedPtr<IEventTask>& task)
	{
		return task->IsRunning() == false;
	});
}

void FEventLoop::RegisterTask(TSharedPtr<IEventTask> task)
{
	if (task.IsNull())
	{
		return;
	}

	constexpr TBadge<FEventLoop> badge;
	task->SetEventLoop(badge, AsShared());

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