#include "DialogueGraphNode.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"
#include "DialogueNodeInfo.h"

FText UDialogueGraphNode::GetNodeTitle(ENodeTitleType::Type Type) const
{ 
	UDialogueNodeInfo* NodeInfo = Cast<UDialogueNodeInfo>(Info);
	if (NodeInfo->Title.IsEmpty())
	{
		FString DialogueTextStr = NodeInfo->DialogueText.ToString();
		if (DialogueTextStr.Len() > 15)
		{
			DialogueTextStr = DialogueTextStr.Left(15) + TEXT("...");
		}

		return FText::FromString(DialogueTextStr);
	}

	return NodeInfo->Title;
}

FLinearColor UDialogueGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(FColor::Green);
}

bool UDialogueGraphNode::CanUserDeleteNode() const
{
	return true;
}

void UDialogueGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString(TEXT("Dialogue Node Actions")));
	UDialogueGraphNode* Node = const_cast<UDialogueGraphNode*>(this);

	Section.AddMenuEntry(TEXT("AddPinEntry"), FText::FromString(TEXT("Add Response")), FText::FromString(TEXT("Creates a new response")),
		FSlateIcon(TEXT("DialogueEditorStyle"), TEXT("DialogueEditor.NodeAddPinIcon")), FUIAction(FExecuteAction::CreateLambda(
			[Node] ()
			{
				Node->GetDialogueNodeInfo()->DialogueOptions.Add(FText::FromString(TEXT("Response")));
				Node->SyncPinsWithResponses();
				Node->GetGraph()->NotifyGraphChanged();
				Node->GetGraph()->Modify();
			})));
	
	Section.AddMenuEntry(TEXT("DeletePinEntry"), FText::FromString(TEXT("Delete Response")), FText::FromString(TEXT("Deletes the last response")),
		FSlateIcon(TEXT("DialogueEditorStyle"), TEXT("DialogueEditor.NodeDeletePinIcon")), FUIAction(FExecuteAction::CreateLambda(
			[Node] ()
			{
				UEdGraphPin* Pin = Node->GetPinAt(Node->Pins.Num() - 1);
				if (Pin->Direction != EEdGraphPinDirection::EGPD_Input)
				{
					UDialogueNodeInfo* Info = Node->GetDialogueNodeInfo();
					Info->DialogueOptions.RemoveAt(Info->DialogueOptions.Num() - 1);
					Node->SyncPinsWithResponses();
					Node->GetGraph()->NotifyGraphChanged();
					Node->GetGraph()->Modify();
				}
			})));

	Section.AddMenuEntry(TEXT("DeleteEntry"), FText::FromString(TEXT("Delete Node")), FText::FromString(TEXT("Deletes the node")),
		FSlateIcon(TEXT("DialogueEditorStyle"), TEXT("DialogueEditor.NodeDeleteNodeIcon")), FUIAction(FExecuteAction::CreateLambda(
			[Node] ()
			{
				Node->GetGraph()->RemoveNode(Node);
			})));
}

UEdGraphPin* UDialogueGraphNode::CreateDialoguePin(EEdGraphPinDirection Direction, FName Name)
{
	const FName Category = (Direction == EEdGraphPinDirection::EGPD_Input) ? TEXT("Inputs") : TEXT("Outputs");
	const FName Subcategory = TEXT("DialoguePin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = Subcategory;
	return Pin;
}

UEdGraphPin* UDialogueGraphNode::CreateDefaultInputPin()
{ 
	return CreateDialoguePin(EEdGraphPinDirection::EGPD_Input, TEXT("Display"));
}

void UDialogueGraphNode::CreateDefaultOutputPins()
{
	const FString DefaultResponse = TEXT("Continue");
	CreateDialoguePin(EEdGraphPinDirection::EGPD_Output, FName(DefaultResponse));
	GetDialogueNodeInfo()->DialogueOptions.Add(FText::FromString(DefaultResponse));
}

EDialogueNodeType UDialogueGraphNode::GetDialogueNodeType() const
{
	return EDialogueNodeType::Dialogue;
}

void UDialogueGraphNode::OnPropertiesChanged()
{
	SyncPinsWithResponses();
}

void UDialogueGraphNode::SyncPinsWithResponses()
{
	UDialogueNodeInfo* NodeInfo = GetDialogueNodeInfo();
	int NumGraphNodePins = Pins.Num() - 1;
	const int NumInfoPins = NodeInfo->DialogueOptions.Num();

	while (NumGraphNodePins > NumInfoPins)
	{
		RemovePinAt(NumGraphNodePins - 1, EEdGraphPinDirection::EGPD_Output);
		NumGraphNodePins--;
	}
	while (NumInfoPins > NumGraphNodePins)
	{
		CreateDialoguePin(EEdGraphPinDirection::EGPD_Output, FName(NodeInfo->DialogueOptions[NumGraphNodePins].ToString()));
		NumGraphNodePins++;
	}

	int Index = 1;
	for (const FText& Option : NodeInfo->DialogueOptions)
	{
		GetPinAt(Index)->PinName = FName(Option.ToString());
		Index++;
	}
}

void UDialogueGraphNode::InitNodeInfo(UObject* inInfo)
{
	Info = NewObject<UDialogueNodeInfo>(inInfo);
}

void UDialogueGraphNode::SetNodeInfo(UDialogueNodeInfoBase* inInfo)
{
	Info = Cast<UDialogueNodeInfo>(inInfo);
}

UDialogueNodeInfoBase* UDialogueGraphNode::GetNodeInfo() const
{
	return Info;
}

UDialogueNodeInfo* UDialogueGraphNode::GetDialogueNodeInfo()
{
	return Info;
}

FText UDialogueStartGraphNode::GetNodeTitle(ENodeTitleType::Type Type) const
{
	return FText::FromString("Start");
}

FLinearColor UDialogueStartGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(FColor::Red);
}

UEdGraphPin* UDialogueStartGraphNode::CreateDialoguePin(EEdGraphPinDirection Direction, FName Name)
{
	const FName category = TEXT("Outputs");
	const FName subcategory = TEXT("StartPin");

	UEdGraphPin* Pin = CreatePin(EEdGraphPinDirection::EGPD_Output, category, Name);
	Pin->PinType.PinSubCategory = subcategory;

	return Pin;
}

EDialogueNodeType UDialogueStartGraphNode::GetDialogueNodeType() const
{
	return EDialogueNodeType::Start;
}

FText UDialogueEndGraphNode::GetNodeTitle(ENodeTitleType::Type Type) const
{
	if (Info != nullptr && Info->Action != EDialogueNodeAction::None)
	{
		FString Result = UEnum::GetDisplayValueAsText(Info->Action).ToString();
		if (!Info->ActionData.IsEmpty())
		{
			FString ActionData = Info->ActionData;
			if (ActionData.Len() > 15)
			{
				ActionData = ActionData.Left(15) + TEXT("...");
			}

			Result += TEXT(" - ") + ActionData;
		}

		return FText::FromString(Result);
	}

	return FText::FromString(TEXT("End"));
}

FLinearColor UDialogueEndGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(FColor::Blue);
}

bool UDialogueEndGraphNode::CanUserDeleteNode() const
{
	return true;
}

void UDialogueEndGraphNode::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	FToolMenuSection& Section = Menu->AddSection(TEXT("SectionName"), FText::FromString(TEXT("End Node Actions")));
	UDialogueEndGraphNode* Node = const_cast<UDialogueEndGraphNode*>(this);

	Section.AddMenuEntry(TEXT("DeleteEntry"), FText::FromString(TEXT("Delete Node")), FText::FromString(TEXT("Deletes the node")),
		FSlateIcon(TEXT("DialogueEditorStyle"), TEXT("DialogueEditor.NodeDeleteNodeIcon")), FUIAction(FExecuteAction::CreateLambda(
			[Node]()
	{
		Node->GetGraph()->RemoveNode(Node);
	})));
}

UEdGraphPin* UDialogueEndGraphNode::CreateDialoguePin(EEdGraphPinDirection Direction, FName Name)
{
	const FName Category = TEXT("Inputs");
	const FName Subcategory = TEXT("EndPin");

	UEdGraphPin* Pin = CreatePin(Direction, Category, Name);
	Pin->PinType.PinSubCategory = Subcategory;
	return Pin;
}

UEdGraphPin* UDialogueEndGraphNode::CreateDefaultInputPin()
{
	return CreateDialoguePin(EEdGraphPinDirection::EGPD_Input, FName(TEXT("Finish")));
}

void UDialogueEndGraphNode::InitNodeInfo(UObject* inInfo)
{
	Info = NewObject<UDialogueEndNodeInfo>(inInfo);
}

void UDialogueEndGraphNode::SetNodeInfo(UDialogueNodeInfoBase* inInfo)
{
	Info = Cast<UDialogueEndNodeInfo>(inInfo);
}

UDialogueNodeInfoBase* UDialogueEndGraphNode::GetNodeInfo() const
{
	return Info;
}

EDialogueNodeType UDialogueEndGraphNode::GetDialogueNodeType() const
{
	return EDialogueNodeType::End;
}

void UDialogueEndGraphNode::OnPropertiesChanged()
{
	Modify();
}
