// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	AActor* GetOwningActor() const { return OwningActor;  }

	void SetOwningActor(AActor* _OwningActor) { OwningActor = _OwningActor; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<AActor> OwningActor;
};
