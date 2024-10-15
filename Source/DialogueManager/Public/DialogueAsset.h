#pragma once

#include "CoreMinimal.h"
#include "DialogueGraph.h"
#include <functional>

#include "DialogueAsset.generated.h"

DECLARE_DELEGATE(FOnPreSaveDelegate);

UCLASS(BlueprintType)
class DIALOGUEMANAGER_API UDialogueAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString DialogueName;

	UPROPERTY()
	UDialogueGraph* Graph;

	FOnPreSaveDelegate PreSaveDelegate;

public:
	void PreSave(FObjectPreSaveContext SaveContext) override;
 
};
