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
#include "Portfolio/Portfolio.h"
#include "Game/PlayerStateSave.h"
#include "WorldStatic/Weapon.h"
#include "Animation/PlayerAnimInstance.h"
#include "Character/ZombieCharacter.h"

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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//if (!IsLocallyControlled())
	//{
	//	return;
	//}

	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	if (IsValid(PlayerController)) {
		EnableInput(PlayerController);
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		Subsystem->ClearAllMappings();
		if (IsValid(Subsystem)) {
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}

	FTimerHandle StateTimer;
	GetWorld()->GetTimerManager().SetTimer(StateTimer, FTimerDelegate::CreateLambda([&] {
		AFPlayerState* FPlayerState = GetPlayerState<AFPlayerState>();
		if (IsValid(FPlayerState)) {
		UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));

		//if (IsLocallyControlled()) {
		//	SetPlayerMesh_Server(PlayerStateSave->PlayerMesh);
		//}

		if (!FPlayerState->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged)) {
			FPlayerState->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
		}
	}
	}), 0.2f, false);
	
	if (GetCharacterComponent() && HasAuthority()) {
		GetCharacterComponent()->EquipWeapon(OverlapWeapon);
	}
	//FName WeaponSocketName = FName(TEXT("Weapon_Socket"));
	//if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
	//	WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
	//}	
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
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent)) {
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::Attack);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Started, this, &ThisClass::StartAiming);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Completed, this, &ThisClass::EndAiming);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->BurstTriggerAction, ETriggerEvent::Started, this, &ThisClass::ToggleBurstTrigger);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Started, this, &ThisClass::AttackOnBurstTrigger);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::StopFire);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LandMineAction, ETriggerEvent::Started, this, &ThisClass::SpawnLandMine);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MenuAction, ETriggerEvent::Started, this, &ThisClass::OnMenu);
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
	DOREPLIFETIME_CONDITION(ThisClass, OverlapWeapon, COND_OwnerOnly);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetCharacterComponent()) {
		GetCharacterComponent()->PlayerCharacter = this;
	}

	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance)) {
		AnimInstance->FireMontage.AddUObject(this, &ThisClass::Fire);
	}
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
	//UpdateAimValue_NetMulticast(AimPitch, AimYaw);
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

	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void APlayerCharacter::Attack(const FInputActionValue& InValue)
{
	if (!bIsBurstTrigger) {
		FirePlay();
		PlayAttackMontage_Server();
		GetCharacterComponent()->CurrentState = ECurrentState::Crouch;
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

void APlayerCharacter::Fire()
{
	if (/*HasAuthority() ||*/ GetOwner() != UGameplayStatics::GetPlayerController(this, 0))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	FHitResult HitResult;

	FVector CameraStartLocation = CameraComponent->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * GunRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredComponent((const UPrimitiveComponent*)CameraComponent);
	QueryParams.bTraceComplex = true;	

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraStartLocation, CameraEndLocation, ECC_GameTraceChannel3, QueryParams);

	if (bIsHit) {
		DrawDebugLine(GetWorld(), CameraStartLocation, HitResult.Location, FColor::Red, true, 0.1f, 0U, 0.5f);

		AZombieCharacter* HitZombie = Cast<AZombieCharacter>(HitResult.GetActor());
		
		if (IsValid(HitZombie)) {
			HitZombie->ZombieHitted(this, HitResult);
			FString BoneNameString = HitResult.BoneName.ToString();		
			FDamageEvent DamageEvent;

			UKismetSystemLibrary::PrintString(this, BoneNameString);
			DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255, 0, 0, 255), true, 20.f, 0U, 5.f);

			if (BoneNameString.Equals(FString(TEXT("head")), ESearchCase::IgnoreCase)){
				ApplyDamageAndDrawLine_Server(CameraStartLocation, HitResult.Location, HitZombie, 50.f, DamageEvent, GetController(), this);
			}
			else {
				ApplyDamageAndDrawLine_Server(CameraStartLocation, HitResult.Location, HitZombie, 10.f, DamageEvent, GetController(), this);
			}
		}
	}
	else {
		FDamageEvent DamageEvent;
		DrawDebugLine(GetWorld(), CameraStartLocation, CameraEndLocation, FColor::Cyan, false, 0.1f, 0U, 0.5f);
		ApplyDamageAndDrawLine_Server(CameraStartLocation, CameraEndLocation, nullptr, 100.f, DamageEvent, GetController(), this);
	}

	if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
	{ 
		PlayerController->ClientStartCameraShake(FireShake);
	}
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
		GetWorldTimerManager().SetTimer(BetweenShotsTimer, this, &ThisClass::Fire, TimeBetWeenFire, true);
	}
}

void APlayerCharacter::StopFire(const FInputActionValue& InValue)
{
	GetWorldTimerManager().ClearTimer(BetweenShotsTimer);
}

void APlayerCharacter::OnMenu(const FInputActionValue& InValue)
{
	APlayerCharacterController* PlayerController = GetController<APlayerCharacterController>();
	if (IsValid(PlayerController))
	{
		PlayerController->ToggleMenu();
	}
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

void APlayerCharacter::ApplyDamageAndDrawLine_Server_Implementation(const FVector& DrawStart, const FVector& DrawEnd, ACharacter* HitCharacter, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsValid(HitCharacter)) {
		HitCharacter->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	DrawLine_NetMulticast(DrawStart, DrawEnd);
}

void APlayerCharacter::DrawLine_NetMulticast_Implementation(const FVector& DrawStart, const FVector& DrawEnd)
{
	DrawDebugLine(GetWorld(), DrawStart, DrawEnd, FColor(255, 255, 255, 64), false, 2.0f, 0U, 0.5f);
}

void APlayerCharacter::PlayAttackMontage_Server_Implementation()
{
	FirePlay();
	PlayAttackMontage_NetMulticast();
}

void APlayerCharacter::PlayAttackMontage_NetMulticast_Implementation()
{
	if (!HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) {
		FirePlay();
	}
}

void APlayerCharacter::SetOverlapWeapon(AWeapon* _Weapon)
{
	if (OverlapWeapon) {
		OverlapWeapon->ShowPickUpText(false); // 서버 Owner에게
	}
	OverlapWeapon = _Weapon;
	if (IsLocallyControlled()) {
		if (OverlapWeapon) {
			OverlapWeapon->ShowPickUpText(true); // 서버 Owner에게
		}
	}
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

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon) // LastWeapon은 Replicate하기 전 OverlapWeapon 값
{
	if (OverlapWeapon) {
		OverlapWeapon->ShowPickUpText(true); // 클라이언트 Owner에게
	}
	if (LastWeapon) {
		LastWeapon->ShowPickUpText(false);
	}
}

void APlayerCharacter::FirePlay()
{
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
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
