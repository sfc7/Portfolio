// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UENUM()
enum class EInteractableType : uint8
{
	PickUp UMETA(DisplayName = "PickUp"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter") ,
	Device UMETA(DisplayName = "Device"),
	Toggle UMETA(DisplayName = "Toggle"),
	Container UMETA(DisplayName = "Container")
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :
		InteractableType(EInteractableType::PickUp),
		Name(FText::GetEmpty()),
		Price(0.0f),
		InteractionDuration(0.0f) {
	};

	UPROPERTY(VisibleAnywhere)
	EInteractableType InteractableType;

	UPROPERTY(VisibleAnywhere)
	FText Name;

	UPROPERTY(VisibleAnywhere)
	float Price;

	UPROPERTY(VisibleAnywhere)
	float InteractionDuration;
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PORTFOLIO_API IInteractionInterface
{
	GENERATED_BODY()

public:
	virtual void BeginFocus();
	virtual void EndFoucs();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(class AFCharacter* FCharacter);
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	FInteractableData InteractableData;
};
