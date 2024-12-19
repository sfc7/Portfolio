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

	UFUNCTION()
		virtual void BindPlayerState(AFPlayerState* _PlayerState);

	UFUNCTION(Client, Reliable)
		void EndMap_Client();

	void WeaponBuyShow(struct FWeaponData* PurchasableWeaponData);

	void WeaponBuyHide();

	void OnHeadShotUI();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnRep_PlayerState() override;


		
public:
	UPROPERTY(Replicated, BlueprintReadWrite)
		FText UserNotificationText;

	UPROPERTY(Replicated, BlueprintReadWrite)
		FText WaveText;

	UPROPERTY()
		TObjectPtr<class UPlayerHUD> HUDWidget;

	UPROPERTY()
		TObjectPtr<class UUserWidget> MenuUI;

	UPROPERTY()
		TObjectPtr<class UUserWidget> LoadingScreen;

	UPROPERTY()
		TObjectPtr<class UPlayerWeaponBuy> WeaponBuyWidget;

	UPROPERTY(Replicated)
		TObjectPtr<class UUserWidget> UserNotificationTextUI;

	UPROPERTY(Replicated)
		TObjectPtr<class UUserWidget> WaveTextUI;

	UPROPERTY(Replicated)
		TObjectPtr<class UUserWidget> HeadShotUI;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UPlayerHUD> HUDWidgetClass;

	bool FlagMenu = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> MenuUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UUserWidget> LoadingScreenClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UPlayerWeaponBuy> WeaponBuyWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> UserNotificationTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> WaveTextUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> HeadShotUIClass;

	FTimerHandle HeadShotHanldle;

};
