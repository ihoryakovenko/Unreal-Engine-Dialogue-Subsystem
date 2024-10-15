#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"

#include "DialogueButton.generated.h"

class SDialogueButton : public SButton
{
public:
	void SetButtonHovered(bool IsHovered);
};

UCLASS()
class DIALOGUEMANAGER_API UDialogueButton : public UButton
{
	GENERATED_BODY()
	
public:
	// TODO: Remove from public and control hovered using focus
	// Main idea is to hover button on focus to support gamepads and keyboard navigation
	void SetButtonHovered(bool IsHovered);

protected:
	TSharedRef<SWidget> RebuildWidget() override;

private:
};
