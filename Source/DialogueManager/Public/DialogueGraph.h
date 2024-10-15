#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "DialogueNodeInfo.h"

#include "DialogueGraph.generated.h"

UCLASS()
class DIALOGUEMANAGER_API UDialoguePin : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FName PinName;

    UPROPERTY()
    FGuid PinId;

    UPROPERTY()
    UDialoguePin* Connection;

    UPROPERTY()
    class UDialogueNode* Parent;
};

UCLASS()
class DIALOGUEMANAGER_API UDialogueNode : public UObject
{
    GENERATED_BODY()

public:
   UPROPERTY()
   EDialogueNodeType Type = EDialogueNodeType::Dialogue;

   UPROPERTY()
   UDialoguePin* InputPin;

   UPROPERTY()
   TArray<UDialoguePin*> OutputPins;

   UPROPERTY()
   FVector2D Position;

   UPROPERTY()
   UDialogueNodeInfoBase* Info;
};

UCLASS()
class DIALOGUEMANAGER_API UDialogueGraph : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<UDialogueNode*> Nodes;
};
