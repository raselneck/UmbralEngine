#include "Engine/Assert.h"
#include "HAL/TextStreamWriter.h"

static const FStringView GIndentString = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"_sv;

void FTextStreamWriter::ClearIndent()
{
	m_IndentLevel = 0;
}

int32 FTextStreamWriter::GetIndentLevel() const
{
	return m_IndentLevel;
}

void FTextStreamWriter::Indent()
{
	++m_IndentLevel;
}

void FTextStreamWriter::SetFileStream(TSharedPtr<IFileStream> fileStream)
{
	m_FileStream = MoveTemp(fileStream);
}

void FTextStreamWriter::Unindent()
{
	if (m_IndentLevel <= 0)
	{
		return;
	}

	--m_IndentLevel;
}

void FTextStreamWriter::Write(const FStringView string)
{
	if (string.IsEmpty())
	{
		return;
	}

	UM_ASSERT(CanWriteToStream(), "String writer cannot writer to underlying stream");

	m_FileStream->Write(static_cast<const void*>(string.GetChars()), static_cast<size_t>(string.Length()));
}

void FTextStreamWriter::WriteNoIndent(const FStringView string)
{
	Write(string);
}

void FTextStreamWriter::WriteLine(const FStringView string)
{
	// Write the indent characters
	int32 indent = m_IndentLevel;
	while (indent > GIndentString.Length())
	{
		Write(GIndentString);
		indent -= GIndentString.Length();
	}
	Write(GIndentString.Substring(0, indent));

	WriteLineNoIndent(string);
}

void FTextStreamWriter::WriteLineNoIndent(const FStringView string)
{
	Write(string);
	Write("\n"_sv);
}

bool FTextStreamWriter::CanWriteToStream() const
{
	return m_FileStream.IsValid() && m_FileStream->IsOpen();
}