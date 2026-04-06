#include "CH4GameMode.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "UObject/ConstructorHelpers.h"

ACH4GameMode::ACH4GameMode()
{
	static ConstructorHelpers::FClassFinder<ACH4Character>
		PlayerCharacter(TEXT("Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));

	if (PlayerCharacter.Class)
	{
		DefaultPawnClass = PlayerCharacter.Class;
	}
}
