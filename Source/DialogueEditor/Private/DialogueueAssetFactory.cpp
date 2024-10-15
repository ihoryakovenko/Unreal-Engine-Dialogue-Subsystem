#include "DialogueAssetFactory.h"

#include "DialogueAsset.h"

UDialogueAssetFactory::UDialogueAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UDialogueAsset::StaticClass();
}

UObject* UDialogueAssetFactory::FactoryCreateNew(UClass* inUclass, UObject* inParent, FName inName, EObjectFlags inFlags, UObject* inContext, FFeedbackContext* inWarn)
{
	return NewObject<UDialogueAsset>(inParent, inName, inFlags);
}

bool UDialogueAssetFactory::CanCreateNew() const
{
	return true;
}
