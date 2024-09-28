#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "Engine/Module.h"
#include "GoogleTestEngine.h"
#include <gtest/gtest.h>

class FUmbralTestModule : public IGameModule
{
public:

	// IModule BEGIN
	void StartupModule() override;
	void ShutdownModule() override;
	// IModule END

protected:

	// IEngineModule BEGIN
	[[nodiscard]] virtual TObjectPtr<UEngine> CreateEngine(TBadge<FApplication>) override;
	// IEngineModule END
};

void FUmbralTestModule::StartupModule()
{
	// TODO Move this into the Google Test engine. Right now, the engine is created AFTER all of the tests
	FCommandLineArguments arguments = FCommandLine::GetMutableArguments();
	int32 argc = arguments.GetArgc();
	char** argv = arguments.GetArgv();

	::testing::InitGoogleTest(&argc, argv);

	(void)RUN_ALL_TESTS();
}

void FUmbralTestModule::ShutdownModule()
{
}

TObjectPtr<UEngine> FUmbralTestModule::CreateEngine(TBadge<FApplication>)
{
	return MakeObject<UGoogleTestEngine>();
}

IMPLEMENT_GAME_MODULE(FUmbralTestModule, UmbralTest)