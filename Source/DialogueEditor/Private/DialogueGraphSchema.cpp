#include "DialogueGraphSchema.h"

#include "DialogueGraphNode.h"
#include "DialogueNodeInfo.h"

void UDialogueGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewNodeAction> NewDialogueNodeAction(new FNewNodeAction(UDialogueGraphNode::StaticClass(),
		FText::FromString(TEXT("Nodes")), FText::FromString(TEXT("New Dialogue Node")),
		FText::FromString(TEXT("Makes a new Dialogue node")), 0));

	TSharedPtr<FNewNodeAction> NewEndNodeAction(new FNewNodeAction(UDialogueEndGraphNode::StaticClass(),
		FText::FromString(TEXT("Nodes")), FText::FromString(TEXT("New End Node")),
		FText::FromString(TEXT("Makes a new end node")), 0));

	ContextMenuBuilder.AddAction(NewDialogueNodeAction);
	ContextMenuBuilder.AddAction(NewEndNodeAction);
}

const FPinConnectionResponse UDialogueGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	if (PinA == nullptr || PinB == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need 2 pins"));
	}

	if (PinA->Direction == PinB->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_AB, TEXT(""));
}

void UDialogueGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	UDialogueStartGraphNode* StartNode = NewObject<UDialogueStartGraphNode>(&Graph);
	StartNode->CreateNewGuid();
	StartNode->NodePosX = 0;
	StartNode->NodePosY = 0;
	StartNode->CreateDialoguePin(EEdGraphPinDirection::EGPD_Output, FName(TEXT("Start")));
	
	Graph.AddNode(StartNode, true, true);
	Graph.Modify();
}

UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UDialogueGraphNodeBase* Result = NewObject<UDialogueGraphNodeBase>(ParentGraph, ClassTemplate);
	Result->CreateNewGuid();
	Result->NodePosX = Location.X;
	Result->NodePosY = Location.Y;
	Result->InitNodeInfo(Result);

	UEdGraphPin* InputPin = Result->CreateDefaultInputPin();
	Result->CreateDefaultOutputPins();

	if (FromPin != nullptr)
	{
		Result->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(Result, true, true);

	return Result;
}
