#include "Game/MainGameMode.h"
#include "Game/FPlayerState.h"
#include "Portfolio/Portfolio.h"
#include "Character/PlayerCharacter.h"
#include "Controller/PlayerCharacterController.h"
#include "WorldStatic/ZombieSpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ZombieCharacter.h"
#include "Game/FGameInstance.h"
#include "Character/PlayerCharacter.h"
#include "Game/FGameState.h"

AMainGameMode::AMainGameMode()
{
	if (IsValid(ZombieRoundTable) && ZombieRoundTable->GetRowMap().Num() > 0) {
		for (int32 i = 1; i <= ZombieRoundTable->GetRowMap().Num(); i++) {
			check(nullptr != GetZombieRoundTableRowFromRound(i));
		}
	}

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


FZombieRoundTable* AMainGameMode::GetZombieRoundTableRowFromRound(int _Round)
{
		
	if (IsValid(ZombieRoundTable)) {
		return ZombieRoundTable->FindRow<FZombieRoundTable>(*FString::FromInt(_Round), TEXT(""));
	}

	return nullptr;
}

void AMainGameMode::BeginPlay()
{
	FGameState = GetGameState<AFGameState>();

	SetLevelStateFromLevelName();

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 1.0f, true);
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(NewPlayer);
	if (IsValid(PlayerCharacterController)) {
		PlayerCharacterControllers.Add(PlayerCharacterController);
	}

	TArray<AActor* > TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieSpawnPoint::StaticClass(), TempArray);
	if (TempArray.Num() > 0) {
		for (AActor* TempActor : TempArray) {
			if (AZombieSpawnPoint* SpawnPoint = Cast<AZombieSpawnPoint>(TempActor)) {
				ZombieSpawnPointArray.Add(SpawnPoint);
			}
		}
	}
}

void AMainGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
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

				if (IsValid(FGameState)) {
					FGameState->SpawnTotalZombiesInRound();
					UE_LOG(LogTemp, Log, TEXT("GetTotalZombiesInRound %d"), FGameState->GetTotalZombiesInRound());
				}
			}
		}
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(ZombieSpawnHandle);
		SetLevelStateFromString("WaitingStage");
	}
}

void AMainGameMode::ZombieDie()
{
	if (IsValid(FGameState)) {
		FGameState->ZombieDie();
	}
}

void AMainGameMode::EndMap()
{
	for (APlayerCharacterController* PlayerCharacterController : PlayerCharacterControllers) {
		if (IsValid(PlayerCharacterController)) {
			PlayerCharacterController->EndMap_Client();
		}
	}
}

void AMainGameMode::InRoom()
{
	if (RemaningWaitTime <= 0) {
		RemaningWaitTime = RoomTime;

		EndMap();

		GetWorld()->ServerTravel("/Game/Level/Stage?listen");
	}
	else {
		NotificationString = FString::Printf(TEXT("%d sec Remaning..."), RemaningWaitTime);
		RemaningWaitTime--;
	}
	OnNotificationText(NotificationString);
}

void AMainGameMode::OnMainTimerElapsed()
{
	switch (LevelState) {
	case ELevelState::None:
		break;
	case ELevelState::Room:
		InRoom();
		break;
	case ELevelState::WaitingStage:
		WaitStage();
		break;
	case ELevelState::Stage:
		InStage();
		break;
	case ELevelState::End:
		break;
	default:
		break;
	}
}

void AMainGameMode::OnNotificationText(const FString& _NotificationString)
{
	for (APlayerCharacterController* AlivePlayerCharacterController : PlayerCharacterControllers) {
		AlivePlayerCharacterController->UserNotificationText = FText::FromString(_NotificationString);
	}
}

void AMainGameMode::SetLevelStateFromLevelName()
{
	FString LevelName = GetWorld()->GetMapName();
	LevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (LevelName == "Stage") {
		LevelState = ELevelState::WaitingStage;
	}
	else if (LevelName == "Room") {
		LevelState = ELevelState::Room;
	}
}

void AMainGameMode::SetLevelStateFromString(const FString& _LevelState)
{
	if (_LevelState == "Stage") {
		LevelState = ELevelState::Stage;
	}
	else if (_LevelState == "Room") {
		LevelState = ELevelState::Room;
	}
	else if (_LevelState == "WaitingStage") {
		LevelState = ELevelState::WaitingStage;
	}
	else if (_LevelState == "End") {
		LevelState = ELevelState::End;
	}
	else {
		LevelState = ELevelState::None;
	}
}

void AMainGameMode::SetZombieRemaning()
{
	if (IsValid(FGameState)) {
		CurrentRound = FGameState->GetRoundNumber();
		ZombieSpawnRemaning = GetZombieRoundTableRowFromRound(CurrentRound++)->Number;
		UE_LOG(LogTemp, Log, TEXT("SetZombieRemaning ZombieSpawnRemaning %d"), ZombieSpawnRemaning);
		FGameState->IncrementRound();
	}
}


void AMainGameMode::WaitStage()
{
	if (RemaningWaitTime <= 0) {
		RemaningWaitTime = RoomTime;
		SetZombieRemaning();
		LevelState = ELevelState::Stage;
		GetWorld()->GetTimerManager().SetTimer(ZombieSpawnHandle, this, &ThisClass::SpawnZombie, 2.0f, true);
	}
	else if (CurrentRound > 3) {
		EndMap();

		GetWorld()->ServerTravel("/Game/Level/Room?listen");
	}
	else {
		NotificationString = FString::Printf(TEXT("%d sec Remaning..."), RemaningWaitTime);
		RemaningWaitTime--;

	}
}

void AMainGameMode::InStage()
{
}


