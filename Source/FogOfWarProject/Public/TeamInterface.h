// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"
#include "FogOfWarProjectGameMode.h"
#include "UObject/Interface.h"
#include "TeamInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTeamInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * A demo project's interface for teams.
 * It should be implemented by any actor that belongs to a team
 * and/or should be invisible to other team's fog of war.
 */
class FOGOFWARPROJECT_API ITeamInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTeamData GetTeamInfo() const;
};
