// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "TrackerComponent.generated.h"

class UBeaconComponent;
/**
 * A sphere component, responsible for testing enemy overlaps
 * and checking if they are visible to owner from first person perspective.
 */
UCLASS(Blueprintable)
class FOGOFWAR_API UTrackerComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UTrackerComponent();

	/** TrackedEnemy is any actor which should be tracked,
	 * meaning it is covered by fog of war and uncovered when close by.
	 * This should only be called on server.
	 */
	UFUNCTION(BlueprintCallable, Category="Fog of War")
	void AssignEnemy(AActor* TrackedEnemy);

	UFUNCTION(BlueprintCallable, Category="Fog of War")
	void AssignBeacon(AActor* TrackedBeaconActor);

	UFUNCTION()
	TArray<AActor*> GetTrackedEnemyActors() const { return TrackedEnemyActors; }

	UFUNCTION()
	TArray<UBeaconComponent*> GetTrackedBeacons() const { return TrackedBeacons; }

#pragma region Enemy tracking and visibility
public:
	void Server_UpdateEnemyVisibilityCounter(AActor* Enemy, bool bIsEnemyVisible);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** All actors in world which are hidden when out of sight and tracked. */
	UPROPERTY(VisibleAnywhere, Category="Fog of War")
	TArray<AActor*> TrackedEnemyActors;

	UPROPERTY(VisibleAnywhere, Category = "Fog of War")
	TArray<UBeaconComponent*> TrackedBeacons;

	/** Actors which are close enough to owner to be tested for line of sight visibility. */
	UPROPERTY()
	TMap<AActor*, bool> OverlappingEnemyActors;

	UPROPERTY()
	TMap<AActor*, int32> EnemyVisibilityCounter;

	/** Add OtherActor to OverlappingEnemyActors array */
	UFUNCTION()
	void Server_OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** When enemy is close enough, check that there are no blocking objects hiding their visibility. */
	UFUNCTION()
	void Server_CheckEnemyLineOfSightVisibility();

	/** Remove OtherActor from OverlappingEnemyActors array and hide them. */
	UFUNCTION()
	void Server_OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Visibility is tested on server, and then updated only on client, because it is local to them. */
	UFUNCTION(Client, Reliable)
	void Client_SetEnemyVisibility(AActor* Enemy, bool bIsEnemyVisible);

#pragma endregion

#pragma region Beacon tracking
private:

	UFUNCTION(Client, Reliable)
	void Client_AssignBeacon(AActor* TrackedBeaconActor);
#pragma endregion
};
