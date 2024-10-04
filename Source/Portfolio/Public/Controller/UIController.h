// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIController.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AUIController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	void JoinServer(const FString& IPAddress);

	UFUNCTION(Server, Reliable)
		void JoinOpen_Server();

	UFUNCTION(Server, Reliable)
		void SpawnPlayerMove_Server();

	UFUNCTION(BlueprintCallable)
	void EndMap();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UUserWidget> WBP_UIClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TObjectPtr<class UUserWidget> WBP_UI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UUserWidget> LoadingScreenClass;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TObjectPtr<class UUserWidget> LoadingScreen;
};
