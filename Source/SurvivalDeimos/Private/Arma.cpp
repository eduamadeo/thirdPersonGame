// Fill out your copyright notice in the Description page of Project Settings.


#include "Arma.h"
#include "Components\SkeletalMeshComponent.h"
#include "Components\ArrowComponent.h"
#include "Engine\EngineTypes.h"
#include "Engine\Public\CollisionQueryParams.h"
#include "Engine\World.h"
#include "Engine\Public\DrawDebugHelpers.h"
#include "UObject\ConstructorHelpers.h"
#include "Engine\SkeletalMesh.h"
#include "Particles\ParticleSystem.h"
#include "Kismet\GameplayStatics.h"
#include "Animation\SkeletalMeshActor.h"
#include "Materials\MaterialInstance.h"
#include "Math\UnrealMathUtility.h"
#include "Sound\SoundBase.h"
#include "GameFramework\Character.h"
#include "BotCharacter.h"
#include "PersonagemTPS.h"

// Sets default values
AArma::AArma()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EfeitoMuzzle = nullptr;
	ImpactoSangue = nullptr;
	ImpactoGeral = nullptr;
	DecalImpactoGeral = nullptr;
	SomDoTiro = nullptr;

	MalhaDaArma = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MalhaDaArma"));
	RootComponent = MalhaDaArma;

	ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshDaArma(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshDaArma.Succeeded())
	{
		MalhaDaArma->SetSkeletalMesh(MeshDaArma.Object);
	}

	SetaDaArma = CreateDefaultSubobject<UArrowComponent>(FName("SetaCanoDaArma"));
	SetaDaArma->SetupAttachment(MalhaDaArma, FName("MuzzleFlashSocket"));
	SetaDaArma->SetRelativeLocation(FVector(1.4f, 0.f, -1.2f));
	SetaDaArma->SetRelativeScale3D(FVector(0.3f, 0.8f, 0.7f));

}

// Called when the game starts or when spawned
void AArma::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArma::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArma::Atirar()
{
	SetaDaArma = FindComponentByClass<UArrowComponent>();
	if (SetaDaArma)
	{
		FVector Inicio = SetaDaArma->GetComponentLocation();
		FVector Direcao = SetaDaArma->GetComponentRotation().Vector();
		FVector Fim = Inicio + (Direcao * 1000);

		FHitResult InfoImpacto;
		FCollisionQueryParams Parametros;
		Parametros.AddIgnoredActor(this);
		Parametros.AddIgnoredActor(GetOwner());
		Parametros.bTraceComplex = true;

		bool AcertouEmAlgo = GetWorld()->LineTraceSingleByChannel(InfoImpacto, Inicio, Fim, ECollisionChannel::ECC_Visibility, Parametros);

		if (SomDoTiro)
		{
			UGameplayStatics::PlaySoundAtLocation(SetaDaArma, SomDoTiro, Inicio);
		}

		if (AcertouEmAlgo)
		{
			UE_LOG(LogTemp, Warning, TEXT("Acertou em algo"));
			AActor* Ator = InfoImpacto.GetActor();
			if (Ator->GetClass()->IsChildOf(ACharacter::StaticClass()) && ImpactoSangue) 
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactoSangue, InfoImpacto.Location, InfoImpacto.ImpactNormal.Rotation(), true);

				APersonagemTPS* Jogador = Cast<APersonagemTPS>(Ator);
				if (Jogador != nullptr)
				{
					Jogador->SetHealth(10.f);
				}
				else
				{
					ABotCharacter* Inimigo = Cast<ABotCharacter>(Ator);
					if (Inimigo != nullptr)
					{
						Inimigo->SetHealth(5.f);
					}
				}
			}
			else if (ImpactoGeral) 
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactoGeral, InfoImpacto.Location, InfoImpacto.ImpactNormal.Rotation(), true);

				FVector TamanhoVariavel = FVector(FMath::RandRange(10.f, 50.f));
				UGameplayStatics::SpawnDecalAttached(DecalImpactoGeral, TamanhoVariavel, InfoImpacto.GetComponent(), NAME_None, InfoImpacto.Location, InfoImpacto.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 60.f);
			}
		}

		DrawDebugLine(GetWorld(), Inicio, Fim, FColor::Red, false, 5.0f, (uint8)0, 1.0f);

		if (EfeitoMuzzle)
		{
			FVector Localizacao = SetaDaArma->GetComponentLocation();
			FRotator Rotacao = SetaDaArma->GetComponentRotation();
			FVector Escala = FVector(0.9f);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EfeitoMuzzle, Localizacao, Rotacao, Escala, true);
		}
	}
}

