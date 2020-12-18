// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAIController.h"
#include "Perception\PawnSensingComponent.h"
#include "BehaviorTree\BehaviorTree.h"
#include "BehaviorTree\BehaviorTreeComponent.h"
#include "BehaviorTree\BlackboardComponent.h"
#include "GameFramework\Pawn.h"
#include "BotCharacter.h"
#include "Engine\Engine.h"
#include "Arma.h"
#include "PersonagemTPS.h"

ABotAIController::ABotAIController()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(FName("PawnSensingComp"));
	PawnSensingComp->SensingInterval = .25f;
	PawnSensingComp->bOnlySensePlayers = true;
	PawnSensingComp->SetPeripheralVisionAngle(180.f);
	PawnSensingComp->SightRadius = 3000.f;

	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(FName("BehaviorTreeComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(FName("BlackBoardComp"));
}

void ABotAIController::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ABotAIController::OnSeePawn);

	if (BehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComp->StartTree(*BehaviorTree);
		BlackboardComp->SetValueAsBool("DevePerambular", true);
	}
}

void ABotAIController::OnSeePawn(APawn * SensedPawn)
{
	ABotCharacter* Bot = Cast<ABotCharacter>(GetPawn());
	APersonagemTPS* Player = Cast<APersonagemTPS>(SensedPawn);
	if (Bot->GetHealth() <= 0.0f || Player->IsEstaMorto())
	{
		BlackboardComp->SetValueAsObject("Inimigo", nullptr);
		return;
	}
	else
	{
		if (BlackboardComp && SensedPawn)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Eu estou te vendo"));

			BlackboardComp->SetValueAsObject("Inimigo", SensedPawn);
			BlackboardComp->SetValueAsBool("DevePerambular", false);

			Bot->ArmaInimigo->Atirar();
		}
	}
}
