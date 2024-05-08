// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:

	class UPlayerHUD* GetHUDWidget() const { return HUDWidget; }

	void ToggleMenu();

	UFUNCTION(Server, Reliable)
		void SpawnPlayerMove_Server();

	void PlayerBeginPlaySetMesh(USkeletalMesh* _PlayerMesh);

	UFUNCTION()
		virtual void BindPlayerState();

	void LevelTransition(const FString& _LevelPath);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnRep_PlayerState() override;

	virtual void BindFirstWeaponAmmo();

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		FText UserNotificationText;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> CrosshairUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UPlayerHUD> HUDWidgetClass;

	UPROPERTY()
		TObjectPtr<class UPlayerHUD> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> MenuUIClass;

	UPROPERTY()
		TObjectPtr<class UUserWidget> MenuUI;

	bool FlagMenu = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> UserNotificationTextUIClass;






};
