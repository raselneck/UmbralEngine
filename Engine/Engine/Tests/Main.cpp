#include "Engine/CommandLine.h"
#include "Engine/Logging.h"
#include "Engine/Module.h"
#include "GoogleTestEngine.h"
#include <gtest/gtest.h>

class FUmbralTestModule : public IGameModule
{
public:

	// IModule BEGIN
	[[nodiscard]] virtual TSubclassOf<UEngine> GetEngineClass() const override;
	void StartupModule() override;
	void ShutdownModule() override;
	// IModule END
};

TSubclassOf<UEngine> FUmbralTestModule::GetEngineClass() const
{
	return UGoogleTestEngine::StaticType();
}

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

IMPLEMENT_GAME_MODULE(FUmbralTestModule, UmbralTest)
