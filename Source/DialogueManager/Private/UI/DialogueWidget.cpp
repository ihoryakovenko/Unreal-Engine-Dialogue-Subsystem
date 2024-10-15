#include "UI/DialogueWidget.h"

#include "GameFramework/InputDeviceSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "UI/DialogueWidgetViewData.h"
#include "DialogueSubsystem.h"

DEFINE_LOG_CATEGORY(DialogueWidget);

void UDialogueWidget::NativePreConstruct()
{
	DialogueOptions->SetSelectionMode(ESelectionMode::Single);
}

void UDialogueWidget::NativeConstruct()
{
	// TODO: Unbind when hidden?
	UDialogueSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDialogueSubsystem>(GetOwningLocalPlayer());
	// TODO: Move bindings to DialogueWidget holder?
	Subsystem->DialogueOptionSelected.AddUObject(this, &UDialogueWidget::SetViewData);
	Subsystem->DialogueStateChanged.AddUObject(this, &UDialogueWidget::OnDialogueStateChanged);

	// TODO: All device changes should be controlled by some external system!!!
	UInputDeviceSubsystem::Get()->OnInputHardwareDeviceChanged.AddDynamic(this, &UDialogueWidget::OnInputDeviceChanged);
}

void UDialogueWidget::SetViewData(const UDialogueOptionViewData* ViewData)
{
	DialogueOptions->ClearListItems();

	DialogueText->SetText(ViewData->MainText);
	ModeSwitcher->SetActiveWidgetIndex(0);

	for (UDialogueResponseViewData* ResponseData : ViewData->Responses)
	{
		DialogueOptions->AddItem(ResponseData);
	}

	NumResponses = ViewData->Responses.Num();

	SetFocus();
}

void UDialogueWidget::OnContinueButtonPressed()
{
	// TODO: Fix on subsystem side
	if (NumResponses == 0)
	{
		OnDialogueStateChanged(EDialogueState::Finished);
		return;
	}

	ModeSwitcher->SetActiveWidgetIndex(1);

	DialogueOptions->SetFocus();
	DialogueOptions->SetKeyboardFocus();
	
	const FHardwareDeviceIdentifier& DeviceIdentifier = UInputDeviceSubsystem::Get()->
		GetMostRecentlyUsedHardwareDevice(GetOwningPlayer()->GetPlatformUserId());

	if (DeviceIdentifier.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
	{
		DialogueOptions->SetSelectedIndex(0);
	}
}

void UDialogueWidget::OnInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	const FHardwareDeviceIdentifier& DeviceIdentifier = UInputDeviceSubsystem::Get()->
		GetMostRecentlyUsedHardwareDevice(UserId);

	APlayerController* PlayerController = GetOwningPlayer();

	if (DeviceIdentifier.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
	{
		// TODO: Should be last selected index
		DialogueOptions->SetSelectedIndex(0);

		if (PlayerController)
		{
			PlayerController->SetShowMouseCursor(false);
		}
	}
	else
	{
		if (PlayerController)
		{
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

FReply UDialogueWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnContinueButtonPressed();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDialogueWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::Virtual_Accept)
	{
		OnContinueButtonPressed();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDialogueWidget::OnDialogueStateChanged(EDialogueState NewState)
{
	switch (NewState)
	{
		case EDialogueState::Started:
		{
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (APlayerController* PlayerController = GetOwningPlayer())
			{
				bCachedShowMouseCursor = PlayerController->bShowMouseCursor;
				PlayerController->SetShowMouseCursor(true);
				UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, nullptr,
					EMouseLockMode::DoNotLock, true);
			}

			break;
		}
		case EDialogueState::Ongoing:
		{
			break;
		}
		case EDialogueState::Finished:
		{
			SetVisibility(ESlateVisibility::Collapsed);

			if (APlayerController* PlayerController = GetOwningPlayer())
			{
				PlayerController->SetShowMouseCursor(bCachedShowMouseCursor);
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController);
			}

			break;
		}
	}
}

void UDialogueOptionEntryWidget::Init(int Index, const FText& Text)
{
	// TODO: support string table
	const FText Format = FText::Format(NSLOCTEXT("Namespace", "Key", "{0}."), FText::AsNumber(Index + 1));

	OptionIndex = Index;
	ResponseText->SetText(Text);
	IndexText->SetText(Format); // Support key inputs
}

void UDialogueOptionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	DialogueButton->OnClicked.AddDynamic(this, &UDialogueOptionEntryWidget::OnDialogueButtonClicked);
}

void UDialogueOptionEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (bIsSelected)
	{
		DialogueButton->SetFocus();
		DialogueButton->SetButtonHovered(true);
	}
	else
	{
		DialogueButton->SetButtonHovered(false);
	}
}

void UDialogueOptionEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Cast<UListView>(GetOwningListView())->SetSelectedIndex(OptionIndex);
}

void UDialogueOptionEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Cast<UListView>(GetOwningListView())->ClearSelection();
}

void UDialogueOptionEntryWidget::OnDialogueButtonClicked()
{
	UDialogueSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UDialogueSubsystem>(GetOwningLocalPlayer());
	Subsystem->SelectDialogueOption(OptionIndex);
}

void UDialogueOptionEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (UDialogueResponseViewData* ViewData = Cast<UDialogueResponseViewData>(ListItemObject))
	{
		Init(ViewData->Index, ViewData->Text);
	}
}
