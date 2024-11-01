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
#include "Blueprint/UserWidget.h"

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

	FGameInstance = Cast<UFGameInstance>(GetGameInstance());

	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(NewPlayer);
	if (IsValid(PlayerCharacterController)) {
		AlivePlayerCharacterControllers.Add(PlayerCharacterController);
		PlayerCharacterControllers.Add(PlayerCharacterController);
	}

	AFPlayerState* FPlayerState = Cast<AFPlayerState>(PlayerCharacterController->PlayerState);
	FPlayerState->InitPlayerState();

	TArray<AActor* > TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieSpawnPoint::StaticClass(), TempArray);
	if (TempArray.Num() > 0) {
		for (AActor* TempActor : TempArray) {
			if (AZombieSpawnPoint* SpawnPoint = Cast<AZombieSpawnPoint>(TempActor)) {
				ZombieSpawnPointArray.Add(SpawnPoint);
			}
		}

		ZombieArrayTotalCount = ZombieSpawnPointArray.Num();
	}

}

void AMainGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	//for (APlayerCharacterController* PlayerCharacterController : PlayerCharacterControllers) {
	//	if (IsValid(PlayerCharacterController) && PlayerCharacterControllers.Find(PlayerCharacterController) != INDEX_NONE) {
	//		PlayerCharacterControllers.Remove(PlayerCharacterController);
	//	}
	//}

	//for (APlayerCharacterController* PlayerCharacterController : AlivePlayerCharacterControllers) {
	//	if (IsValid(PlayerCharacterController) && AlivePlayerCharacterControllers.Find(PlayerCharacterController) != INDEX_NONE) {
	//		AlivePlayerCharacterControllers.Remove(PlayerCharacterController);
	//	}
	//}
}

void AMainGameMode::SpawnZombie()
{
	if (HasAuthority()) {
		int ArrayCount = ZombieArrayCurrntCount % ZombieArrayTotalCount;
		ZombieArrayCurrntCount++;

		if (AZombieSpawnPoint* SpawnPoint = ZombieSpawnPointArray[ArrayCount]) {
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();

			if (AZombieCharacter* Zombie = GetWorld()->SpawnActor<AZombieCharacter>(ZombieCharacterClass, Loc, Rot)) {
				Zombie->SpawnDefaultController();
				ZombieSpawnRemaning--;
			}
		}
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

void AMainGameMode::OnMainTimerElapsed()
{
	FGameState = GetGameState<AFGameState>();
	if (!IsValid(FGameState)) {
		return;
	}

	switch (LevelState) {
	case ELevelState::None:
		break;
	case ELevelState::Room:
		InRoom();
		break;
	case ELevelState::WaitingStage:
		InWaitingStage();
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

void AMainGameMode::ChangeNotificationText(const FString& _NotificationString)
{
	for (APlayerCharacterController* AlivePlayerCharacterController : AlivePlayerCharacterControllers) {
		AlivePlayerCharacterController->UserNotificationText = FText::FromString(_NotificationString);
	}
}

void AMainGameMode::ChangeWaveText(const FString& _WaveString)
{
	for (APlayerCharacterController* AlivePlayerCharacterController : AlivePlayerCharacterControllers) {
		AlivePlayerCharacterController->WaveText = FText::FromString(_WaveString);
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
	if (IsValid(FGameInstance)) {
		int32 CurrentRound = FGameInstance->GetTotalRoundNumber();

		ZombieSpawnRemaning = GetZombieRoundTableRowFromRound(CurrentRound)->Number;
		UE_LOG(LogTemp, Log, TEXT("SetZombieRemaning ZombieSpawnRemaning %d"), ZombieSpawnRemaning);
	}
}

void AMainGameMode::InRoom()
{
	if (RemaningRoomTime <= 0) {
		RemaningRoomTime = RoomTime;

		NotificationString = FString::Printf(TEXT(""));

		EndMap();

		GetWorld()->ServerTravel("/Game/Level/Stage?listen");
	}
	else {
		NotificationString = FString::Printf(TEXT("%d sec Remaning..."), RemaningRoomTime);

		RemaningRoomTime--;
	}

	ChangeNotificationText(NotificationString);

}

void AMainGameMode::InWaitingStage()
{
	WaveString = FString::Printf(TEXT("Wave : %d/%d"), FGameInstance->GetTotalRoundNumber(), MaxRoundNumber); // max°ª Ã£±â


	if (FGameInstance->GetTotalRoundNumber() >= MaxRoundNumber) {
		NotificationString = FString::Printf(TEXT("Clear !!"));

		ChangeNotificationText(NotificationString);
	}
	else if (FGameState->GetCurrentRoundNumber() >= 4) {
		ChangeWaveText(WaveString);
		EndMap();

		GetWorld()->ServerTravel("/Game/Level/Room?listen");
	}
	else if (RemaningWaitTime <= 0) {
		RemaningWaitTime = WaitTime;
		NotificationString = FString::Printf(TEXT(""));
		SetZombieRemaning();

		FGameState->SetTotalZombiesInRound(ZombieSpawnRemaning);

		SetLevelStateFromString("Stage");
	}
	else {
		NotificationString = FString::Printf(TEXT("%d sec Remaning..."), RemaningWaitTime);
		RemaningWaitTime--;
	}

	ChangeWaveText(WaveString);
	ChangeNotificationText(NotificationString);
}


void AMainGameMode::InStage()
{
	if (FGameState->GetTotalZombiesInRound() <= 0) {
		SetLevelStateFromString("WaitingStage");

		for (APlayerCharacterController* AlivePlayerCharacterController : AlivePlayerCharacterControllers) {
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(AlivePlayerCharacterController->GetPawn());
			if(IsValid(PlayerCharacter)) {
				PlayerCharacter->GetCharacterComponent()->SetCurrentHp(100.f);
			}
		}

		FGameState->IncrementCurrentRoundNumber();
		FGameInstance->IncrementTotalRoundNumber();
	}
	else if (FGameState->GetTotalZombiesInRound() >= 24) {

	}
	else if (ZombieSpawnRemaning > 0) {
		SpawnZombie();
	}
}


	