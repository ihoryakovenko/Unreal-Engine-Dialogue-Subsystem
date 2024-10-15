#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"

class FDialogueEditorModule : public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;

private:
	TSharedPtr<struct FDialoguePinFactory> PinFactory;
};
