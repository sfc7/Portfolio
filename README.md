# 시연 영상
https://www.youtube.com/watch?v=R8HM7mRmQdQ
<br/>
# (멀티플레이) 좀비 서바이벌
 
- 사용엔진 Unreal 5.1
  
- 장르 : TPS 좀비 서바이벌

- 개요 : 멀티플레이가 가능한 3인칭 FPS 좀비 서바이벌 게임입니다. <br/>Wave 마다 쏟아져나오는 좀비를 막는 형식으로 구성되어 있습니다.<br/>
  좀비의 부위를 사격해 돈을 벌고 Room으로 돌아가 재정비를 하며 총 9번의 Wave를 막으면 클리어합니다.<br/>

# 기술 설명서
## 구조 다이어그램
![DiaGram](https://github.com/user-attachments/assets/d40dbd5d-74dd-422a-92e4-1cd39287f219)

## 멀티플레이
### 맵이동

~~~cpp
void ALoadingController::BeginPlay()
{
...
	FString NextLevel = UGameplayStatics::ParseOption(GM->OptionsString, FString(TEXT("NextLevel"))) + "?listen";

	UGameplayStatics::OpenLevel(GM, *NextLevel, false);
...
}

void AUIController::JoinServer(const FString& IPAddress)
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString::Printf(TEXT("NextLevel=%s?Saved=false"), *IPAddress));
}
~~~


멀티플레이 환경을 언리얼 PIE모드에서 listen server로 실행했기 때문에 완벽한 seamless travel이 불가능했습니다.<br/> 
그래서 단순한 level 이동의 경우에는 LoadingController에서 목적지를 ParseOption을 받아 Loading 화면을 보여주고 넘어가는 방식으로 하였고 


~~~cpp
GetWorld()->ServerTravel("/Game/Level/Stage?listen");
~~~
모든 클라이언트들이 함께 이동할 경우 서버에서만 호출 가능하고 클라이언트들이 뒤따라오는 ServerTravel을 주로 사용하였습니다.

![image1](https://github.com/user-attachments/assets/d71aa11e-242e-4a6b-a097-7c5c0e2a051c)

서버와 클라이언들이 만나는 방법은 LAN을 이용해 Session으로 만나는 방법을 이용했는데 Create/Find Session은 블루프린트 함수이다보니
C++로 구현할 경우 가져올 변수와 설정할 함수들이 많아서 Widget에서 블루프린트로 구현했습니다. 






## 데이터 관리

![image2](https://github.com/user-attachments/assets/f658e80c-7e54-4658-914a-f793b7af92e1)

총기나 Zombie와 Character의 스탯, 라운드별 좀비의 Spawn 같은 것들은 DataTable을 통하여 관리하였고

![image3](https://github.com/user-attachments/assets/633475df-3239-46bd-b950-bf4012c9a547)

좀비의 Mesh는 PortfolioSettings라는 새로운 모듈을 만들어 Target파일에 ExtraModuleNames.Add("PortfolioSettings"); 해준 뒤
UZombieCharacterSettings에 UCLASS(config = ZombieCharacterMeshPaths) 속성을 주어 메모장으로 값을 불러오게 해보았습니다. <br/>






## AI

~~~
	float ClosestDistance = FLT_MAX;
	AActor* ClosestActor = nullptr;

	TArray<AActor*> PlayerCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), PlayerCharacters);

	for (AActor* PlayerCharacter : PlayerCharacters) {
		float Distance = PlayerCharacter->GetDistanceTo(OwnerComp.GetOwner());
		if (ClosestDistance > Distance) {
			ClosestDistance = Distance;
			ClosestActor = PlayerCharacter;
		}
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	BlackboardComponent->SetValueAsObject(AZombieAIController::TargetActorKey, ClosestActor);
~~~

![image4](https://github.com/user-attachments/assets/d2981f63-7ea3-4d98-ab74-bd4f297e783f)

게임의 속도감이나 진행을 위해 킬링플로어나 월드워Z에서 보던 그것처럼 게임을 시작하자말자 실시간으로 가장 가까운 Character를 탐지하여 다가올 필요가 있었습니다.

그래서 AI Perception 같은 기능으로 탐지하기 보다는 GetAllActorsOfClass를 통해 가장 가까운 거리를 계산하여 탐지하게 하였습니다.

다만 캐릭터가 죽고나서는 지속적으로 자연스러움을 위해 GetNavigationSystem으로 좀비를 Patrol 시켰습니다. <br/>


## 반동


![image5](https://github.com/user-attachments/assets/c45a50e2-b725-4b96-95e8-01cdc21827df)

총기에 반동에 대한 구현은 다음과 같은 주기를 무한 반복하는 Curve의 Vertical과 Horizontal 차트를 만들고 

~~~
void APlayerCharacter::BeginPlay()
{
	FOnTimelineFloat RecoilCurve_X;
	FOnTimelineFloat RecoilCurve_Y;

	RecoilCurve_X.BindUFunction(this, FName("StartHorizontalRecoil"));
	RecoilCurve_Y.BindUFunction(this, FName("StartVerticalRecoil"));

	RecoilTimeline.AddInterpFloat(HorizontalCurve, RecoilCurve_X);
	RecoilTimeline.AddInterpFloat(VerticalCurve, RecoilCurve_Y);

}
~~~

FTimeLine에 Curve 보간값을 묶어준뒤 

~~~
void APlayerCharacter::StartHorizontalRecoil(float Value)
{
	if (RecoilTimeline.IsReversing()) return;
	AddControllerYawInput(Value);
}

void APlayerCharacter::StartVerticalRecoil(float Value)
{
	if (RecoilTimeline.IsReversing()) return;
	AddControllerPitchInput(Value);
}
~~~


연사기능의 시작과 끝에 Timeline을 Start하고 Reverse하게한뒤 Tick에서 TimeLine의 흐름 따라 Contrller에 Pitfch,Yaw 값을 넣어주어 구현하였고

~~~
	FVector CameraStartLocation = CameraComponent->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + CameraComponent->GetForwardVector() * GunRange;

	CameraEndLocation.Y += CameraEndLocation.Y * FMath::RandRange(-RandomSpreadValue, RandomSpreadValue);
	CameraEndLocation.Z += CameraEndLocation.Z * FMath::RandRange(-RandomSpreadValue, RandomSpreadValue);
~~~

랜덤스프레드에 경우 단순히 총알이 닿는 곳의 끝값에 RandRange값을 주어 튀는 방식으로 구현하였습니다.<br/>

# 상호작용
![image7](https://github.com/user-attachments/assets/7659ae77-b889-4d3b-bd07-235f39cc3a65)
![image6](https://github.com/user-attachments/assets/6fd40524-e70e-4782-9a95-29e8e3e0a5b7)

총기의 구매와 같은 상호작용은 linetrace를 사용하여 관측하는 방식으로 구현하였습니다.<br/>
필요한 데이터를 앞서 데이터 관리에서 설명했던 DataTable에서 가져온 뒤 UI로 나타나게 하였습니다.





![Image8](https://github.com/user-attachments/assets/d71f31ca-9e59-4037-ae7e-2f4cce529ed0)

이러한 상호작용들은 상호작용 타입을 구분하고 획일화된 포맷을 위해 위와 같은 Interace를 사용하였습니다.<br/>

# 좀비 피격 구현
![Untitled-ezgif com-video-to-gif-converter (3)](https://github.com/user-attachments/assets/6ee5e69b-d3f2-4388-870e-b0de6542f3cc)

일반적인 피격에 대해서는 특별한 애니메이션이나 물리적인 요소는 취하지 않았고 단순히 출혈과 같은 피격효과만 나타나는 방식으로 표현했습니다.

다만 좀비가 죽었을 경우에만 시물레이션을 활성화 시킨 뒤 Hips 밑으로 ragdoll을 적용 시키도록 했습니다.

경험치는 좀비가 죽었을 경우 캐릭터가 획득하게 하였고 Money의 경우 콜오브듀티와 유사한 총격이 적중한 Skeletal Bone에 따라 각기 다른 Money를 획득하는 방식을 채택했습니다.

헤드샷의 경우에는 일발에 죽게 하였고 화면에 특별한 UI를 표시하게 하였습니다. 
