#include "UI/DialogueButton.h"

#include "Components/ButtonSlot.h"

void SDialogueButton::SetButtonHovered(bool IsHover)
{
	if (IsHover)
	{
		SetHover(true);
	}
	else
	{
		SetHover(false);
		TAttribute<bool> InHovered;
		SetHover(InHovered);
	}
}

void UDialogueButton::SetButtonHovered(bool IsHovered)
{
	static_cast<SDialogueButton*>(MyButton.Get())->SetButtonHovered(IsHovered);
}

TSharedRef<SWidget> UDialogueButton::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyButton = SNew(SDialogueButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
		if (GetChildrenCount() > 0)
		{
			Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
		}

	return MyButton.ToSharedRef();
}
