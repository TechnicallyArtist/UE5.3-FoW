// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FogOfWarProjectGameMode.generated.h"


class AFoWHeroCharacter;

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None	UMETA(DisplayName = "None"),
	Red		UMETA(DisplayName = "Red Team"),
	Blue	UMETA(DisplayName = "Blue Team")
};

USTRUCT(BlueprintType)
struct FTeamData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TeamIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeam Name;
};

/**
 * An example of game mode implementing Fog of War plugin.
 */
UCLASS(minimalapi)
class AFogOfWarProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFogOfWarProjectGameMode();

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* InPlayerController) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

private:
	/**
	 * An example of setting team visibility after everyone has joined the game.
	 * In your project you'd probably call this on MatchStart if this was AGameMode.
	 * Here, we will just wait for a couple of seconds and then set everyone's enemies.
	 */
	void StartFogOfWar() const;

private:
	/** We will use PlayerTeam to set their team. */
	UPROPERTY()
	int32 PlayerTeam = 0;

	/**
	 * Fog of War
	 * This will probably be different in your game.
	 * Each new player gets a team assigned from ETeam enum.
	 * TeamInfo will decide who player can see.
	 * It is just an example of plugin usage.
	 */
	UFUNCTION()
	void SetPlayerTeam(APawn* OldPawn, APawn* NewPawn);


	void SetEnemiesForPlayer(const AFoWHeroCharacter* HeroCharacter) const;
	void SetBeaconsForPlayer(const AFoWHeroCharacter* HeroCharacter) const;
};



