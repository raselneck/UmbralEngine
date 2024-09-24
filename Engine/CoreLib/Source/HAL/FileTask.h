#pragma once

#include "Containers/String.h"
#include "Engine/Error.h"
#include "HAL/EventLoop.h"
#include "HAL/File.h"
#include <uv.h>

// Thanks, libuv, for including Windows.h :^)
#ifdef CloseFile
#   undef CloseFile
#endif
#ifdef DeleteFile
#   undef DeleteFile
#endif
#ifdef GetMessage
#   undef GetMessage
#endif
#ifdef OpenFile
#   undef OpenFile
#endif

#ifndef S_IRWXU
enum : int32
{
	S_IXOTH = 00001, // others have execute permission
	S_IWOTH = 00002, // others have write permission
	S_IROTH = 00004, // others have read permission
	S_IRWXO = 00007, // others have read, write, and execute permission
	S_IXGRP = 00010, // group has execute permission
	S_IWGRP = 00020, // group has write permission
	S_IRGRP = 00040, // group has read permission
	S_IRWXG = 00070, // group has read, write, and execute permission
	S_IXUSR = 00100, // user has execute permission
	S_IWUSR = 00200, // user has write permission
	S_IRUSR = 00400, // user has read permission
	S_IRWXU = 00700, // user (file owner) has read, write, and execute permission
};
#endif

// TODO Incorporate file stat-ing into this class

/**
 * @brief Defines the base for file-based event tasks.
 */
class FFileTask : public IEventTask
{
	using Super = IEventTask;

public:

	/** Destructor type for uv_fs_t */
	struct FRequestDestructor
	{
		void Delete(uv_fs_t* handle)
		{
			uv_fs_req_cleanup(handle);
			FMemory::Free(handle);
		}
	};

	/** Memory managed handle to a libuv file request */
	using FRequestHandle = TUniquePtr<uv_fs_t, FRequestDestructor>;

	/**
	 * @brief Destroys this file task.
	 */
	virtual ~FFileTask() override = default;

	/**
	 * @brief Gets the handle to the currently opened file.
	 *
	 * @return The handle to the currently opened file.
	 */
	[[nodiscard]] uv_file GetFileHandle() const
	{
		return m_FileHandle;
	}

	/**
	 * @brief Gets the path to the currently opened file.
	 *
	 * @return The path to the currently opened file.
	 */
	[[nodiscard]] FStringView GetFilePath() const
	{
		return m_FilePath.AsStringView();
	}

	/**
	 * @brief Gets the associated libuv event loop.
	 *
	 * @return The associated libuv event loop.
	 */
	[[nodiscard]] uv_loop_t* GetLoop() const;

protected:

	/**
	 * @brief Sets default values for this file task's properties.
	 */
	FFileTask() = default;

	/**
	 * @brief Begins closing the associated file.
	 */
	void CloseFile();

	/**
	 * @brief Provides a function callback that can be used to dispatch a request.
	 *
	 * @tparam Callback The class function to call.
	 * @param request The request.
	 */
	template<void(FFileTask::*Callback)()>
	static void DispatchRequestCallback(uv_fs_t* request)
	{
		DispatchRequestCallback<FFileTask, Callback>(request);
	}

	/**
	 * @brief Provides a function callback that can be used to dispatch a request.
	 *
	 * @tparam T The class type.
	 * @tparam Callback The class function to call.
	 * @param request The request.
	 */
	template<typename T, void(T::*Callback)()>
	static void DispatchRequestCallback(uv_fs_t* request)
	{
		T* task = reinterpret_cast<T*>(request->data);
		(task->*Callback)();
	}

	/**
	 * @brief Makes a new request.
	 *
	 * @return The new request.
	 */
	[[nodiscard]] FRequestHandle MakeRequest() const;

	/**
	 * @brief Begins opening the file with the given path.
	 *
	 * @param filePath The file path.
	 * @param openFlags The open flags.
	 * @param openMode The open mode.
	 */
	void OpenFile(FStringView filePath, int32 openFlags, int32 openMode);

	/**
	 * @brief Called when an error occurs.
	 *
	 * @param error The error.
	 */
	virtual void OnError(FError error) { (void)error; }

	/**
	 * @brief Called when the file has been closed.
	 */
	virtual void OnFileClosed() { }

	/**
	 * @brief Called when the file has been opened.
	 */
	virtual void OnFileOpened() { }

	/**
	 * @brief Called when a file has been stat-ed.
	 *
	 * TODO Maybe supply the file path here
	 */
	virtual void OnFileStat(FFileStats stats) { (void)stats; }

	/**
	 * @brief Begin stat-ing a file.
	 *
	 * @param filePath The path to the file to stat.
	 */
	void StatFile(FStringView filePath);

private:

	// Make this private to force child classes to use GetLoop()
	using Super::GetEventLoop;

	/**
	 * @brief Handles when the underlying file is closed.
	 */
	void HandleFileClosed();

	/**
	 * @brief Handles when the underlying file is opened.
	 */
	void HandleFileOpened();

	/**
	 * @brief Handles when a file has been stat-ed.
	 */
	void HandleFileStat();

	FString m_FilePath;
	FRequestHandle m_OpenRequest;
	FRequestHandle m_CloseRequest;
	FRequestHandle m_StatRequest;
	uv_loop_t* m_Loop = nullptr;
	uv_file m_FileHandle = INDEX_NONE;
};