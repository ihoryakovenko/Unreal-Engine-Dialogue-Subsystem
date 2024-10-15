#include "DialogueEditorModule.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraphPin.h"

#include "DialogueAssetHelpers.h"

#define LOCTEXT_NAMESPACE "FDialogueEditorModule"

class SDialogueGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogueGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

private:
	FSlateColor GetPinColor() const override
	{
		return FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f));
	}
};

class SDialogueStartGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogueGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f));
	}
};

class SDialogueEndGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SDialogueGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& inArgs, UEdGraphPin* inGraphPinObj)
{
		SGraphPin::Construct(SGraphPin::FArguments(), inGraphPinObj);
	}

protected:
	virtual FSlateColor GetPinColor() const override
{
		return FSlateColor(FLinearColor(0.0f, 0.0f, 1.0f));
	}
};

struct FDialoguePinFactory : public FGraphPanelPinFactory
{
public:
	TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* pin) const override
	{
		if (FName(TEXT("DialoguePin")) == pin->PinType.PinSubCategory)
		{
			return SNew(SDialogueGraphPin, pin);
		}
		else if (FName(TEXT("StartPin")) == pin->PinType.PinSubCategory)
		{
			return SNew(SDialogueStartGraphPin, pin);
		}
		else if (FName(TEXT("EndPin")) == pin->PinType.PinSubCategory)
		{
			return SNew(SDialogueEndGraphPin, pin);
		} 

		return nullptr;
	}
};

void FDialogueEditorModule::StartupModule()
{
	IAssetTools& assetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type assetType = assetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("DialogueAssets")), LOCTEXT("DialogueAssets", "Dialogue Assets"));
	TSharedPtr<FDialogueAssetAction> DialogueAssetAction = MakeShareable(new FDialogueAssetAction(assetType));
	assetToolsModule.RegisterAssetTypeActions(DialogueAssetAction.ToSharedRef());

	PinFactory = MakeShareable(new FDialoguePinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
}

void FDialogueEditorModule::ShutdownModule()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueEditorModule, DialogueEditor)
