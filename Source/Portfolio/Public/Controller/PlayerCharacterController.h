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
		virtual void BindPlayerState(AFPlayerState* _PlayerState);

	void EndMap();

	UFUNCTION(Client, Reliable)
		void EndMap_Client();

	void WeaponBuyShow(bool ShowFlag, struct FInteractableData* InteractableData);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnRep_PlayerState() override;


		
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		FText UserNotificationText;



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> CrosshairUIClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UPlayerHUD> HUDWidgetClass;

	UPROPERTY()
		TObjectPtr<class UPlayerHUD> HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> MenuUIClass;

	UPROPERTY()
		TObjectPtr<class UUserWidget> MenuUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UUserWidget> LoadingScreenClass;

	UPROPERTY()
		TObjectPtr<class UUserWidget> LoadingScreen;

	UPROPERTY()
		TObjectPtr<class UPlayerWeaponBuy> WeaponBuyWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UPlayerWeaponBuy> WeaponBuyWidgetClass;

	bool FlagMenu = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UUserWidget> UserNotificationTextUIClass;
	 //TObjectPtr //
	






};
