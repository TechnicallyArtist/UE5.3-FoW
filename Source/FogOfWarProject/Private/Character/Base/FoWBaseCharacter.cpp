// Copyright TechnicallyArtist


#include "Character/Base/FoWBaseCharacter.h"
#include "FogOfWarProjectGameMode.h"
#include "Net/UnrealNetwork.h"

AFoWBaseCharacter::AFoWBaseCharacter(): TeamInfo()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFoWBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFoWBaseCharacter, TeamInfo);
}


void AFoWBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetTeamColor();
}

void AFoWBaseCharacter::Server_SetTeam_Implementation(const int32 InTeamNumber)
{
	if (!HasAuthority()) return;

	TeamInfo = Teams[InTeamNumber];
}


void AFoWBaseCharacter::SetTeamColor()
{
	// Set dynamic material for teams to assign different colors.
	BodyDynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0, BodyMaterialClass);

	if (BodyDynamicMaterial)
	{
		GetMesh()->SetMaterial(0, BodyDynamicMaterial);
		GetMesh()->SetMaterial(1, BodyDynamicMaterial);

		FLinearColor TeamColor = TeamInfo.Color;
		BodyDynamicMaterial->SetVectorParameterValue("TeamColor", TeamColor);
	}
}