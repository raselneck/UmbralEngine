#include "Engine/Application.h"
#include "Engine/ContentManager.h"
#include "Engine/Engine.h"
#include "Engine/EngineViewport.h"
#include "Engine/EngineWindow.h"
#include "Input/InputManager.h"
#include "Graphics/GraphicsDevice.h"

void UEngineViewport::Draw(const FGameTime& gameTime)
{
	(void)gameTime;
}

TObjectPtr<UEngine> UEngineViewport::GetEngine() const
{
	TObjectPtr<UApplication> application = FindAncestorOfType<UApplication>();
	return application->GetEngine();
}

bool UEngineViewport::IsButtonDown(const EMouseButton button) const
{
	return m_InputManager->GetButtonState(button) == EButtonState::Pressed;
}

bool UEngineViewport::IsButtonUp(const EMouseButton button) const
{
	return m_InputManager->GetButtonState(button) == EButtonState::Released;
}

bool UEngineViewport::IsKeyDown(const EKey key) const
{
	return m_InputManager->GetKeyState(key) == EKeyState::Pressed;
}

bool UEngineViewport::IsKeyUp(const EKey key) const
{
	return m_InputManager->GetKeyState(key) == EKeyState::Released;
}

void UEngineViewport::Update(const FGameTime& gameTime)
{
	(void)gameTime;
}

bool UEngineViewport::WasButtonPressed(const EMouseButton button) const
{
	return m_InputManager->WasButtonPressed(button);
}

bool UEngineViewport::WasButtonReleased(const EMouseButton button) const
{
	return m_InputManager->WasButtonReleased(button);
}

bool UEngineViewport::WasKeyPressed(const EKey key) const
{
	return m_InputManager->WasKeyPressed(key);
}

bool UEngineViewport::WasKeyReleased(const EKey key) const
{
	return m_InputManager->WasKeyReleased(key);
}

void UEngineViewport::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Window = FindAncestorOfType<UEngineWindow>();

	TObjectPtr<UApplication> application = FindAncestorOfType<UApplication>();
	m_InputManager = application->GetInputManager(m_Window);
	m_GraphicsDevice = application->GetGraphicsDevice(m_Window);

	m_ContentManager = MakeObject<UContentManager>(m_GraphicsDevice);
}