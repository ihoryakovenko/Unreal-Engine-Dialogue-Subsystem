#pragma once

#include "EdGraph/EdGraphNode.h"
#include "DialogueNodeInfo.h"

#include "DialogueGraphNode.generated.h"

class UToolMenu;
class UGraphNodeContextMenuContext;

UCLASS()
class UDialogueGraphNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	virtual UEdGraphPin* CreateDialoguePin(EEdGraphPinDirection direction, FName name) { return nullptr; };
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutputPins() { }

	virtual void InitNodeInfo(UObject* outer) { }
	virtual void SetNodeInfo(UDialogueNodeInfoBase* nodeInfo) { }
	virtual UDialogueNodeInfoBase* GetNodeInfo() const { return nullptr; }

	virtual EDialogueNodeType GetDialogueNodeType() const { return EDialogueNodeType::Unknown; }

	virtual void OnPropertiesChanged() { }
};

UCLASS()
class UDialogueGraphNode : public UDialogueGraphNodeBase
{
	GENERATED_BODY()

public:
	FText GetNodeTitle(ENodeTitleType::Type Type) const override;
	FLinearColor GetNodeTitleColor() const override;
	bool CanUserDeleteNode() const override;
	void GetNodeContextMenuActions(class UToolMenu* menu, class UGraphNodeContextMenuContext* Context) const override;

	UEdGraphPin* CreateDialoguePin(EEdGraphPinDirection Direction, FName Name) override;
	UEdGraphPin* CreateDefaultInputPin() override;
	void CreateDefaultOutputPins() override;

	EDialogueNodeType GetDialogueNodeType() const override;

	void OnPropertiesChanged();

	void SyncPinsWithResponses();

	void InitNodeInfo(UObject* inInfo);
	void SetNodeInfo(UDialogueNodeInfoBase* inInfo) override;
	UDialogueNodeInfoBase* GetNodeInfo() const override;
	UDialogueNodeInfo* GetDialogueNodeInfo();

private:
	UPROPERTY()
	UDialogueNodeInfo* Info;
};

UCLASS()
class UDialogueStartGraphNode : public UDialogueGraphNodeBase
{
	GENERATED_BODY()

public:
	FText GetNodeTitle(ENodeTitleType::Type Type) const override;
	FLinearColor GetNodeTitleColor() const override;
	bool CanUserDeleteNode() const override { return false; }

	UEdGraphPin* CreateDialoguePin(EEdGraphPinDirection Direction, FName Name) override;
	EDialogueNodeType GetDialogueNodeType() const override;
};

UCLASS()
class UDialogueEndGraphNode : public UDialogueGraphNodeBase
{
	GENERATED_BODY()

public:
	FText GetNodeTitle(ENodeTitleType::Type Type) const override;
	FLinearColor GetNodeTitleColor() const override;
	bool CanUserDeleteNode() const override;
	void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	UEdGraphPin* CreateDialoguePin(EEdGraphPinDirection Direction, FName Name) override;
	UEdGraphPin* CreateDefaultInputPin() override;

	void InitNodeInfo(UObject* inInfo);
	void SetNodeInfo(UDialogueNodeInfoBase* inInfo) override;
	UDialogueNodeInfoBase* GetNodeInfo() const override;
	EDialogueNodeType GetDialogueNodeType() const override;

	void OnPropertiesChanged();

private:
	UPROPERTY()
	UDialogueEndNodeInfo* Info;
};
