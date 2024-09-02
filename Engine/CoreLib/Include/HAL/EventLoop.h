#pragma once

#include "Containers/Array.h"
#include "Memory/EnabledSharedFromThis.h"
#include "Memory/SharedPtr.h"
#include "Memory/UniquePtr.h"
#include "Misc/Badge.h"

class FEventLoop;

/**
 * @brief Defines the base for all event loop tasks.
 */
class IEventTask : public TEnableSharedFromThis<IEventTask>
{
	UM_DISABLE_COPY(IEventTask);
	UM_DISABLE_MOVE(IEventTask);

public:

	/**
	 * @brief Gets the event loop associated with this task.
	 *
	 * @return The event loop associated with this task.
	 */
	[[nodiscard]] TSharedPtr<FEventLoop> GetEventLoop() const;

	/**
	 * @brief Sets the event loop associated with this task.
	 *
	 * @param eventLoop The event loop associated with this task.
	 */
	void SetEventLoop(TBadge<FEventLoop>, const TSharedPtr<FEventLoop>& eventLoop);

protected:

	/**
	 * @brief Sets default values for this event task's properties.
	 */
	IEventTask() = default;

	/**
	 * @brief Destroys this async task.
	 */
	virtual ~IEventTask();

private:

	TWeakPtr<FEventLoop> m_EventLoop;
};

/**
 * @brief Defines a way to keep track of async events.
 */
class FEventLoop final : public TEnableSharedFromThis<FEventLoop>
{
	UM_DISABLE_COPY(FEventLoop);
	UM_DISABLE_MOVE(FEventLoop);

	struct FLoopDestructor
	{
		void Delete(struct uv_loop_s* loop);
	};

	using FLoopHandle = TUniquePtr<struct uv_loop_s, FLoopDestructor>;

public:

	/**
	 * @brief Sets default values for this event loop's properties.
	 *
	 * @param loop The libuv event loop.
	 */
	FEventLoop(TBadge<FEventLoop>, FLoopHandle loop);

	/**
	 * @brief Destroys this event loop.
	 */
	~FEventLoop();

	/**
	 * @brief Creates a new event loop.
	 *
	 * @return The event loop.
	 */
	[[nodiscard]] static TSharedPtr<FEventLoop> Create();

	/**
	 * @brief Adds a task to this event loop.
	 *
	 * @tparam TaskType The task's type.
	 * @tparam TaskArgTypes The types of the arguments to supply to the task's constructor.
	 * @param args The arguments to supply to the task's constructor.
	 * @return The handle to the task.
	 */
	template<typename TaskType, typename... TaskArgTypes>
	[[nodiscard]] TSharedPtr<TaskType> AddTask(TaskArgTypes... args)
	{
		// TODO CLion complains about AddTask not being implemented if this is a requires() clause...
		static_assert(IsBaseOf<IEventTask, TaskType>, "Tasks must inherit from IEventTask");

		TSharedPtr<TaskType> task = MakeShared<TaskType>(Forward<TaskArgTypes>(args)...);
		RegisterTask(task);
		return task;
	}

	/**
	 * @brief Checks to see if this event loop is empty.
	 *
	 * @return True if there are no tasks being run in this event loop, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_Tasks.IsEmpty();
	}

	/**
	 * @brief Gets the number of tasks being run in this event loop.
	 *
	 * @return The number of tasks being run in this event loop.
	 */
	[[nodiscard]] int32 NumTasks() const
	{
		return m_Tasks.Num();
	}

	/**
	 * @brief Polls all owned tasks for any remaining work to do.
	 */
	void PollTasks();

	/**
	 * @brief Removes the given task from this event loop.
	 *
	 * @param task The task to remove.
	 */
	void RemoveTask(TBadge<IEventTask>, IEventTask* task);

private:

	/**
	 * @brief Default constructor for event loops is not allowed.
	 */
	FEventLoop() = delete;

	/**
	 * @brief Registers the given task with this event loop.
	 *
	 * @param task The task.
	 */
	void RegisterTask(TSharedPtr<IEventTask> task);

	TArray<TSharedPtr<IEventTask>> m_Tasks;
	FLoopHandle m_Loop;
};