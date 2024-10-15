#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class DialogueEditor;
class FTabManager;

class FDialogueAssetAction : public FAssetTypeActions_Base
{
public:
	FDialogueAssetAction(EAssetTypeCategories::Type Category);

	FText GetName() const override;
	FColor GetTypeColor() const override;
	UClass* GetSupportedClass() const override;
	void OpenAssetEditor(const TArray<UObject*>& inObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	uint32 GetCategories() override;

private:
	EAssetTypeCategories::Type AssetCategory;
};

class DialogueAssetApplicationMode : public FApplicationMode
{
public:
	DialogueAssetApplicationMode(TSharedPtr<DialogueEditor> inApp);

	void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	void PreDeactivateMode() override;
	void PostActivateMode() override;

private:
	TWeakPtr<DialogueEditor> Application;
	FWorkflowAllowedTabSet Tabs;
};

class DialogueAssetPrimaryTabFactory : public FWorkflowTabFactory
{
public:
	DialogueAssetPrimaryTabFactory(TSharedPtr<DialogueEditor> inApp);

	TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<DialogueEditor> App;
};

class DialogueAssetPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	DialogueAssetPropertiesTabFactory(TSharedPtr<DialogueEditor> inApp);

	TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<DialogueEditor> App;
};