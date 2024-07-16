// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/AFoWHeroCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AFoWHeroCharacter::AFoWHeroCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 3.f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->SetIsReplicated(false);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AFoWHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitCustomCamera();
}

void AFoWHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitCustomCamera();
}

void AFoWHeroCharacter::InitCustomCamera()
{
	if (IsLocallyControlled())
	{
		const FRotator CharacterRotation = GetActorRotation();
		const FRotator CameraRotation = CameraBoom->GetComponentRotation();
		CameraBoom->SetRelativeRotation(FRotator(CameraRotation.Pitch, CharacterRotation.Yaw, CameraRotation.Roll));
		InitialCameraForward = GetActorForwardVector();
		InitialCameraRight = GetActorRightVector();
	}
}

void AFoWHeroCharacter::MoveCamera(const FVector2D& CameraMovementVector) const
{
	if (TopDownCameraComponent)
	{
		const auto CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const float ForwardValue = CameraMovementVector.Y;
		const float RightValue = CameraMovementVector.X;
		const float Speed = 10.f;
		const FVector NewLocation = CameraLocation + InitialCameraForward * ForwardValue * Speed + InitialCameraRight * RightValue * Speed;

		TopDownCameraComponent->SetWorldLocation(NewLocation);
	}
}

