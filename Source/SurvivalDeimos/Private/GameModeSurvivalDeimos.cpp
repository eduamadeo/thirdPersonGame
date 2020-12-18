// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeSurvivalDeimos.h"
#include "PersonagemTPS.h"
#include "Kismet\GameplayStatics.h"
#include "Blueprint\UserWidget.h"

void AGameModeSurvivalDeimos::BeginPlay()
{
	Super::BeginPlay();
	APersonagemTPS* Player = Cast<APersonagemTPS>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (PlayerHUDClass != nullptr)
	{
		WidgetAtual = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (WidgetAtual)
		{
			WidgetAtual->AddToViewport();
		}
	}
}

