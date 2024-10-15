#pragma once

#include "CoreMinimal.h"

#include "DialogueWidgetViewData.generated.h"

UCLASS()
class DIALOGUEMANAGER_API UDialogueResponseViewData : public UObject
{
	GENERATED_BODY()

public:
	FText Text;
	int Index = -1;
};

UCLASS()
class DIALOGUEMANAGER_API UDialogueOptionViewData : public UObject
{
	GENERATED_BODY()

public:
	FText MainText;
	TArray<UDialogueResponseViewData*> Responses;
};