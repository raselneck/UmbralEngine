#include "Bocks/BocksEngine.h"
#include "Engine/Logging.h"
#include "Engine/Module.h"
#include "HAL/Directory.h"
#include "Threading/Thread.h"

class FBocksGameModule : public IGameModule
{
public:

	// IModule BEGIN
	virtual TSubclassOf<UEngine> GetEngineClass() const override;
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// IModule END
};

TSubclassOf<UEngine> FBocksGameModule::GetEngineClass() const
{
	return UBocksEngine::StaticType();
}

void FBocksGameModule::StartupModule()
{
	UM_LOG(Info, "Executable directory = \"{}\"", FDirectory::GetExecutableDir());
	UM_LOG(Info, "   Working directory = \"{}\"", FDirectory::GetWorkingDir());

	const FTimeSpan sleepDuration = FTimeSpan::FromMilliseconds(123.456);
	UM_LOG(Info, "Requesting to sleep for {} ms", sleepDuration.GetTotalMilliseconds());

	const FTimePoint sleepStart = FTimePoint::Now();
	FThread::Sleep(sleepDuration);
	const FTimePoint sleepEnd = FTimePoint::Now();

	const FTimeSpan actualSleepDuration = sleepEnd - sleepStart;
	UM_LOG(Info, "Actually slept for {} ms", actualSleepDuration.GetTotalMilliseconds());
}

void FBocksGameModule::ShutdownModule()
{
}

IMPLEMENT_GAME_MODULE(FBocksGameModule, Bocks)