// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MonsterCharacter.h"
#include "ZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieCharacter : public AMonsterCharacter
{
	GENERATED_BODY()
public:
	AZombieCharacter();
protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
private:
	UFUNCTION()
		void MeshAssetLoad();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UMonsterComponent> MonsterComponent;
private:
	FSoftObjectPath	CurrentZombieCharacterMeshPath = FSoftObjectPath();

	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;
};
