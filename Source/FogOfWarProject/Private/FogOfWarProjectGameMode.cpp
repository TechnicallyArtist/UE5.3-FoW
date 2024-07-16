// Copyright Epic Games, Inc. All Rights Reserved.

#include "FogOfWarProjectGameMode.h"
#include "FogOfWarProjectPlayerController.h"
#include "Components/TrackerComponent.h"
#include "Character/AFoWHeroCharacter.h"
#include "Components/BeaconComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


AFogOfWarProjectGameMode::AFogOfWarProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFogOfWarProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

}

AActor* AFogOfWarProjectGameMode::ChoosePlayerStart_Implementation(AController* InPlayerController)
{
	const auto PlayerActor = Super::ChoosePlayerStart_Implementation(InPlayerController);

	// As soon as player is spawned, we will set their team.
	InPlayerController->OnPossessedPawnChanged.AddDynamic(this, &AFogOfWarProjectGameMode::SetPlayerTeam);

	return PlayerActor;
}

void AFogOfWarProjectGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFogOfWarProjectGameMode::StartFogOfWar, 3.0f, false);
}

void AFogOfWarProjectGameMode::SetPlayerTeam(APawn* OldPawn, APawn* NewPawn)
{
	if (const auto Character = Cast<AFoWHeroCharacter>(NewPawn))
	{
		Character->Server_SetTeam(PlayerTeam++);
	}
}

void AFogOfWarProjectGameMode::StartFogOfWar() const
{
	// For each player controller in the world
	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		// Cast to actual player controller
		const auto PlayerController = Cast<AFogOfWarProjectPlayerController>(PlayerControllerIterator->Get());
		if (!PlayerController) continue;

		const auto HeroCharacter = Cast<AFoWHeroCharacter>(PlayerController->GetPawn());
		if (!HeroCharacter) continue;

		SetEnemiesForPlayer(HeroCharacter);
			
		SetBeaconsForPlayer(HeroCharacter);
		
	}
}

void AFogOfWarProjectGameMode::SetEnemiesForPlayer(const AFoWHeroCharacter* const HeroCharacter) const
{
	// Only characters with TrackerComponent can track enemies
	const auto TrackerComponent = HeroCharacter->FindComponentByClass<UTrackerComponent>();
	if (!TrackerComponent) return;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		if (Actor == HeroCharacter) continue;// In this case, we are not our own worst enemy.

		// Cast to base character, as that is where we keep teams
		// But this can be any actor in your project, depends on your setup
		if (const auto PotentialEnemy = Cast<AFoWBaseCharacter>(Actor))
		{
			const auto HeroCharacterTeamInfo = ITeamInterface::Execute_GetTeamInfo(HeroCharacter);
			const auto EnemyCharacterTeamInfo = ITeamInterface::Execute_GetTeamInfo(PotentialEnemy);

			if ((HeroCharacterTeamInfo.Name != ETeam::None && EnemyCharacterTeamInfo.Name != ETeam::None) && (HeroCharacterTeamInfo.Name != EnemyCharacterTeamInfo.Name))
			{
				TrackerComponent->AssignEnemy(Actor);
			}
		}

	}
}

void AFogOfWarProjectGameMode::SetBeaconsForPlayer(const AFoWHeroCharacter* HeroCharacter) const
{
	TArray<AActor*> PotentialBeaconActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), PotentialBeaconActors);

	for (AActor* PotentialBeaconActor : PotentialBeaconActors)
	{
		if (PotentialBeaconActor == HeroCharacter) continue;

		if (const auto BeaconComponent = PotentialBeaconActor->FindComponentByClass<UBeaconComponent>())
		{
			const auto HeroCharacterTeamInfo = ITeamInterface::Execute_GetTeamInfo(HeroCharacter);
			const auto BeaconCharacterTeamInfo = ITeamInterface::Execute_GetTeamInfo(PotentialBeaconActor);

			if ((HeroCharacterTeamInfo.Name != ETeam::None && BeaconCharacterTeamInfo.Name != ETeam::None) && (HeroCharacterTeamInfo.Name == BeaconCharacterTeamInfo.Name))
			{
				if (const auto HeroTrackerComponent = HeroCharacter->FindComponentByClass<UTrackerComponent>())
				{
					BeaconComponent->AssignTracker(HeroTrackerComponent);
					HeroTrackerComponent->AssignBeacon(PotentialBeaconActor);
				}
			}
		}
	}
}
