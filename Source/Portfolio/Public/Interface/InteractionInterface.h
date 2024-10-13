// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UENUM()
enum class EInteractableType : uint8
{
	Active UMETA(DisplayName = "Active"),
	Hold UMETA(DisplayName = "Hold") ,
	Trade UMETA(DisplayName = "Trade"),
	Toggle UMETA(DisplayName = "Toggle"),
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :
		InteractableType(EInteractableType::Active),
		Name(FText::GetEmpty()),
		InteractionDuration(0.0f) {
	};

	UPROPERTY(EditAnywhere)
		EInteractableType InteractableType;

	UPROPERTY(EditAnywhere)
		FText Name;

	UPROPERTY(VisibleAnywhere)
		float InteractionDuration;
};

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class PORTFOLIO_API IInteractionInterface
{
	GENERATED_BODY()

public:
	virtual void BeginFocus();
	virtual void EndFoucs();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(class AFCharacter* FCharacter);
public:
	FInteractableData InteractableData;
};
