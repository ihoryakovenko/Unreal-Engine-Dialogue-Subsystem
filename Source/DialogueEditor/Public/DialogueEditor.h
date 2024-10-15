#pragma once

#include "CoreMinimal.h"
#include "SGraphPanel.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class FTabManager;
class IToolkitHost;
class UDialogueAsset;
class UEdGraph;
class IDetailsView;
class UDialogueGraphNodeBase;

class DialogueEditor : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* inObject);

	FName GetToolkitFName() const override;
	FText GetBaseToolkitName() const override;
	FString GetWorldCentricTabPrefix() const override;
	FLinearColor GetWorldCentricTabColorScale() const override;

	UDialogueAsset* GetWorkingAsset();
	UEdGraph* GetWorkingGraph();

	void SetWorkingGraphUi(TSharedPtr<SGraphEditor> WorkingGraphUi);
	void SetSelectedNodeDetailView(TSharedPtr<IDetailsView> DetailsView);
	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection);

	void OnClose() override;
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event);
	void OnWorkingAssetPreSave();

private:
	void UpdateWorkingAssetFromGraph();
	void UpdateEditorGraphFromWorkingAsset();
	UDialogueGraphNodeBase* GetSelectedNode(const FGraphPanelSelectionSet& selection);

private:
	UPROPERTY()
	UDialogueAsset* WorkingAsset;

	UPROPERTY()
	UEdGraph* WorkingGraph;

	TSharedPtr<SGraphEditor> WorkingGraphUi;
	TSharedPtr<IDetailsView> SelectedNodeDetailView;
};
