#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FCharacter.generated.h"


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

	UFUNCTION(NetMulticast, Reliable)
		void SetPlayerMesh_Multicast(USkeletalMesh* _PlayerMesh);

	UFUNCTION(Client, Reliable)
		void SetPlayerMesh_Client(USkeletalMesh* _PlayerMesh);

	class UCharacterComponent* GetCharacterComponent() { return CharacterComponent; }

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual	void OnRep_Mesh();

	virtual void OnRep_PlayerState() override;

	void EquipWeapon();

public:	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> Rifle;

	UPROPERTY(EditAnywhere)
		TObjectPtr<AWeapon> Weapon;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UCharacterComponent> CharacterComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		TObjectPtr<class AFPlayerState> FPlayerState;


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
