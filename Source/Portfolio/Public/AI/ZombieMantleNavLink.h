// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "ZombieMantleNavLink.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieMantleNavLink : public ANavLinkProxy
{
	GENERATED_BODY()
public:
	AZombieMantleNavLink();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		bool bIsOccupied;
};
