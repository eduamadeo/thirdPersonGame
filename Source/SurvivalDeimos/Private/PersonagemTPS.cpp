// Fill out your copyright notice in the Description page of Project Settings.


#include "PersonagemTPS.h"
#include "Components\InputComponent.h"
#include "Camera\CameraComponent.h"
#include "GameFramework\SpringArmComponent.h"
#include "GameFramework\PawnMovementComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "GameFramework\Character.h"
#include "Engine\World.h"
#include "Arma.h"
#include "Engine\EngineTypes.h"
#include "Engine\Public\WorldCollision.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/LatentActionManager.h"

// Sets default values
APersonagemTPS::APersonagemTPS()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmCamera = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArmCamera"));
	SpringArmCamera->TargetArmLength = 200.f;
	SpringArmCamera->bUsePawnControlRotation = true;
	SpringArmCamera->AddRelativeLocation(FVector(0.f, 40.f, 50.f));
	SpringArmCamera->bEnableCameraLag = true;
	SpringArmCamera->CameraLagSpeed = 40.f;
	SpringArmCamera->SetupAttachment(RootComponent);

	CameraPersonagem = CreateDefaultSubobject<UCameraComponent>(FName("CameraPersonagem"));
	CameraPersonagem->SetupAttachment(SpringArmCamera);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCharacterMovement()->AirControl = 3.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->CrouchedHalfHeight = 70.f;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void APersonagemTPS::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Parametros;
	Parametros.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ArmaPlayer = GetWorld()->SpawnActor<AArma>(BP_ArmaTipoRifle, FTransform(), Parametros);
	ArmaPlayer->AttachToComponent(Cast<USceneComponent>(GetMesh()), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("SocketDaArma"));
	
}

void APersonagemTPS::MoverParaFrente(float Valor)
{
	AddMovementInput(GetActorForwardVector() * Valor);
}

void APersonagemTPS::MoverDireita(float Valor)
{
	AddMovementInput(GetActorRightVector() * Valor);
}

void APersonagemTPS::Pular()
{
	bEstaPulando = true;
	float X = GetCapsuleComponent()->GetRelativeLocation().X;
	float Y = GetCapsuleComponent()->GetRelativeLocation().Y + 40;
	float Z = GetCapsuleComponent()->GetRelativeLocation().Z + 100;
	FRotator RotCapsule = GetCapsuleComponent()->GetRelativeRotation();
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.UUID = GetUniqueID();
	LatentInfo.ExecutionFunction = "Jump";
	LatentInfo.Linkage = 0;
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), FVector(X, Y, Z), RotCapsule, false, false, 0.5f, false, EMoveComponentAction::Move, LatentInfo);
}

void APersonagemTPS::PararPulo()
{
	bEstaPulando = false;
}

void APersonagemTPS::Agachar()
{
	Crouch();
}

void APersonagemTPS::Levantar()
{
	UnCrouch();
}

void APersonagemTPS::Atirar()
{
	ArmaPlayer->Atirar();
}

// Called every frame
void APersonagemTPS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APersonagemTPS::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoverParaFrente", this, &APersonagemTPS::MoverParaFrente);
	PlayerInputComponent->BindAxis("MoverDireita", this, &APersonagemTPS::MoverDireita);
	PlayerInputComponent->BindAxis("OlharCimaBaixo", this, &APersonagemTPS::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("OlharDireitaEsquerda", this, &APersonagemTPS::AddControllerYawInput);
	PlayerInputComponent->BindAction("Agachar", EInputEvent::IE_Pressed, this, &APersonagemTPS::Agachar);
	PlayerInputComponent->BindAction("Agachar", EInputEvent::IE_Released, this, &APersonagemTPS::Levantar);
	PlayerInputComponent->BindAction("Pular", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Pular", EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Atirar", EInputEvent::IE_Pressed, this, &APersonagemTPS::Atirar);
}

void APersonagemTPS::SetHealth(float Dano)
{
	if (Health > 0.0f)
	{
		Health -= Dano;
	}
	else
	{
		bEstaMorto = true;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		check(PlayerController);
		PlayerController->GetPawn()->DisableInput(PlayerController);
	}
}

float APersonagemTPS::GetHealth()
{
	return Health;
}

bool APersonagemTPS::IsEstaMorto()
{
	return bEstaMorto;
}

