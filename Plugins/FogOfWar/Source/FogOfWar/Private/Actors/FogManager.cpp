// Copyright TechnicallyArtist

#include "Actors/FogManager.h"

#include "Components/BeaconComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/TrackerComponent.h"
#include "Engine/Level.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

AFogManager::AFogManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 1 / 60.f;
	bOnlyRelevantToOwner = true;

	FogPostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("FogPostProcess"));
	FogPostProcessComponent->bEnabled = true;
	FogPostProcessComponent->bUnbound = true;
}

void AFogManager::BeginPlay()
{
	Super::BeginPlay();

	if (!FogMaterial)
	{
		return;
	}

	FoWMaterialInstance = UMaterialInstanceDynamic::Create(FogMaterial, this);
	FoWMaterialInstance->SetScalarParameterValue(FName("MapSize"), MapSize);
	FoWMaterialInstance->SetScalarParameterValue(FName("IsMapCentered"), bIsMapCentered);
	FoWMaterialInstance->SetScalarParameterValue(FName("CharacterVisibilityRadius"), CharacterVisibilityRadius);
	// set Texture Parameter to FloorMapTexture
	if (FloorMapTexture)
	{
		// UTexture2D inherits from UTexture, so we can pass it directly
		FoWMaterialInstance->SetTextureParameterValue(FName("FloorMap"), FloorMapTexture);
	}

	FogPostProcessComponent->Settings.WeightedBlendables.Array.Empty();
	FogPostProcessComponent->AddOrUpdateBlendable(TScriptInterface<IBlendableInterface>(FoWMaterialInstance));
}

void AFogManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacterLocation();
	UpdateBeaconLocations();
}

TArray<AActor*> AFogManager::GetAllNonOccludingActors() const
{
	const FName OccluderTag = "VISION_OCCLUDER";
	const TArray<AActor*> AllActors = GetWorld()->GetCurrentLevel()->Actors;

	TArray<AActor*> NonOccludingActors;
	for (const auto Actor : AllActors)
	{
		if (Actor && !Actor->ActorHasTag(OccluderTag))
		{
			NonOccludingActors.Add(Actor);
		}
	}
	return NonOccludingActors;
}


void AFogManager::UpdateCharacterLocation()
{
	if (!GetLocalTracker() || !GetLocalTracker()->GetOwner()) return;

	const FVector WorldLocation = LocalTracker->GetOwner()->GetActorLocation();
	// TODO: Why does only 3000 work here and not MapSize
	const FVector CharacterUVLocation = FVector(WorldLocation.X / MapSize, WorldLocation.Y / MapSize, 0);
	const FVector CharacterUVLocationFixed = FVector(WorldLocation.X / 3000, WorldLocation.Y / 3000, 0);

	FoWMaterialInstance->SetVectorParameterValue(FName("CharacterUVLocation"), CharacterUVLocation);
	FoWMaterialInstance->SetVectorParameterValue(FName("CharacterUVLocationFixed"), CharacterUVLocationFixed);
}

UTrackerComponent* AFogManager::GetLocalTracker()
{
	if (!LocalTracker)
	{
		if (const auto World = GetWorld())
		{
			if (const auto Controller = World->GetFirstPlayerController())
			{
				const auto LocalCharacter = Controller->GetCharacter();
				if (const auto TrackerComponent = LocalCharacter->FindComponentByClass<UTrackerComponent>())
				{
					LocalTracker = TrackerComponent;
				}
			}
		}
	}

	return LocalTracker;

}

#pragma region Beacon locations

void AFogManager::UpdateBeaconLocations()
{
	BeaconLocationsAndSizes.Empty();
	if (!GetLocalTracker()) return;

	for (const auto Beacon : GetLocalTracker()->GetTrackedBeacons())
	{
		if (!Beacon) continue;

		const FVector WorldLocation = Beacon->GetComponentLocation();
		const FVector4 LocationAndSize = bIsMapCentered ?
			FVector4(WorldLocation.X + MapSize / 2, WorldLocation.Y + MapSize / 2, WorldLocation.Z, Beacon->VisibilityRadius):
			FVector4(WorldLocation.X, WorldLocation.Y, WorldLocation.Z, Beacon->VisibilityRadius);

		BeaconLocationsAndSizes.Add(LocationAndSize);
	}
}
#pragma endregion
