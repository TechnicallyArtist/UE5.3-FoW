// Copyright TechnicallyArtist


#include "Components/BeaconComponent.h"

#include "Components/TrackerComponent.h"

UBeaconComponent::UBeaconComponent()
{
	SphereRadius = VisibilityRadius - 100; // We make the sphere radius a bit smaller because visibility radius had a fade to it
	SetIsReplicatedByDefault(true);

	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UPrimitiveComponent::SetCollisionObjectType(ECC_WorldDynamic);
	UPrimitiveComponent::SetEnableGravity(false);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 3.f;
}

void UBeaconComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		OnComponentBeginOverlap.AddDynamic(this, &UBeaconComponent::Server_OnActorBeginOverlap);
		OnComponentEndOverlap.AddDynamic(this, &UBeaconComponent::Server_OnActorEndOverlap);
	}
}

void UBeaconComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Server_CheckEnemyLineOfSightVisibility();
}

void UBeaconComponent::AssignTracker(UTrackerComponent* NewMasterTracker)
{
	// Only allow assigning trackers on server
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	MasterTracker = NewMasterTracker;
}

void UBeaconComponent::Server_OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GetOwner()->HasAuthority() || (OtherActor == GetOwner()) || !OtherActor || !MasterTracker || !MasterTracker->GetTrackedEnemyActors().Contains(OtherActor)) return;

	OverlappingEnemyActors.Add(OtherActor);
}

void UBeaconComponent::Server_CheckEnemyLineOfSightVisibility()
{
	if (!GetOwner()->HasAuthority()) return;

	for (const auto& OverlappingEnemyEntry : OverlappingEnemyActors)
	{
		AActor* OverlappingEnemy = OverlappingEnemyEntry.Key;
		FHitResult HitResult;
		const FVector StartLocation = GetOwner()->GetActorLocation();
		const FVector EndLocation = OverlappingEnemy->GetActorLocation();
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		CollisionQueryParams.AddIgnoredActor(OverlappingEnemy);

		// If se didn't hit any static object between us and overlapping actor, then we can see each other
		const bool bIsEnemyVisible = !GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, CollisionQueryParams);
		if (bIsEnemyVisible != OverlappingEnemyEntry.Value)
		{
			OverlappingEnemyActors[OverlappingEnemy] = bIsEnemyVisible;
			MasterTracker->Server_UpdateEnemyVisibilityCounter(OverlappingEnemy, bIsEnemyVisible);
		}
	}
}


void UBeaconComponent::Server_OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!GetOwner()->HasAuthority() || !OtherActor || !MasterTracker || !MasterTracker->GetTrackedEnemyActors().Contains(OtherActor)) return;

	MasterTracker->Server_UpdateEnemyVisibilityCounter(OtherActor, false);

	OverlappingEnemyActors.Remove(OtherActor);
		
}
