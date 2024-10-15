#pragma once

#include "CoreMinimal.h"
#include "DialogueAssetFactory.generated.h"

UCLASS()
class UDialogueAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UDialogueAssetFactory(const FObjectInitializer& InObjectInitializer);

	UObject* FactoryCreateNew(UClass* inUclass, UObject* inParent, FName inName, EObjectFlags inFlags, UObject* inContext, FFeedbackContext* inWarn) override;
	bool CanCreateNew() const override;
};
