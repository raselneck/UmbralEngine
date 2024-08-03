#include "Engine/Assert.h"
#include "Threading/Windows/WindowsThread.h"
#include "Templates/IsPointer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// https://stackoverflow.com/a/7827101
typedef long NTSTATUS;
typedef NTSTATUS __stdcall FNNTDELAYEXECUTION(IN BOOLEAN Alertable, IN PLARGE_INTEGER Interval);
using PFNNTDELAYEXECUTION = AddPointer<FNNTDELAYEXECUTION>;

/**
 * @brief Defines a context for calling NtDelayExecution.
 */
class FDelayExecutionContext final
{
public:

	/**
	 * @brief Loads ntdll.dll and retrieves NtDelayExecution from it.
	 */
	FDelayExecutionContext()
	{
		m_NtDllHandle = ::LoadLibraryA("ntdll.dll");
		UM_ASSERT(m_NtDllHandle != nullptr, "Failed to load ntdll.dll");

		m_NtDelayExecution = reinterpret_cast<PFNNTDELAYEXECUTION>(::GetProcAddress(m_NtDllHandle, "NtDelayExecution"));
		UM_ASSERT(m_NtDllHandle != nullptr, "Failed to find NtDelayExecution");
	}

	/**
	 * @brief Unloads ntdll.dll.
	 */
	~FDelayExecutionContext()
	{
		m_NtDelayExecution = nullptr;

		::FreeLibrary(m_NtDllHandle);
		m_NtDllHandle = nullptr;
	}

	/**
	 * @brief Delays execution for the given duration.
	 *
	 * @param duration The duration.
	 */
	void DelayExecution(const FTimeSpan duration) const
	{
		// According to the documentation of the undocumented NtDelayExecution function, the duration is in 100ns intervals.
		// That's perfect for us because all of our "tick"-based time tracking is also based in 100ns intervals.
		// http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FNT%20Objects%2FThread%2FNtDelayExecution.html

		LARGE_INTEGER delayInterval {};
		delayInterval.QuadPart = -duration.GetTicks(); // "Negative value means delay relative to current"

		m_NtDelayExecution(FALSE, &delayInterval);
	}

private:

	HMODULE m_NtDllHandle = nullptr;
	PFNNTDELAYEXECUTION m_NtDelayExecution = nullptr;
};

void FWindowsThread::Sleep(const FTimeSpan duration)
{
	if (duration.GetTicks() <= 0)
	{
		return;
	}

	static __declspec(thread) const FDelayExecutionContext delayExecutionContext;
	delayExecutionContext.DelayExecution(duration);
}