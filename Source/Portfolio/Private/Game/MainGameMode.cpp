// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainGameMode.h"
#include "Game/FPlayerState.h"
#include "Portfolio/Portfolio.h"
#include "Character/PlayerCharacter.h"
#include "Controller/PlayerCharacterController.h"
#include "WorldStatic/ZombieSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ZombieCharacter.h"
#include "Game/FGameState.h"

AMainGameMode::AMainGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerCharacterController> PlayerControllerBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Controller/BP_PlayerCharacterController.BP_PlayerCharacterController_C'"));
	if (PlayerControllerBP.Succeeded())
	{
		PlayerControllerClass = PlayerControllerBP.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerCharacterBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Character/BP_Character.BP_Character_C'"));
	if (PlayerCharacterBP.Succeeded()) {
		PlayerCharacterClass = PlayerCharacterBP.Class;
	}	

	PlayerStateClass = AFPlayerState::StaticClass();
}

void AMainGameMode::BeginPlay()
{
	FGameState = GetGameState<AFGameState>();

	SetLevelStateFromLevelName();
	SetZombieRemaning();

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);
	GetWorld()->GetTimerManager().SetTimer(ZombieSpawnHandle, this, &ThisClass::SpawnZombie, 2.0f, true);
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(NewPlayer);
	if (IsValid(PlayerCharacterController)) {
		AlivePlayerCharacterControllers.Add(PlayerCharacterController);
	}

	AFPlayerState* PlayerState = NewPlayer->GetPlayerState<AFPlayerState>();
	if (IsValid(PlayerState)) {
		PlayerState->InitPlayerState();
	}

	TArray<AActor* > TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieSpawnPoint::StaticClass(), TempArray);
	for (AActor* TempActor : TempArray) {
		if (AZombieSpawnPoint* SpawnPoint = Cast<AZombieSpawnPoint>(TempActor)) {
			ZombieSpawnPointArray.Add(SpawnPoint);
		}
	}
}

void AMainGameMode::SpawnZombie()
{
	if (ZombieSpawnRemaning > 0) {
		int RandomIndex = FMath::RandRange(0, ZombieSpawnPointArray.Num() - 1);

		if (AZombieSpawnPoint* SpawnPoint = ZombieSpawnPointArray[RandomIndex]) {
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();

			if (AZombieCharacter* Zombie = GetWorld()->SpawnActor<AZombieCharacter>(ZombieCharacterClass, Loc, Rot)) {
				Zombie->SpawnDefaultController();
				ZombieSpawnRemaning--;
			}
		}
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(ZombieSpawnHandle);
	}
}

void AMainGameMode::RemaningZombieDie()
{
	if (IsValid(FGameState)) {
		FGameState->ZombieDie();
	}
}


//void AMainGameMode::SpawnPlayer(APlayerController* _PlayerController)
//{
//	Super::SpawnPlayer(_PlayerController);
//
//	if (HasAuthority()) {
//		APlayerCharacter* PlayerCharacter = _PlayerController->GetPawn<APlayerCharacter>();
//		if (IsValid(PlayerCharacter)) {
//			GetWorld()->DestroyActor(PlayerCharacter);
//		}
//
//		APlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, GetPlayerStartTransform());
//		_PlayerController->Possess(NewPlayerCharacter);
//	}
//}

void AMainGameMode::SetZombieRemaning()
{
	if (IsValid(FGameState)) {
		uint8 RoundNumber = FGameState->GetRoundNumber();
		ZombieSpawnRemaning = RoundNumber * 3;
	}
}

void AMainGameMode::OnMainTimerElapsed()
{
	switch (LevelState) {
		case ELevelState::None:
			break;
		case ELevelState::WaitingRoom:
			if (RemaningWaitTime <= 0) {
				RemaningWaitTime = WaitingRoomTime;
				LevelState = ELevelState::Stage;
				GetWorld()->ServerTravel("/Game/Safe_House/maps/Safe_House_2?listen?copy");
			}	
			else {
				NotificationString = FString::Printf(TEXT("%d sec Remaning..."), RemaningWaitTime);
				RemaningWaitTime--;
			}

			OnNotificationText(NotificationString);
			break;
		case ELevelState::Stage:
			break;
		case ELevelState::End:
			break;
		default:
			break;
	}
}

void AMainGameMode::OnNotificationText(const FString& _NotificationString)
{
	for (APlayerCharacterController* AlivePlayerCharacterController : AlivePlayerCharacterControllers) {
		AlivePlayerCharacterController->UserNotificationText = FText::FromString(_NotificationString);
	}
}

void AMainGameMode::SetLevelStateFromLevelName()
{
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (LevelName == "Stage") {
		LevelState = ELevelState::Stage;
	}
	else if (LevelName == "WaitingRoom") {
		LevelState = ELevelState::WaitingRoom;
	}


}

