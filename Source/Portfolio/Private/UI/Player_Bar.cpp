// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player_Bar.h"
#include "Components/ProgressBar.h"
#include "Component/CharacterComponent.h"

UPlayer_Bar::UPlayer_Bar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayer_Bar::SetMaxFigure(float _MaxFigure)
{
	if (_MaxFigure < KINDA_SMALL_NUMBER) {
		MaxFigure = 0.f;
		return;
	}

	MaxFigure = _MaxFigure;
}

void UPlayer_Bar::NativeConstruct()
{
	Super::NativeConstruct();

	Bar = Cast<UProgressBar>(GetWidgetFromName("BarWidget"));

	check(nullptr != Bar);
}
