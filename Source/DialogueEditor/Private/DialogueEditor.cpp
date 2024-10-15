#include "DialogueEditor.h"

#include "Kismet2/BlueprintEditorUtils.h"

#include "DialogueAsset.h"
#include "DialogueAssetHelpers.h"
#include "DialogueGraphSchema.h"
#include "DialogueGraphNode.h"
#include "DialogueNodeInfo.h"

DEFINE_LOG_CATEGORY_STATIC(DialogueEditorSub, Log, All);

void DialogueEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& inTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(inTabManager);
}

void DialogueEditor::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& initToolkitHost, UObject* inObject)
{
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(inObject);

	WorkingAsset = Cast<UDialogueAsset>(inObject);
	WorkingAsset->PreSaveDelegate = FOnPreSaveDelegate::CreateRaw(this, &DialogueEditor::OnWorkingAssetPreSave);

	WorkingGraph = FBlueprintEditorUtils::CreateNewGraph(WorkingAsset, NAME_None, UEdGraph::StaticClass(), UDialogueGraphSchema::StaticClass());

	InitAssetEditor(Mode, initToolkitHost, TEXT("DialogueEditor"), FTabManager::FLayout::NullLayout, true, true, ObjectsToEdit);

	AddApplicationMode(TEXT("DialogueAssetApplicationMode"), MakeShareable(new DialogueAssetApplicationMode(SharedThis(this))));
	SetCurrentMode(TEXT("DialogueAssetApplicationMode"));

	UpdateEditorGraphFromWorkingAsset();
}

UDialogueAsset* DialogueEditor::GetWorkingAsset()
{
	return WorkingAsset;
}

UEdGraph* DialogueEditor::GetWorkingGraph()
{
	return WorkingGraph;
}

void DialogueEditor::OnClose()
{
	UpdateWorkingAssetFromGraph();
	WorkingAsset->PreSaveDelegate.Unbind();
	FAssetEditorToolkit::OnClose();
}

void DialogueEditor::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if (WorkingGraphUi != nullptr)
	{
		UDialogueGraphNodeBase* DialogueNode = GetSelectedNode(WorkingGraphUi->GetSelectedNodes());
		if (DialogueNode != nullptr)
		{
			DialogueNode->OnPropertiesChanged();
		}

		WorkingGraphUi->NotifyGraphChanged();
	}
}

void DialogueEditor::OnWorkingAssetPreSave()
{
	UpdateWorkingAssetFromGraph();
}

void DialogueEditor::UpdateWorkingAssetFromGraph()
{
	if (WorkingAsset == nullptr || WorkingGraph == nullptr)
{
		return;
	}

	UDialogueGraph* RuntimeGraph = NewObject<UDialogueGraph>(WorkingAsset);
	WorkingAsset->Graph = RuntimeGraph;

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UDialoguePin*> idToPinMap;

	for (UEdGraphNode* GraphNode : WorkingGraph->Nodes)
	{
		UDialogueNode* DialogueNode = NewObject<UDialogueNode>(RuntimeGraph);
		DialogueNode->Position = FVector2D(GraphNode->NodePosX, GraphNode->NodePosY);

		for (UEdGraphPin* GraphPin : GraphNode->Pins)
		{
			UDialoguePin* DialoguePin = NewObject<UDialoguePin>(DialogueNode);
			DialoguePin->PinName = GraphPin->PinName;
			DialoguePin->PinId = GraphPin->PinId;
			DialoguePin->Parent = DialogueNode;

			if (GraphPin->HasAnyConnections() && GraphPin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				std::pair<FGuid, FGuid> Connection = std::make_pair(GraphPin->PinId, GraphPin->LinkedTo[0]->PinId);
				Connections.Add(Connection);
			}

			idToPinMap.Add(GraphPin->PinId, DialoguePin);
			if (GraphPin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				DialogueNode->InputPin = DialoguePin;
			}
			else
			{
				DialogueNode->OutputPins.Add(DialoguePin);
			}
		}

		UDialogueGraphNodeBase* GraphDialogueNode = Cast<UDialogueGraphNodeBase>(GraphNode);
		DialogueNode->Info = DuplicateObject(GraphDialogueNode->GetNodeInfo(), DialogueNode);
		DialogueNode->Type = GraphDialogueNode->GetDialogueNodeType();

		RuntimeGraph->Nodes.Add(DialogueNode);
	}

	for (std::pair<FGuid, FGuid>& Connection : Connections)
	{
		UDialoguePin* pin1 = idToPinMap[Connection.first];
		UDialoguePin* pin2 = idToPinMap[Connection.second];
		pin1->Connection = pin2;
	};
}

void DialogueEditor::UpdateEditorGraphFromWorkingAsset()
{
	if (WorkingAsset->Graph == nullptr)
	{
		WorkingGraph->GetSchema()->CreateDefaultNodesForGraph(*WorkingGraph);
		return;
	}

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UEdGraphPin*> IdToPinMap;

	for (UDialogueNode* DialogueNode : WorkingAsset->Graph->Nodes)
	{
		UDialogueGraphNodeBase* NewNode = nullptr;
		if (DialogueNode->Type == EDialogueNodeType::Start)
		{
			NewNode = NewObject<UDialogueStartGraphNode>(WorkingGraph);
		}
		else if (DialogueNode->Type == EDialogueNodeType::Dialogue)
		{
			NewNode = NewObject<UDialogueGraphNode>(WorkingGraph);
		}
		else if (DialogueNode->Type == EDialogueNodeType::End)
		{
			NewNode = NewObject<UDialogueEndGraphNode>(WorkingGraph);
		}
		else
		{
			UE_LOG(DialogueEditorSub, Error, TEXT("DialogueEditor::UpdateEditorGraphFromWorkingAsset: Unknown node type"));
			continue;
		}

		NewNode->CreateNewGuid();
		NewNode->NodePosX = DialogueNode->Position.X;
		NewNode->NodePosY = DialogueNode->Position.Y;
		
		if (DialogueNode->Info != nullptr)
		{
			NewNode->SetNodeInfo(DuplicateObject(DialogueNode->Info, NewNode));
		}
		else
		{
			NewNode->InitNodeInfo(NewNode);
		}

		if (DialogueNode->InputPin != nullptr)
		{
			UDialoguePin* DialoguePin = DialogueNode->InputPin;
			UEdGraphPin* GraphPin = NewNode->CreateDialoguePin(EEdGraphPinDirection::EGPD_Input, DialoguePin->PinName);
			GraphPin->PinId = DialoguePin->PinId;

			if (DialoguePin->Connection != nullptr)
			{
				Connections.Add(std::make_pair(DialoguePin->PinId, DialoguePin->Connection->PinId));
			}

			IdToPinMap.Add(DialoguePin->PinId, GraphPin);
		}

		for (UDialoguePin* DialoguePin : DialogueNode->OutputPins)
		{
			UEdGraphPin* GraphPin = NewNode->CreateDialoguePin(EEdGraphPinDirection::EGPD_Output, DialoguePin->PinName);
			GraphPin->PinId = DialoguePin->PinId;

			if (DialoguePin->Connection != nullptr)
			{
				Connections.Add(std::make_pair(DialoguePin->PinId, DialoguePin->Connection->PinId));
			}

			IdToPinMap.Add(DialoguePin->PinId, GraphPin);
		}

		WorkingGraph->AddNode(NewNode, true, true);
	}

	for (std::pair<FGuid, FGuid>& connection : Connections)
	{
		UEdGraphPin* FromPin = IdToPinMap[connection.first];
		UEdGraphPin* ToPin = IdToPinMap[connection.second];
		FromPin->LinkedTo.Add(ToPin);
		ToPin->LinkedTo.Add(FromPin);
	}
}


UDialogueGraphNodeBase* DialogueEditor::GetSelectedNode(const FGraphPanelSelectionSet& Selection)
{
	for (UObject* Object : Selection)
	{
		if (UDialogueGraphNodeBase* node = Cast<UDialogueGraphNodeBase>(Object))
		{
			return node;
		}
	}

	return nullptr;
}

void DialogueEditor::SetWorkingGraphUi(TSharedPtr<SGraphEditor> inWorkingGraphUi)
{
	WorkingGraphUi = inWorkingGraphUi;
}

void DialogueEditor::SetSelectedNodeDetailView(TSharedPtr<IDetailsView> DetailsView)
{ 
	SelectedNodeDetailView = DetailsView;
	SelectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &DialogueEditor::OnNodeDetailViewPropertiesUpdated);
}

void DialogueEditor::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection)
{
	UDialogueGraphNodeBase* SelectedNode = GetSelectedNode(Selection);
	if (SelectedNode != nullptr)
	{
		SelectedNodeDetailView->SetObject(SelectedNode->GetNodeInfo());
	}
	else
	{
		SelectedNodeDetailView->SetObject(nullptr);
	}
}

FName DialogueEditor::GetToolkitFName() const
{
	return FName(TEXT("DialogueEditorApplication"));
}

FText DialogueEditor::GetBaseToolkitName() const
{
	return FText::FromString(TEXT("DialogueEditorApplication"));
}

FString DialogueEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("DialogueEditorApplication");
}

FLinearColor DialogueEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);
}
