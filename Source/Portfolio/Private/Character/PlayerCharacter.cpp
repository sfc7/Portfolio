#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/CharacterComponent.h"
#include "GameFramework/PlayerState.h"
#include "Controller/PlayerCharacterController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/InputConfig.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Controller/PlayerCharacterController.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "WorldStatic/LandMine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPlayerState.h"
#include "Game/MainGameMode.h"
#include "Portfolio/Portfolio.h"
#include "Game/PlayerStateSave.h"
#include "WorldStatic/Weapon/Weapon.h"
#include "Animation/PlayerAnimInstance.h"
#include "Game/FGameInstance.h"
#include "Character/ZombieCharacter.h"
#include "UI/PlayerHUD.h"
#include "Data/DataStruct.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetRelativeLocation(FVector(150.f, 50.f, 70.f));

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	TimeBetWeenFire = 60.f / FirePerMinute;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//if (IsValid(FPlayerState)) {
	//	if (!FPlayerState->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged)) {
	//		FPlayerState->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
	//	}
	//}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	if (IsValid(PlayerController)) {
		EnableInput(PlayerController);
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		Subsystem->ClearAllMappings();
		if (IsValid(Subsystem)) {
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}

	FOnTimelineFloat RecoilCurve_X;
	FOnTimelineFloat RecoilCurve_Y;

	RecoilCurve_X.BindUFunction(this, FName("StartHorizontalRecoil"));
	RecoilCurve_Y.BindUFunction(this, FName("StartVerticalRecoil"));

	RecoilTimeline.AddInterpFloat(HorizontalCurve, RecoilCurve_X);
	RecoilTimeline.AddInterpFloat(VerticalCurve, RecoilCurve_Y);

}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	/*if (IsValid(FPlayerState)) {
		if (!FPlayerState->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged)) {
			FPlayerState->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
		}
	}*/
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 35.f);
	CameraComponent->SetFieldOfView(CurrentFOV);
	if (IsValid(GetController())) {
		FRotator ControlRotation = GetController()->GetControlRotation();
		AimPitch = ControlRotation.Pitch;
		AimYaw = ControlRotation.Yaw;
	}

	if (bIsNowRagdollBlending) {
		CurrentRagDollBlendWeight = FMath::FInterpTo(CurrentRagDollBlendWeight, TargetRagDollBlendWeight, DeltaTime, 10.f);

		FName PivotBoneName = FName(TEXT("spine_01"));
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagDollBlendWeight);

		if (CurrentRagDollBlendWeight - TargetRagDollBlendWeight < KINDA_SMALL_NUMBER) {
			GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
			bIsNowRagdollBlending = false;
		}

		if (IsValid(GetCharacterComponent()) && GetCharacterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
			GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("root")), 1.f);
			bIsNowRagdollBlending = false;
		}
	}

	if (IsValid(GetController())) {
		PreviousAimPitch = AimPitch;
		PreviousAimYaw = AimYaw;

		FRotator ControlRotation = GetController()->GetControlRotation();
		AimPitch = ControlRotation.Pitch;
		AimYaw = ControlRotation.Yaw;
		if (PreviousAimPitch != AimPitch || PreviousAimYaw != AimYaw)
		{
			if (!HasAuthority()) {
				UpdateAimValue_Server(AimPitch, AimYaw);
			}
		}
	}

	if (PreviousForwardInputValue != ForwardInputValue || PreviousRightInputValue != RightInputValue) {
		if (!HasAuthority()) {
			UpdateInputValue_Server(ForwardInputValue, RightInputValue);
		}
	}

	if (IsLocallyControlled()) {
		if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency) {
			PerformInteractionCheck();
		}
	}

	if (RecoilTimeline.IsPlaying()) {

		RecoilTimeline.TickTimeline(DeltaTime);
	}

	if (RecoilTimeline.IsReversing()) {

		if (FMath::Abs(YawInput) > 0 || FMath::Abs(PitchInput) > 0) { 
			RecoilTimeline.Stop();
			return;
		}

		RecoilTimeline.TickTimeline(DeltaTime);

		FRotator NewRotation = UKismetMathLibrary::RInterpTo(GetControlRotation(), StartRotation, DeltaTime, 4.5f);


		Controller->ClientSetRotation(NewRotation);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent)) {
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->CrouchAction, ETriggerEvent::Started, this, &ThisClass::Crouch);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::Attack);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Started, this, &ThisClass::StartAiming);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Completed, this, &ThisClass::EndAiming);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->BurstTriggerAction, ETriggerEvent::Started, this, &ThisClass::ToggleBurstTrigger);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::AttackOnBurstTrigger);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::StopFire);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LandMineAction, ETriggerEvent::Started, this, &ThisClass::SpawnLandMine);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MenuAction, ETriggerEvent::Started, this, &ThisClass::OnMenu);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ReloadAction, ETriggerEvent::Started, this, &ThisClass::Reload);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->E_InteractionAction, ETriggerEvent::Started, this, &ThisClass::WeaponBuyInteract);
	}
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	GetCharacterComponent()->SetCurrentHp(GetCharacterComponent()->GetCurrentHp() - ActualDamage);

	if (!IsValid(GetCharacterComponent())) {
		return ActualDamage;
	}

	if (IsDead()) {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	if (GetCharacterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
		GetMesh()->SetSimulatePhysics(true);
	}
	else {
		FName PivotBoneName = FName(TEXT("spine_01"));
		GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
		HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
		GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.f, false);
	}

	PlayRagdoll_NetMulticast();

	return ActualDamage;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ForwardInputValue);
	DOREPLIFETIME(ThisClass, RightInputValue);
	DOREPLIFETIME(ThisClass, AimPitch);
	DOREPLIFETIME(ThisClass, AimYaw);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetCharacterComponent()) {
		GetCharacterComponent()->PlayerCharacter = this;
	}

	AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance)) {
		AnimInstance->FireMontage.AddUObject(this, &ThisClass::FireBullet);
		AnimInstance->ReloadMontageEnd.AddUObject(this, &ThisClass::ReloadAmmo);
		AnimInstance->ChangeWeaponMontage.AddUObject(this, &ThisClass::ChangeWeapon);
	}
}

void APlayerCharacter::Destroyed()
{
	Super::Destroyed();

	UpdateDestroyedActor();
}

void APlayerCharacter::UpdateInputValue_Server_Implementation(const float& _ForwardInputValue, const float& _RightInputValue)
{
	ForwardInputValue = _ForwardInputValue;
	RightInputValue = _RightInputValue;
}

void APlayerCharacter::UpdateAimValue_Server_Implementation(const float& _AimPitch, const float& _AimYaw)
{
	AimPitch = _AimPitch;
	AimYaw = _AimYaw;
}

void APlayerCharacter::UpdateAimValue_NetMulticast_Implementation(const float& _AimPitch, const float& _AimYaw)
{
	AimPitch = _AimPitch;
	AimYaw = _AimYaw;
}
	
void APlayerCharacter::Move(const FInputActionValue& InValue)
{
	FVector2D MovementVector = InValue.Get<FVector2D>();
	ForwardInputValue = MovementVector.X;
	RightInputValue = MovementVector.Y;

	const FRotator CurrentControlRotation = GetController()->GetControlRotation();
	const FRotator CurrentControlRotationYaw(0.f, CurrentControlRotation.Yaw, 0.f);

	FVector ForwardDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, ForwardInputValue);
	AddMovementInput(RightDirection, RightInputValue);
}

void APlayerCharacter::Look(const FInputActionValue& InValue)
{
	FVector2D LookVector = InValue.Get<FVector2D>();

	PitchInput = LookVector.Y;
	YawInput = LookVector.X;

	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void APlayerCharacter::Attack(const FInputActionValue& InValue)
{
	if (!IsValid(GetPlayerState())) {
		return;
	}
	if (!bIsBurstTrigger) {
		Fire();
	}
}

void APlayerCharacter::Crouch(const FInputActionValue& InValue)
{
	if (GetCharacterComponent()) {
		if (GetCharacterComponent()->CurrentState == ECurrentState::Stand) {
			GetCharacterComponent()->CurrentState = ECurrentState::Crouch;
		}
		else {
			GetCharacterComponent()->CurrentState = ECurrentState::Stand;
		}
	}
}

void APlayerCharacter::StartAiming(const FInputActionValue& InValue)
{
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1080.f, 0.f);
	TargetFOV = 45.f;

	if (GetCharacterComponent()) {
		GetCharacterComponent()->SetAiming(true);
	}
}

void APlayerCharacter::EndAiming(const FInputActionValue& InValue)
{
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	TargetFOV = 70.f;

	if (GetCharacterComponent()) {
		GetCharacterComponent()->SetAiming(false);
	}
}

void APlayerCharacter::FireBullet()
{
	if (GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	UseAmmo_Server();

	FHitResult HitResult;

	FVector CameraStartLocation = CameraComponent->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * GunRange;

	CameraEndLocation.Y += CameraEndLocation.Y * FMath::RandRange(-RandomSpreadValue, RandomSpreadValue);
	CameraEndLocation.Z += CameraEndLocation.Z * FMath::RandRange(-RandomSpreadValue, RandomSpreadValue);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Weapon);
	QueryParams.AddIgnoredComponent((const UPrimitiveComponent*)CameraComponent);
	QueryParams.bTraceComplex = true;	

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraStartLocation, CameraEndLocation, ECC_GameTraceChannel3, QueryParams);

	if (bIsHit) {
		DrawDebugLine(GetWorld(), CameraStartLocation, HitResult.Location, FColor::Red, true, 0.1f, 0U, 0.5f);	

		AZombieCharacter* HitZombie = Cast<AZombieCharacter>(HitResult.GetActor());
		
		if (IsValid(HitZombie)) {
			HitZombie->ZombieHitted(this, HitResult);

			FVector ShotDirection = -(CameraComponent->GetComponentRotation().Vector());
			UParticleSystem* ImpactEffect = GetCharacterComponent()->EquippedWeapon->ImpactEffect;
			SpawnImpactEffect_Server(ImpactEffect, HitResult.Location, ShotDirection.Rotation());

			FString HitActorClass = HitResult.GetActor()->GetClass()->GetSuperClass()->GetName();
			if (HitActorClass == FString(TEXT("ZombieCharacter"))) {
				SpawnBloodEffect_Server(HitResult.Location, ShotDirection.Rotation());
			}

			FString BoneNameString = HitResult.BoneName.ToString();		
			FDamageEvent DamageEvent;

			DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255, 0, 0, 255), true, 20.f, 0U, 5.f);

			if (BoneNameString.Equals(FString(TEXT("head")), ESearchCase::IgnoreCase)){
				ApplyDamageAndDrawLine_Server(HitZombie, HitResult, 100.f, DamageEvent, GetController(), this);
				if (GetCharacterComponent()) {
					GetCharacterComponent()->SetCrossHairColor_Red();
					
					APlayerCharacterController* PlayerCharacterController = GetController<APlayerCharacterController>();
					if (IsValid(PlayerCharacterController)) {
						PlayerCharacterController->OnHeadShotUI();
					}
				}
			}
			else {
				ApplyDamageAndDrawLine_Server(HitZombie, HitResult, 10.f, DamageEvent, GetController(), this);
			}
		}
	}
	else {
		FDamageEvent DamageEvent;
		DrawDebugLine(GetWorld(), CameraStartLocation, CameraEndLocation, FColor::Cyan, false, 0.1f, 0U, 0.5f);
	}

	if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
	{ 
		PlayerController->ClientStartCameraShake(FireShake);
	}

	if (GetCharacterComponent()) {
		GetCharacterComponent()->EquippedWeapon->SpawnMuzzleFlash_Server();
	}
}

void APlayerCharacter::UseAmmo_Server_Implementation()
{
	GetCharacterComponent()->SetCurrentAndTotalAmmo(GetCharacterComponent()->GetCurrentAmmo() - 1, GetCharacterComponent()->GetTotalAmmo());
}

void APlayerCharacter::Reload()
{
	if (GetCharacterComponent()->GetTotalAmmo() > 0 && GetCharacterComponent()->GetReloadMaxAmmo() != GetCharacterComponent()->GetCurrentAmmo()) {
		ReloadAnimationPlay();
		PlayReloadMontage_Server();
	}
	else {
		return;
	}
}

void APlayerCharacter::ReloadAmmo()
{
	int32 AmmoDif = GetCharacterComponent()->GetReloadMaxAmmo() - GetCharacterComponent()->GetCurrentAmmo();
	int32 CurrentAmmo;
	int32 TotalAmmo;

	if (GetCharacterComponent()->GetTotalAmmo() < AmmoDif) {
		CurrentAmmo = AmmoDif;
		TotalAmmo = 0;
	}
	else {
		CurrentAmmo = GetCharacterComponent()->GetReloadMaxAmmo();
		TotalAmmo = GetCharacterComponent()->GetTotalAmmo() - AmmoDif;
	}

	GetCharacterComponent()->SetCurrentAndTotalAmmo(CurrentAmmo, TotalAmmo);

	return;
}

void APlayerCharacter::UpdateDestroyedActor()
{
	UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		FGameInstance->TotalAmmo = GetCharacterComponent()->GetTotalAmmo();
		FGameInstance->CurrentAmmo = GetCharacterComponent()->GetCurrentAmmo();
		FGameInstance->ReloadMaxAmmo = GetCharacterComponent()->GetReloadMaxAmmo();
		FGameInstance->CurrentWeaponType = GetCharacterComponent()->GetCurrentWeaponType();

		FGameInstance->CurrentLevel = GetCharacterComponent()->GetCurrentLevel();
		FGameInstance->CurrentEXP = GetCharacterComponent()->GetCurrentEXP();
		FGameInstance->PlayerMoney = GetCharacterComponent()->GetMoney();
	}
}

void APlayerCharacter::StartHorizontalRecoil(float Value)
{
	if (RecoilTimeline.IsReversing()) return;
	AddControllerYawInput(Value);
}

void APlayerCharacter::StartVerticalRecoil(float Value)
{
	if (RecoilTimeline.IsReversing()) {
		return;
	}
	AddControllerPitchInput(Value);
}

void APlayerCharacter::StartRecoil()
{
	RecoilTimeline.PlayFromStart();
}

void APlayerCharacter::ReverseRecoil()
{
	RecoilTimeline.ReverseFromEnd();
}

void APlayerCharacter::OnCurrentLevelChanged(int32 NewCurrentLevel)
{
	/*ParticleSystemComponent->Activate(true);*/
}

void APlayerCharacter::ToggleBurstTrigger(const FInputActionValue& InValue)
{
	bIsBurstTrigger = !bIsBurstTrigger;
}

void APlayerCharacter::AttackOnBurstTrigger(const FInputActionValue& InValue)
{
	if (bIsBurstTrigger) {
		StartRotation = GetControlRotation();
		StartRecoil();
		GetWorldTimerManager().SetTimer(BetweenShotsTimer, this, &ThisClass::Fire, TimeBetWeenFire, true);
	}
}

void APlayerCharacter::StopFire(const FInputActionValue& InValue)
{
	GetWorldTimerManager().ClearTimer(BetweenShotsTimer);
	ReverseRecoil();
}

void APlayerCharacter::Fire()
{
	if (!IsValid(GetCharacterComponent()) || GetCharacterComponent()->GetCurrentAmmo() == 0) {
		ReverseRecoil();
		GetWorldTimerManager().ClearTimer(BetweenShotsTimer);
		return;
	}

	FireAnimationPlay();
	PlayAttackMontage_Server();

	if (GetCharacterComponent()->EquippedWeapon) {
		GetCharacterComponent()->CrossHairShootingValue += 0.75f;
	}
}

void APlayerCharacter::OnMenu(const FInputActionValue& InValue)
{
	APlayerCharacterController* PlayerController = GetController<APlayerCharacterController>();
	if (IsValid(PlayerController))
	{
		PlayerController->ToggleMenu();
	}
}

void APlayerCharacter::UpdateDestroyedActor_Client_Implementation()
{
	UpdateDestroyedActor();
}

void APlayerCharacter::SpawnLandMine(const FInputActionValue& InValue)
{
	SpawnLandMine_Server();
}

bool APlayerCharacter::SpawnLandMine_Server_Validate()
{
	return true;
}

void APlayerCharacter::SpawnLandMine_Server_Implementation()
{
	if (IsValid(LandMineClass)) {
		FVector SpawnLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
		ALandMine* LandMine = GetWorld()->SpawnActor<ALandMine>(LandMineClass, SpawnLocation, FRotator::ZeroRotator);
		LandMine->SetOwner(GetController());
	}
}

void APlayerCharacter::OnHittedRagdollRestoreTimerElapsed()
{
	FName PivotBoneName = FName(TEXT("spine_01"));
	TargetRagDollBlendWeight = 0.f;
	CurrentRagDollBlendWeight = 1.f;
	bIsNowRagdollBlending = true;
}

void APlayerCharacter::PlayRagdoll_NetMulticast_Implementation()
{
	if (!IsValid(GetCharacterComponent())) {
		return;
	}

	if (GetCharacterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
		GetMesh()->SetSimulatePhysics(true);
	}
	else {
		FName PivotBoneName = FName(TEXT("spine_01"));
		GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
		TargetRagDollBlendWeight = 1.f;
		HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
		GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.f, false);
	}
}

void APlayerCharacter::ApplyDamageAndDrawLine_Server_Implementation(ACharacter* HitCharacter, const FHitResult& HitResult, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsValid(HitCharacter)) {
		HitCharacter->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}
	
	/*DrawLine_NetMulticast(DrawStart, DrawEnd);*/
}

void APlayerCharacter::DrawLine_NetMulticast_Implementation(const FVector& DrawStart, const FVector& DrawEnd)
{
	DrawDebugLine(GetWorld(), DrawStart, DrawEnd, FColor(255, 255, 255, 64), false, 2.0f, 0U, 0.5f);
}

bool APlayerCharacter::IsAiming()
{
	return (GetCharacterComponent() && GetCharacterComponent()->bIsAiming);
}

bool APlayerCharacter::IsDead()
{
	return (GetCharacterComponent() && GetCharacterComponent()->bIsDead);
}

ECurrentState APlayerCharacter::IsCurrentState()
{
	if (IsValid(GetCharacterComponent())) {
		return(GetCharacterComponent()->CurrentState);
	}
	else {
		return ECurrentState::None;
	}
}

void APlayerCharacter::FireAnimationPlay()
{
	if (!IsValid(AnimInstance)) {
		return;
	}

	if (IsAiming()) {
		AnimInstance->PlayRifleIronSightFireAnimMontage();
	}
	else {
		AnimInstance->PlayRifleFireAnimMontage();
	}
}

void APlayerCharacter::ReloadAnimationPlay()
{
	if (!IsValid(AnimInstance)) {
		return;
	}

	if (IsAiming()) {
		AnimInstance->PlayRifleIronSightReloadAnimMontage();
	}
	else {
		AnimInstance->PlayRifleReloadAnimMontage();
	}
}


void APlayerCharacter::WeaponChangeAnimationPlay()
{	
	if (!IsValid(AnimInstance)) {
		return;
	}

	//if (IsAiming()) {
	//	if (GetCharacterComponent()) {
	//		GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	//		TargetFOV = 70.f;

	//		if (GetCharacterComponent()) {
	//			GetCharacterComponent()->SetAiming(false);
	//		}
	//	}
	//}

	AnimInstance->PlayChangeWeaponAnimMontage();
}

void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector CameraStartLocation = CameraComponent->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * InteractionCheckRange;

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), CameraComponent->GetForwardVector());
	if (LookDirection > 0) {
		/*DrawDebugLine(GetWorld(), CameraStartLocation, CameraEndLocation, FColor::Blue, false, 1.0f, 0, 2.0f);*/

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult HitResult;
		
		bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraStartLocation, CameraEndLocation, ECC_GameTraceChannel6, QueryParams);
		if (IsHit) {
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass())) {
				if (HitResult.GetActor() != InteractionData.CurrentInteractable) {
					FoundInteractable(HitResult.GetActor());
					return;
				}

				if (HitResult.GetActor() == InteractionData.CurrentInteractable) {
					return;
				}
			}
		}
	}

	NoInteractableFound();
}

void APlayerCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting()) {
		EndInteract();
	}

	if (InteractionData.CurrentInteractable) {
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFoucs();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;
	FindTargetInteractableInfo();

	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("TargetInteractable: %s"), *TargetInteractable->InteractableData.Name.ToString()));
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("FindTargetWeaponData: %s"), *FindTargetWeaponData.Name.ToString()));
	PlayerController->WeaponBuyShow(&FindTargetWeaponData);


	TargetInteractable->BeginFocus();
}

void APlayerCharacter::NoInteractableFound()
{
	if (IsInteracting()) {
		GetWorldTimerManager().ClearTimer(InteractionTimerHandle);
	}

	if (InteractionData.CurrentInteractable) {
		if (IsValid(TargetInteractable.GetObject())) {
			TargetInteractable->EndFoucs();
		}
	}

	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	PlayerController->WeaponBuyHide();

	InteractionData.CurrentInteractable = nullptr;
	TargetInteractable = nullptr;
}


//아무것도 변하지 않았는지 확인하기위한 안전장치, 동일한 것을 보고 있다는 것을 확인, 미사용중
void APlayerCharacter::BeginInteract()
{
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable) {
		if (IsValid(TargetInteractable.GetObject())) {
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f)) {
				Interact();
			}
			else {
				GetWorldTimerManager().SetTimer(InteractionTimerHandle, this, &APlayerCharacter::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
			}
		}
	}
}

void APlayerCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(InteractionTimerHandle);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	} 
}

//

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(InteractionTimerHandle);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void APlayerCharacter::FindTargetInteractableInfo()
{	
	if (IsValid(TargetInteractable.GetObject()))
	{
		switch (TargetInteractable->InteractableData.InteractableType)
		{
		case EInteractableType::Active:
			break;
		case EInteractableType::Toggle:
			break;
		case EInteractableType::Trade:
		{
			FName ItemName = FName(*TargetInteractable->InteractableData.Name.ToString());
			
			FPurchasableWeaponData* ItemData = PurchasableItemData->FindRow<FPurchasableWeaponData>(ItemName, ItemName.ToString());

			if (ItemData) {
				FindTargetWeaponData.Name = ItemData->Name;
				FindTargetWeaponData.CurrentAmmo = ItemData->CurrentAmmo;
				FindTargetWeaponData.TotalAmmo = ItemData->TotalAmmo;
				FindTargetWeaponData.ReloadMaxAmmo = ItemData->ReloadMaxAmmo;
				FindTargetWeaponData.Price = ItemData->Price;
			}
		break;
		}	
		default:
			break;
		}
	}
}

void APlayerCharacter::WeaponBuyInteract()
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		if (GetCharacterComponent()->GetMoney() > FindTargetWeaponData.Price) {
			interactableWeapon = Cast<AWeapon>(InteractionData.CurrentInteractable);
			int32 CalCulatedMoney = GetCharacterComponent()->GetMoney() - FindTargetWeaponData.Price;
			GetCharacterComponent()->SetMoney(CalCulatedMoney);

			PlayWeaponChangeMontage_Server();
		 }

		TargetInteractable->Interact(this);
		// or Interact();
	}
}

void APlayerCharacter::ChangeWeapon()
{
	if (IsLocallyControlled() && GetCharacterComponent()) {
		GetCharacterComponent()->EquipWeapon(interactableWeapon);
		GetCharacterComponent()->SetCurrentAndTotalAmmo(FindTargetWeaponData.CurrentAmmo, FindTargetWeaponData.TotalAmmo);
		GetCharacterComponent()->SetReloadMaxAmmo(FindTargetWeaponData.ReloadMaxAmmo);
	}
}

void APlayerCharacter::SpawnBloodEffect_Server_Implementation(FVector _Location, FRotator _Rotation)
{
	if (BloodEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEffect, _Location, _Rotation);
	}
	SpawnBloodEffect_NetMulticast(_Location, _Rotation);
}

void APlayerCharacter::SpawnBloodEffect_NetMulticast_Implementation(FVector _Location, FRotator _Rotation)
{
	if (BloodEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEffect, _Location, _Rotation);
	}
}

void APlayerCharacter::SpawnImpactEffect_Server_Implementation(UParticleSystem* _ImpactEffect, FVector _Location, FRotator _Rotation)
{
	if (GetCharacterComponent()) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _ImpactEffect, _Location, _Rotation);
	}
	SpawnImpactEffect_NetMulticast(_ImpactEffect, _Location, _Rotation);
}

void APlayerCharacter::SpawnImpactEffect_NetMulticast_Implementation(UParticleSystem* _ImpactEffect, FVector _Location, FRotator _Rotation)
{
	if (GetCharacterComponent()) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _ImpactEffect, _Location, _Rotation);
	}
}

void APlayerCharacter::PlayReloadMontage_Server_Implementation()
{
	ReloadAnimationPlay();
	PlayReloadMontage_NetMulticast();
}

void APlayerCharacter::PlayReloadMontage_NetMulticast_Implementation()
{
	if (!HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) {
		ReloadAnimationPlay();
	}
}

void APlayerCharacter::PlayAttackMontage_Server_Implementation()
{
	FireAnimationPlay();
	PlayAttackMontage_NetMulticast();
}

void APlayerCharacter::PlayAttackMontage_NetMulticast_Implementation()
{
	if (!HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) {
		FireAnimationPlay();
	}
}

void APlayerCharacter::PlayWeaponChangeMontage_Server_Implementation()
{
	WeaponChangeAnimationPlay();
	PlayWeaponChangeMontage_NetMulticast();
}

void APlayerCharacter::PlayWeaponChangeMontage_NetMulticast_Implementation()
{
	if (!HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) {
		WeaponChangeAnimationPlay();
	}
}