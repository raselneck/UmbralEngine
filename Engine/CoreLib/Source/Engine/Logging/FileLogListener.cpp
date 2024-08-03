#include "Containers/String.h"
#include "Engine/Assert.h"
#include "Engine/Logging/FileLogListener.h"
#include "HAL/FileSystem.h"

FFileLogListener::~FFileLogListener()
{
	Close();
}

void FFileLogListener::Close()
{
	if (m_File.IsNull())
	{
		return;
	}

	m_File->Close();
	m_File.Reset();
}

void FFileLogListener::Flush()
{
	if (m_File.IsNull())
	{
		return;
	}

	m_File->Flush();
}

bool FFileLogListener::Open(const FStringView fileName)
{
	Close();

	m_File = FFileSystem::OpenWrite(fileName);
	return m_File.IsValid();
}

void FFileLogListener::Write(ELogLevel, const FStringView string) const
{
	UM_ASSERT(m_File.IsValid(), "Attempting to log to an un-opened file");

	m_File->Write(string);
}