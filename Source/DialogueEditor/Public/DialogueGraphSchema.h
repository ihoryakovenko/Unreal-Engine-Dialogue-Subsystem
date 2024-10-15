#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"

#include "DialogueGraphSchema.generated.h"

UCLASS()
class UDialogueGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const override;
	void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
};

USTRUCT()
struct FNewNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewNodeAction() {}
	FNewNodeAction(UClass* ClassTemplate, FText inNodeCategory, FText inMenuDesc, FText inToolTip, const int32 inGrouping)
		: FEdGraphSchemaAction(inNodeCategory, inMenuDesc, inToolTip, inGrouping), ClassTemplate(ClassTemplate) { }

	UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

private:
	UClass* ClassTemplate;
};
