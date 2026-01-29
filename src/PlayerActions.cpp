#include <PlayerActions.h>
#include <PlayerComponent.h>

void MoveLeftAction::Execute(PlayerComponent& player)
{
	player.SetHorizontalVelocity(-player.GetPlayerSpeed());
}

void MoveRightAction::Execute(PlayerComponent& player)
{
	player.SetHorizontalVelocity(player.GetPlayerSpeed());
}

void JumpAction::Execute(PlayerComponent& player)
{
	if (player.IsGrounded()) {
		player.SetVerticalVelocity(-JumpSpeed);
	}
}

void ShootAction::Execute(PlayerComponent& player)
{
	player.ShootBullet();
}
