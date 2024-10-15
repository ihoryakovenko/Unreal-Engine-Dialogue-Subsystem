#include "DialogueSubsystem.h"

#include "DialogueGraph.h"
#include "DialogueAsset.h"
#include "UI/DialogueWidgetViewData.h"

DEFINE_LOG_CATEGORY(DialogueSubsystem);

void UDialogueSubsystem::InitiateDialogue(UDialogueAsset* Asset)
{
	if (!CanStartDialogue)
	{
		UE_LOG(DialogueSubsystem, Log, TEXT("Cant start dialogue %s"), *Asset->DialogueName);
		return;
	}

	UE_LOG(DialogueSubsystem, Log, TEXT("Started dialogue %s"), *Asset->DialogueName);

	ActiveAsset = Asset;
	DialoguePhase = 0;

	UDialogueGraph* Graph = Asset->Graph;
	if (Graph->Nodes.Num() < 0 && Graph->Nodes[0]->Type != EDialogueNodeType::Start)
	{
		UE_LOG(DialogueSubsystem, Error, TEXT("Invalid asset or start node %s"), *Asset->DialogueName);
		return;
	}

	ActiveNode = Graph->Nodes[0];

	DialogueStateChanged.Broadcast(EDialogueState::Started);

	SelectDialogueOption(0);
}

void UDialogueSubsystem::FinishDialogue()
{
	UE_LOG(DialogueSubsystem, Log, TEXT("Finished dialogue %s"), *ActiveAsset->DialogueName);

	ActiveNode = nullptr;
	ActiveAsset = nullptr;

	DialogueStateChanged.Broadcast(EDialogueState::Finished);
}

void UDialogueSubsystem::SelectDialogueOption(int Index)
{
	check(Index >= 0 && ActiveNode != nullptr);
	UE_LOG(DialogueSubsystem, Log, TEXT("Selected dialogue option %d"), Index);

	++DialoguePhase;

	if (Index >= ActiveNode->OutputPins.Num())
	{
		UE_LOG(DialogueSubsystem, Error, TEXT("Invalid dialogue option %d"), Index);
		return;
	}

	UDialoguePin* OutputPin = ActiveNode->OutputPins[Index];
	if (OutputPin->Connection != nullptr)
	{
		ActiveNode = OutputPin->Connection->Parent;
	}
	else
	{
		ActiveNode = nullptr;
	}

	if (ActiveNode != nullptr && ActiveNode->Type == EDialogueNodeType::Dialogue)
	{
		UDialogueNodeInfo* Info = Cast<UDialogueNodeInfo>(ActiveNode->Info);
		
		UDialogueOptionViewData* OptionViewData = NewObject<UDialogueOptionViewData>();
		OptionViewData->MainText = Info->DialogueText;

		for (int OptionIndex = 0; OptionIndex < Info->DialogueOptions.Num(); ++OptionIndex)
		{
			UDialogueResponseViewData* ResponseData = NewObject<UDialogueResponseViewData>();
			ResponseData->Text = Info->DialogueOptions[OptionIndex];
			ResponseData->Index = OptionIndex;
			OptionViewData->Responses.Add(ResponseData);
		}

		DialogueOptionSelected.Broadcast(OptionViewData);
	}
	else if (ActiveNode == nullptr || ActiveNode->Type == EDialogueNodeType::End)
	{
		FinishDialogue();
	}
}
