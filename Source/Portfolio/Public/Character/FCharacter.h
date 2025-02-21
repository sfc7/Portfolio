#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/DataStruct.h"
#include "FCharacter.generated.h"

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_USTRUCT_BODY()
		FWeaponSlot() {}

	UPROPERTY()
		TObjectPtr<class AWeapon> FirstPrimaryWeapon;

	UPROPERTY()
		TObjectPtr<class AWeapon> SecondPrimaryWeapon;

	UPROPERTY()
		uint8 Granade;
};

UCLASS()
class PORTFOLIO_API AFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetSkeletalMeshInPlayerStateSave(USkeletalMesh* _PlayerMesh);

	UFUNCTION(Server, Reliable)
		void SetPlayerMesh_Server(USkeletalMesh* _PlayerMesh);

	class UCharacterComponent* GetCharacterComponent() { return CharacterComponent; }

	TObjectPtr<class AFPlayerState> GetFPlayerState() const { return FPlayerState.Get(); }

	void EquipWeapon();

	void WeaponAttachToSocket(AWeapon* _Weapon);

	UFUNCTION(Server, Reliable)
		void WeaponAttachToSocket_Server();

	void FirstEquipWeapon();

	UFUNCTION(Server, Reliable)
		void EquipWeapon_Server();

	UFUNCTION(Server, Reliable)
		void FirstEquipWeapon_Server();

	FWeaponSlot GetWeaponSlot() { return WeaponSlot; }

	UFUNCTION()
		void LoadPlayerStateSave();

	void WeaponHudBind();

	class UCameraComponent* GetCameraCommpoent() { return CameraComponent; }


protected:
	virtual void BeginPlay() override;
		
	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION()
	virtual	void OnRep_Mesh();

	virtual void OnRep_PlayerState() override;

	UFUNCTION()
		void OnRep_Weapon();



public:	

	UPROPERTY(ReplicatedUsing = OnRep_Weapon, EditAnywhere)
		TObjectPtr<class AWeapon> CurrentWeapon;

protected:
	bool bFirstPlayerStateLoad;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UCharacterComponent> CharacterComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TObjectPtr<class AFPlayerState> FPlayerState;
 
	UPROPERTY(Replicated, meta = (AllowPrivateAccess= true))
		FWeaponSlot WeaponSlot;
	

	// Move
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float ForwardInputValue;

	float PreviousForwardInputValue = 0.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float RightInputValue;

	float PreviousRightInputValue = 0.f;
	//

	//Aim
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float AimPitch = 0.f;

	float PreviousAimPitch = 0.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float AimYaw = 0.f;

	float PreviousAimYaw = 0.f;
	//

	UPROPERTY(ReplicatedUsing = OnRep_Mesh)
		USkeletalMesh* ReplicateMesh;



};
