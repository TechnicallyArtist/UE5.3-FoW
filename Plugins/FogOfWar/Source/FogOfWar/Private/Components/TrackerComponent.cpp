// Copyright TechnicallyArtist


#include "Components/TrackerComponent.h"

#include "Components/BeaconComponent.h"

UTrackerComponent::UTrackerComponent()
{
	SphereRadius = 400.f;
	PrimaryComponentTick.bCanEverTick = true;

	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UPrimitiveComponent::SetCollisionObjectType(ECC_WorldDynamic);
}


void UTrackerComponent::Server_UpdateEnemyVisibilityCounter(AActor* Enemy, bool bIsEnemyVisible)
{
	if (!GetOwner()->HasAuthority() || !Enemy) return;

	if (bIsEnemyVisible)
	{
		if (!EnemyVisibilityCounter.Contains(Enemy))
		{
			EnemyVisibilityCounter.Add(Enemy, 1);
		}
		else
		{
			EnemyVisibilityCounter[Enemy]++;
		}
	}
	else
	{
		if (EnemyVisibilityCounter.Contains(Enemy))
		{
			EnemyVisibilityCounter[Enemy]--;
			if (EnemyVisibilityCounter[Enemy] <= 0)
			{
				EnemyVisibilityCounter.Remove(Enemy);
				
			}
		}

	}
	if (EnemyVisibilityCounter.Contains(Enemy))
	{
		Client_SetEnemyVisibility(Enemy, true);
	}
	else
	{
		Client_SetEnemyVisibility(Enemy, false);
	}

}

void UTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
		SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		OnComponentBeginOverlap.AddDynamic(this, &UTrackerComponent::Server_OnActorBeginOverlap);
		OnComponentEndOverlap.AddDynamic(this, &UTrackerComponent::Server_OnActorEndOverlap);
	}
}

void UTrackerComponent::AssignEnemy(AActor* TrackedEnemy)
{
	// Only allow assigning enemies on server
	if (!GetOwner()->HasAuthority() || !TrackedEnemy) return;

	TrackedEnemyActors.Add(TrackedEnemy);
	Client_SetEnemyVisibility(TrackedEnemy, false);
}

void UTrackerComponent::AssignBeacon(AActor* TrackedBeaconActor)
{
	// Only allow assigning beacons on server and replicate on client
	if (!GetOwner()->HasAuthority()) return;

	if (const auto ActorBeaconComponent = TrackedBeaconActor->FindComponentByClass<UBeaconComponent>())
	{
		ActorBeaconComponent->GetOwner()->SetOwner(GetOwner());
		TrackedBeacons.Add(ActorBeaconComponent);
		Client_AssignBeacon(TrackedBeaconActor);
	} else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("AssignBeacon: NewBeacon is nullptr"));
	}

	
}

void UTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Server_CheckEnemyLineOfSightVisibility();
}


void UTrackerComponent::Server_OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!GetOwner()->HasAuthority() || (OtherActor == GetOwner()) || !OtherActor || !TrackedEnemyActors.Contains(OtherActor)) return;

	OverlappingEnemyActors.Add(OtherActor);
}

void UTrackerComponent::Server_OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!GetOwner()->HasAuthority() || !OtherActor || !TrackedEnemyActors.Contains(OtherActor)) return;

	Server_UpdateEnemyVisibilityCounter(OtherActor, false);

	OverlappingEnemyActors.Remove(OtherActor);
}


void UTrackerComponent::Server_CheckEnemyLineOfSightVisibility()
{
	if (!GetOwner()->HasAuthority()) return;

	// change OverlappingEnemyActors to TMap<AActor*, bool> to keep track of current visibility
	// in for, run visibility check, if it is different from current, call Server_UpdateEnemyVisibilityCounter

	// for each actor in OverlappingEnemyActors keys
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
			Server_UpdateEnemyVisibilityCounter(OverlappingEnemy, bIsEnemyVisible);
		}
	}
}


void UTrackerComponent::Client_SetEnemyVisibility_Implementation(AActor* Enemy, const bool bIsEnemyVisible)
{
	if (!Enemy) return;

	Enemy->SetActorHiddenInGame(!bIsEnemyVisible);

}


void UTrackerComponent::Client_AssignBeacon_Implementation(AActor* TrackedBeaconActor)
{

	const auto ActorBeaconComponent = TrackedBeaconActor->FindComponentByClass<UBeaconComponent>();
	if (!ActorBeaconComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Client_AssignBeacon_Implementation: NewBeacon is nullptr"));
		return;
	}
	TrackedBeacons.Add(ActorBeaconComponent);
}