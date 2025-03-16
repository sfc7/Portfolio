#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineMeshComponent.h"
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
#include "Components/SplineComponent.h"
#include "UI/PlayerHUD.h"
#include "WorldStatic/Weapon/Grenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SpotLightComponent.h"
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

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
	SplinePath->SetupAttachment(GetMesh());

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLightComponent->SetupAttachment(GetCapsuleComponent());

	DefaultCameraZOffset = SpringArmComponent->SocketOffset.Z;
	CrouchedCameraZOffset = DefaultCameraZOffset - 80.0f; 
}
	
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
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
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled()) {
		ThrowGrenade_Tick();
	}

	AMainGameMode* gamemode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
	if (gamemode) {
		FString LevelStateString;
	}

	if (CurrentFOV != TargetFOV) {
        CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, 8.f);
        CameraComponent->SetFieldOfView(CurrentFOV);
    }

	SpringArmComponent->SocketOffset.Z = FMath::FInterpTo(SpringArmComponent->SocketOffset.Z, TargetCameraZOffset, DeltaTime, 8.f);

	if (IsValid(GetController())) {
		FRotator ControlRotation = GetController()->GetControlRotation();
		AimPitch = ControlRotation.Pitch;
		AimYaw = ControlRotation.Yaw;
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
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MenuAction, ETriggerEvent::Started, this, &ThisClass::OnMenu);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ReloadAction, ETriggerEvent::Started, this, &ThisClass::Reload);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->E_InteractionAction, ETriggerEvent::Started, this, &ThisClass::WeaponBuyInteract);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Number1_InteractionAction, ETriggerEvent::Started, this, &ThisClass::Swap_1);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->Number2_InteractionAction, ETriggerEvent::Started, this, &ThisClass::Swap_2);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->GrenadeAction, ETriggerEvent::Started, this, &ThisClass::ThrowGrenade);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->GrenadeAction, ETriggerEvent::Triggered, this, &ThisClass::ThrowGrenadeDistance);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->GrenadeAction, ETriggerEvent::Completed, this, &ThisClass::ThrowGrenadeEnd);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->FlashLightAction, ETriggerEvent::Started, this, &ThisClass::ToggleFlashLight);

	}
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetCharacterComponent()->GetIsDead())
	{
		return 0.f;
	}

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	FString CharacterName = this->GetName();

	if (HasAuthority()) {
		if (CharacterName == TEXT("BP_Character_C_0")) {
			GetCharacterComponent()->SetCurrentHp(GetCharacterComponent()->GetCurrentHp() - ActualDamage);
		}
	}	

	if (GetCharacterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
		GetCharacterComponent()->SetIsDead(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);


		APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(GetController());
		if (PlayerCharacterController) {
			PlayerCharacterController->Destroy();
		}

		if (IsValid(CurrentWeapon)) {
			CurrentWeapon->Destroy();
		}

		AMainGameMode* MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
		if (IsValid(MainGameMode) && MainGameMode->AlivePlayerCharacterControllers.Find(PlayerCharacterController) != INDEX_NONE) {
			MainGameMode->AlivePlayerCharacterControllers.Remove(PlayerCharacterController);
		}
	}
	else {
		PlayHitMontage_Server();
	}

	return ActualDamage;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ForwardInputValue);
	DOREPLIFETIME(ThisClass, RightInputValue);
	DOREPLIFETIME(ThisClass, AimPitch);
	DOREPLIFETIME(ThisClass, AimYaw); 
	DOREPLIFETIME(ThisClass, bThrowMontageEndFlag); 
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
		AnimInstance->SwapWeaponMontage.AddUObject(this, &ThisClass::SwapWeapon);
		AnimInstance->ThrowGrenadeMontageEnd.AddUObject(this, &ThisClass::ThrowGrenadeAnimationEnd);
	}
}

void APlayerCharacter::Destroyed()
{
	Super::Destroyed();
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
			TargetCameraZOffset = CrouchedCameraZOffset;
		}
		else {
			GetCharacterComponent()->CurrentState = ECurrentState::Stand;
			TargetCameraZOffset = DefaultCameraZOffset;
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

	if (IsLocallyControlled()) {
		UseAmmo();
	}

	if (CurrentWeapon->GetWeaponData().WeaponType == EWeaponType::SG) {
		for (int i = 0; i < 6; i++) {
			FVector CameraStartLocation = CameraComponent->GetComponentLocation();
			MaxSprayAngle = 5.0f;
			FRotator SprayRotation = GetSprayAngle(CameraComponent->GetForwardVector(), FMath::DegreesToRadians(MaxSprayAngle));
			FVector SprayDirection = SprayRotation.Vector();
			FVector CameraEndLocation = CameraStartLocation + (SprayDirection * GunRange);

			FireBullet_LineTrace(CameraStartLocation, CameraEndLocation);
		}
	}
	else {
		FVector CameraStartLocation = CameraComponent->GetComponentLocation();
		MaxSprayAngle = 1.0f;
		FRotator SprayRotation = GetSprayAngle(CameraComponent->GetForwardVector(), FMath::DegreesToRadians(MaxSprayAngle));
		FVector SprayDirection = SprayRotation.Vector();
		FVector CameraEndLocation = CameraStartLocation + (SprayDirection * GunRange);

		FireBullet_LineTrace(CameraStartLocation, CameraEndLocation);
	}
	

	if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
	{ 
		PlayerController->ClientStartCameraShake(FireShake);
	}

	if (CurrentWeapon) {
		CurrentWeapon->SpawnMuzzleFlash_Server();
	}
}

void APlayerCharacter::FireBullet_LineTrace(FVector _CameraStartLocation, FVector _CameraEndLocation)
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(CurrentWeapon);
	QueryParams.AddIgnoredComponent((const UPrimitiveComponent*)CameraComponent);
	QueryParams.bTraceComplex = true;

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, _CameraStartLocation, _CameraEndLocation, ECC_GameTraceChannel3, QueryParams);

	if (bIsHit) {
		/*DrawDebugLine(GetWorld(), _CameraStartLocation, HitResult.Location, FColor::Blue, true, 0.1f, 0U, 0.5f);*/	

		FVector ShotDirection = -(CameraComponent->GetComponentRotation().Vector());
		UParticleSystem* ImpactEffect = CurrentWeapon->GetWeaponData().ImpactEffect;
		SpawnImpactEffect_Server(ImpactEffect, HitResult.Location, ShotDirection.Rotation());

		AZombieCharacter* HitZombie = Cast<AZombieCharacter>(HitResult.GetActor());

		if (IsValid(HitZombie)) {
			HitZombie->ZombieHitted(this, HitResult);

			FString HitActorClass = HitResult.GetActor()->GetClass()->GetSuperClass()->GetName();
			if (HitActorClass == FString(TEXT("ZombieCharacter"))) {
				SpawnBloodEffect_Server(HitResult.Location, ShotDirection.Rotation());
			}

			FString BoneNameString = HitResult.BoneName.ToString();
			FDamageEvent DamageEvent;

			/*DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255, 0, 0, 255), true, 20.f, 0U, 5.f);*/

			if (BoneNameString.Equals(FString(TEXT("head")), ESearchCase::IgnoreCase)) {
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
				ApplyDamageAndDrawLine_Server(HitZombie, HitResult, CurrentWeapon->GetWeaponData().Damage, DamageEvent, GetController(), this);
			}
		}
	}
	else {
		FDamageEvent DamageEvent;
		//DrawDebugLine(GetWorld(), _CameraStartLocation, _CameraEndLocation, FColor::Cyan, false, 0.1f, 0U, 0.5f);
	}
}

FRotator APlayerCharacter::GetSprayAngle(FVector _MuzzleDirection, float _MaxAngle)
{
	FVector RandomDirection = FMath::VRandCone(_MuzzleDirection, _MaxAngle);

	return FRotationMatrix::MakeFromX(RandomDirection).Rotator();
}

void APlayerCharacter::UseAmmo_Server_Implementation()
{
	CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetCurrentAmmo() - 1, CurrentWeapon->GetTotalAmmo());
}

void APlayerCharacter::Reload()
{
	if (CurrentWeapon->GetWeaponData().TotalAmmo > 0 && CurrentWeapon->GetWeaponData().ReloadMaxAmmo != CurrentWeapon->GetWeaponData().CurrentAmmo) {
		if(GetCharacterComponent()->CurrentState == ECurrentState::Stand || GetCharacterComponent()->CurrentState == ECurrentState::Stand)
		ReloadAnimationPlay();
		PlayReloadMontage_Server();
	}
	else {
		return;
	}
}

void APlayerCharacter::ReloadAmmo()
{
	int32 AmmoDif = CurrentWeapon->GetReloadMaxAmmo() - CurrentWeapon->GetCurrentAmmo();
	int32 CurrentAmmo;
	int32 TotalAmmo;

	if (CurrentWeapon->GetTotalAmmo() < AmmoDif) {
		CurrentAmmo = AmmoDif;
		TotalAmmo = 0;
	}
	else {
		CurrentAmmo = CurrentWeapon->GetReloadMaxAmmo();
		TotalAmmo = CurrentWeapon->GetTotalAmmo() - AmmoDif;
	}

	CurrentWeapon->SetCurrentAndTotalAmmo(CurrentAmmo, TotalAmmo);

	return;
}

void APlayerCharacter::UpdateDestroyedActor()
{
	
	UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		FGameInstance->WeaponSlotData.FirstPrimaryWeaponData = WeaponSlot.FirstPrimaryWeapon->GetWeaponData();
		FGameInstance->WeaponSlotData.SecondPrimaryWeaponData = WeaponSlot.SecondPrimaryWeapon->GetWeaponData();

		FGameInstance->CurrentLevel = GetCharacterComponent()->GetCurrentLevel();
		FGameInstance->CurrentEXP = GetCharacterComponent()->GetCurrentEXP();
		FGameInstance->PlayerMoney = GetCharacterComponent()->GetMoney();
	}
}

void APlayerCharacter::Swap_1()
{
	if (CurrentWeapon == WeaponSlot.SecondPrimaryWeapon) {
		if (WeaponSlot.FirstPrimaryWeapon->GetWeaponData().Mesh != nullptr) {
			if (GetCharacterComponent()->CurrentState == ECurrentState::Stand || GetCharacterComponent()->CurrentState == ECurrentState::Crouch) {
				WeaponSwapAnimationPlay();
				PlayWeaponSwapMontage_Server();
			}
		}
	} 
}

void APlayerCharacter::Swap_2()
{
	if (CurrentWeapon == WeaponSlot.FirstPrimaryWeapon) {
		if (WeaponSlot.SecondPrimaryWeapon->GetWeaponData().Mesh != nullptr) {
			if (GetCharacterComponent()->CurrentState == ECurrentState::Stand || GetCharacterComponent()->CurrentState == ECurrentState::Crouch) {
				WeaponSwapAnimationPlay();
				PlayWeaponSwapMontage_Server();
			}
		}
	}
}

void APlayerCharacter::SwapWeapon()
{
	if (IsLocallyControlled()) {
		bIsBurstTrigger = false;
		if (HasAuthority()) {
			SetSwapWeaponActive();
		}
		else {
			SetSwapWeaponActive();
			SetSwapWeaponActive_Server();
		}
	}
}

void APlayerCharacter::SetSwapWeaponActive()
{
	if (CurrentWeapon == WeaponSlot.SecondPrimaryWeapon) {
		if (WeaponSlot.FirstPrimaryWeapon->GetWeaponData().Mesh != nullptr) {
			WeaponSlot.SecondPrimaryWeapon->SetWeaponState(EWeaponState::UnEquipped);
			WeaponSlot.FirstPrimaryWeapon->SetWeaponState(EWeaponState::Equipped);
			CurrentWeapon = WeaponSlot.FirstPrimaryWeapon;
		}
	}
	else if (CurrentWeapon == WeaponSlot.FirstPrimaryWeapon) {
		if (WeaponSlot.SecondPrimaryWeapon->GetWeaponData().Mesh != nullptr) {
			WeaponSlot.FirstPrimaryWeapon->SetWeaponState(EWeaponState::UnEquipped);
			WeaponSlot.SecondPrimaryWeapon->SetWeaponState(EWeaponState::Equipped);
			CurrentWeapon = WeaponSlot.SecondPrimaryWeapon;
		}
	}

	if (IsLocallyControlled()) {
		CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetTotalAmmo());
	}
}

void APlayerCharacter::SetSwapWeaponActive_Server_Implementation()
{
	SetSwapWeaponActive();
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

void APlayerCharacter::HitAnimationPlay()
{
	if (!IsValid(AnimInstance)) {
		return;
	}

	AnimInstance->PlayHitAnimMontage();
}

void APlayerCharacter::OnCurrentLevelChanged(int32 NewCurrentLevel)
{
	/*ParticleSystemComponent->Activate(true);*/
}

void APlayerCharacter::ToggleBurstTrigger(const FInputActionValue& InValue)
{
	if (CurrentWeapon->GetWeaponData().WeaponType == EWeaponType::AR || CurrentWeapon->GetWeaponData().WeaponType == EWeaponType::SMG) {
		bIsBurstTrigger = !bIsBurstTrigger;
		TimeBetWeenFire = 60.f / (float)CurrentWeapon->GetWeaponData().RPM;
	}
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
	if (CurrentWeapon->GetCurrentAmmo() <= 0) {
		ReverseRecoil();
		GetWorldTimerManager().ClearTimer(BetweenShotsTimer);
		return;
	}

	FireAnimationPlay();
	PlayAttackMontage_Server();

	if (CurrentWeapon) {
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
	return (GetCharacterComponent() && GetCharacterComponent()->GetIsDead());
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

void APlayerCharacter::UseAmmo()
{
	CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetCurrentAmmo() - 1, CurrentWeapon->GetTotalAmmo());
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

	AnimInstance->PlayChangeWeaponAnimMontage();
}

void APlayerCharacter::WeaponSwapAnimationPlay()
{
	if (!IsValid(AnimInstance)) {
		return;
	}

	AnimInstance->PlaySwapWeaponAmimMontage();
}

void APlayerCharacter::ThrowGrenadeAnimationPlay()
{
	if (!IsValid(AnimInstance)) {
		return;
	}

	if (IsValid(GetCharacterComponent())) {
		GetCharacterComponent()->CurrentState = ECurrentState::Grenade;
	}

	bThrowMontageEndFlag = false;
	AnimInstance->PlayThrowGrenadeWeaponAmimMontage();
}

void APlayerCharacter::ThrowGrenade_Tick()
{
	if (GetCharacterComponent()->CurrentState == ECurrentState::Grenade) {
		FHitResult HitResult;
		TArray<FVector> PathResults;
		FVector LastTraceDestinationResult;
		TArray<TEnumAsByte<EObjectTypeQuery>> PredictObjectType;
		PredictObjectType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		TArray<AActor*> IgnoreActors;

		FVector ForwardVector = GetActorForwardVector();
		ForwardVector *= 100.f;
		ForwardVector += FVector(0.0f, 0.0f, 50.0f);

		FVector SocketForwardVector = GetActorForwardVector();
		LaunchVecter = (SocketForwardVector + SocketForwardVector * Grenade_Distance) * 500.f;

		bool bHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(
			this,
			OUT HitResult,
			OUT PathResults,
			OUT LastTraceDestinationResult,
			GetActorLocation() + ForwardVector,
			LaunchVecter,
			true,
			10.0f,
			PredictObjectType,
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			1.0f,
			10.0f,
			10.0f,
			0.0f
		);

		if (SplinePath)
		{
			if (SplinePath)
			{
				SplinePath->ClearSplinePoints();

				for (USplineMeshComponent* MeshComp : SplineMeshComponents)
				{
					MeshComp->DestroyComponent();
				}
				SplineMeshComponents.Empty();
			}

			for (int32 i = 0; i < PathResults.Num(); i++)
			{
				SplinePath->AddSplinePointAtIndex(PathResults[i], i, ESplineCoordinateSpace::World, true);
			}

			SplinePath->UpdateSpline();

			for (int32 i = 0; i < SplinePath->GetNumberOfSplinePoints() - 1; i++) // SplinePoints - 1·Î ¼öÁ¤
			{
				USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);

				if (SplineMeshComponent) {

					SplineMeshComponent->SetMobility(EComponentMobility::Movable);
					SplineMeshComponent->AttachToComponent(SplinePath, FAttachmentTransformRules::KeepRelativeTransform);

					FVector StartPos = SplinePath->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
					FVector EndPos = SplinePath->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
					FVector StartTangent = SplinePath->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
					FVector EndTangent = SplinePath->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
					SplineMeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
					SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

					if (SplineStaticMaterial) {
						SplineMeshComponent->SetStaticMesh(SplineStaticMesh);
					}

					SplineMeshComponent->RegisterComponent();

					SplineMeshComponents.Add(SplineMeshComponent);
				}
			}
		}
	}
	else {
		if (SplinePath)
		{
			SplinePath->ClearSplinePoints();

			for (USplineMeshComponent* MeshComp : SplineMeshComponents)
			{
				MeshComp->DestroyComponent();
			}
			SplineMeshComponents.Empty();
		}
	}
}

void APlayerCharacter::ThrowGrenade(const FInputActionValue& InValue)
{
	if (WeaponSlot.Granade >= 0) {
		Grenade_Distance = 0;
		
		ThrowGrenadeAnimationPlay();
		PlayThrowGrenadeMontage_Server();
	}
}

void APlayerCharacter::ThrowGrenadeDistance(const FInputActionValue& InValue)
{
	float Value = InValue.Get<float>();
	Grenade_Distance = FMath::FInterpTo(Grenade_Distance, Grenade_Distance + (Value / 10.f), GetWorld()->GetDeltaSeconds(), 5.0f);
}

void APlayerCharacter::ThrowGrenadeEnd()
{
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	AnimInstance->Montage_Resume(CurrentMontage);
	bThrowMontageEndFlag = true;
	GetCharacterComponent()->SetCurrenState(ECurrentState::Stand);

	if (IsLocallyControlled()) {
		ThrowGrenadeEnd_Server();
	}
}

void APlayerCharacter::ThrowGrenadeSpawn_Server_Implementation(FVector _LaunchVecter)
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	SpawnParam.Instigator = this;

	AGrenade* SpawnGrenade = GetWorld()->SpawnActor<AGrenade>(
		CharacterComponent->GetGrenadeType(),
		GetMesh()->GetSocketLocation("Grenade_Socket"),
		GetMesh()->GetSocketRotation("Grenade_Socket"),
		SpawnParam
	);

	if (SpawnGrenade)
	{
		SpawnGrenade->SetVelocity(_LaunchVecter);
	}
}

void APlayerCharacter::ThrowGrenadeEnd_Server_Implementation()
{
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	AnimInstance->Montage_Resume(CurrentMontage);
	bThrowMontageEndFlag = true;
	ThrowGrenadeEnd_NetMulticast();
}

void APlayerCharacter::ThrowGrenadeEnd_NetMulticast_Implementation()
{
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	AnimInstance->Montage_Resume(CurrentMontage);
	bThrowMontageEndFlag = true;
}

void APlayerCharacter::ThrowGrenadeAnimationEnd()
{
	if (IsLocallyControlled()) {
		ThrowGrenadeSpawn_Server(LaunchVecter);
	}
}

void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector CameraStartLocation = CameraComponent->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * InteractionCheckRange;

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), CameraComponent->GetForwardVector());
	if (LookDirection > 0) {
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult HitResult;
		
		bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraStartLocation, CameraEndLocation, ECC_GameTraceChannel6, QueryParams);
		if (IsHit) {
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()) && HitResult.GetActor()->GetOwner() == nullptr) {
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
			FWeaponData* ItemData = PurchasableItemData->FindRow<FWeaponData>(ItemName, ItemName.ToString());

			if (ItemData) {
				FindTargetWeaponData = *ItemData;
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

void APlayerCharacter::ToggleFlashLight_Server_Implementation()
{
	bool bIsCurrentlyHidden = CurrentWeapon->MuzzleSpotLightComponent->IsVisible();
	CurrentWeapon->MuzzleSpotLightComponent->SetVisibility(!bIsCurrentlyHidden);
}

void APlayerCharacter::ToggleFlashLight()
{
	if (HasAuthority())
	{
		bool bIsCurrentlyHidden = CurrentWeapon->MuzzleSpotLightComponent->IsVisible();
		CurrentWeapon->MuzzleSpotLightComponent->SetVisibility(!bIsCurrentlyHidden);
	}
	else
	{
		ToggleFlashLight_Server();
	}
}

void APlayerCharacter::ChangeWeapon()
{
	if (IsLocallyControlled()) {
		bIsBurstTrigger = false;
		if (CurrentWeapon == WeaponSlot.FirstPrimaryWeapon) {
			if (WeaponSlot.SecondPrimaryWeapon->GetWeaponData().Mesh == nullptr) {
				ChangeWeaponActive();
				ChangeWeaponActive_Server();
			}
		}

		CurrentWeapon->SetWeaponData(FindTargetWeaponData);
		CurrentWeapon->SetWeaponMesh(FindTargetWeaponData.Mesh);
		CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetTotalAmmo());
	}

	//if (IsLocallyControlled() && GetCharacterComponent()) {
	//	GetCharacterComponent()->SetCurrentAndTotalAmmo(FindTargetWeaponData.CurrentAmmo, FindTargetWeaponData.TotalAmmo);
	//	GetCharacterComponent()->SetReloadMaxAmmo(FindTargetWeaponData.ReloadMaxAmmo);
	//	CurrentWeapon->SetWeaponMesh_Server(FindTargetWeaponData.Mesh);
	//}
}

void APlayerCharacter::ChangeWeaponActive()
{
	CurrentWeapon->SetWeaponState(EWeaponState::UnEquipped);
	CurrentWeapon = WeaponSlot.SecondPrimaryWeapon;
	CurrentWeapon->SetWeaponState(EWeaponState::Equipped);
}

void APlayerCharacter::ChangeWeaponActive_Server_Implementation()
{
	ChangeWeaponActive();
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
	FireAnimationPlay();
}

void APlayerCharacter::PlayWeaponChangeMontage_Server_Implementation()
{
	WeaponChangeAnimationPlay();
	PlayWeaponChangeMontage_NetMulticast();
}

void APlayerCharacter::PlayWeaponChangeMontage_NetMulticast_Implementation()
{
	WeaponChangeAnimationPlay();
	
}

void APlayerCharacter::PlayHitMontage_Server_Implementation()
{
	HitAnimationPlay();
	PlayHitMontage_NetMulticast();
}

void APlayerCharacter::PlayHitMontage_NetMulticast_Implementation()
{
	HitAnimationPlay();
}

void APlayerCharacter::PlayWeaponSwapMontage_Server_Implementation()
{
	WeaponSwapAnimationPlay();
	PlayWeaponSwapMontage_NetMulticast();
}

void APlayerCharacter::PlayWeaponSwapMontage_NetMulticast_Implementation()
{
	if (!HasAuthority() && GetOwner() != UGameplayStatics::GetPlayerController(this, 0)) {
		WeaponSwapAnimationPlay();
	}
}

void APlayerCharacter::PlayThrowGrenadeMontage_Server_Implementation()
{
	ThrowGrenadeAnimationPlay();
	PlayThrowGrenadeMontage_NetMulticast();

}

void APlayerCharacter::PlayThrowGrenadeMontage_NetMulticast_Implementation()
{
	ThrowGrenadeAnimationPlay();
}