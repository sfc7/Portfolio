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

	ZombieRemaning = 0;
}

void AMainGameMode::BeginPlay()
{
	FGameState = GetGameState<AFGameState>();

	CountZombieRemaning();
	GetWorld()->GetTimerManager().SetTimer(ZombieSpawnHandle, this, &ThisClass::SpawnZombie, 2.0f, true);
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

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
		
	UE_LOG(LogTemp, Log, TEXT("count : %d"), ZombieSpawnPointArray.Num());
}

void AMainGameMode::SpawnZombie()
{
	if (ZombieRemaning > 0) {
		int RandomIndex = FMath::RandRange(0, ZombieSpawnPointArray.Num() - 1);

		if (AZombieSpawnPoint* SpawnPoint = ZombieSpawnPointArray[RandomIndex]) {
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();

			if (AZombieCharacter* Zombie = GetWorld()->SpawnActor<AZombieCharacter>(ZombieCharacterClass, Loc, Rot)) {
				ZombieRemaning--;
			}
		}
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(ZombieSpawnHandle);
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

void AMainGameMode::CountZombieRemaning()
{
	if (FGameState) {
		uint8 RoundNumber = FGameState->GetRoundNumber();
		ZombieRemaning = 6;
	}
}
