# (멀티플레이) TPS 좀비 서바이벌
 
- 사용엔진 Unreal 5.1
  
- 장르 : TPS 좀비 서바이벌

- 개요 : 멀티플레이가 가능한 3인칭 FPS 좀비 서바이벌 게임입니다. Wave 마다 쏟아져나오는 좀비를 막는 형식으로 구성되어 있습니다.
  좀비의 부위를 쏘아 돈을 벌고 3Wave마다 Room으로 돌아가 재정비를 하며 총 9Wave를 막으면 클리어 합니다.
  
시연 영상:
https://www.youtube.com/watch?v=R8HM7mRmQdQ

# 조작
W,A,S,D (이동)  
  
마우스 좌클릭 (공격)  
  
마우스 우클릭 (조준) 

SPACE (점프)  

R (재장전)
  
Z (단발/연사 전환)  

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

멀티플레이 환경을 언리얼 PIE모드에서 listen server로 실행했기 때문에 완벽한 seamless travel이 불가능했다. 
그래서 단순한 level 이동의 경우에는 LoadingController에서 목적지를 ParseOption을 받아 Loading 화면을 보여주고 넘어가는 방식으로 하였고 
