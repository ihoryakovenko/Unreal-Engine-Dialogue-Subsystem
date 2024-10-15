#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "DialogueButton.h"

#include "DialogueWidget.generated.h"

class UDialogueOptionViewData;

DECLARE_LOG_CATEGORY_EXTERN(DialogueWidget, Log, All);

UCLASS()
class DIALOGUEMANAGER_API UDialogueOptionEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void Init(int Index, const FText& Text);

protected:
	void NativeConstruct() override;
	void NativeOnItemSelectionChanged(bool bIsSelected) override;
	void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
	UPROPERTY(meta = (BindWidget))
	UDialogueButton* DialogueButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IndexText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResponseText;

	int OptionIndex = -1;

private:
	UFUNCTION()
	void OnDialogueButtonClicked();

	void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};

UCLASS()
class DIALOGUEMANAGER_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetViewData(const UDialogueOptionViewData* ViewData);

protected:
	void NativePreConstruct() override;
	void NativeConstruct() override;

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void OnContinueButtonPressed();

	UFUNCTION()
	void OnInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);

	UFUNCTION()
	void OnDialogueStateChanged(EDialogueState NewState);

public:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* ModeSwitcher;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;

	UPROPERTY(meta = (BindWidget))
	UListView* DialogueOptions;

private:
	bool bCachedShowMouseCursor;
	int32 NumResponses = 0;
};
