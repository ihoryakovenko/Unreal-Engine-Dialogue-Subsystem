#pragma once

#include "CoreMinimal.h"

#include "DialogueNodeInfo.generated.h"

UENUM()
enum class EDialogueNodeType
{
    Unknown,
    Start,
    Dialogue,
    End
};

UENUM(BlueprintType)
enum class EDialogueNodeAction : uint8
{
    None,
    StartQuest
};

UCLASS(BlueprintType)
class DIALOGUEMANAGER_API UDialogueNodeInfoBase : public UObject
{
    GENERATED_BODY()
};

UCLASS(BlueprintType)
class DIALOGUEMANAGER_API UDialogueNodeInfo : public UDialogueNodeInfoBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FText Title;

    UPROPERTY(EditAnywhere)
    FText DialogueText;

    UPROPERTY(EditAnywhere)
    TArray<FText> DialogueOptions;
};

UCLASS(BlueprintType)
class DIALOGUEMANAGER_API UDialogueEndNodeInfo : public UDialogueNodeInfoBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    EDialogueNodeAction Action = EDialogueNodeAction::None;

    UPROPERTY(EditAnywhere)
    FString ActionData = TEXT("");
};
