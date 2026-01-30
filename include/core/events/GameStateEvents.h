#pragma once

#include <core/events/MulticastDelegate.h>

class Entity;

/**
 * Centralized game state events for decoupling game components from game mode logic.
 * Components fire events here, and the GameMode subscribes to handle state transitions.
 *
 * This follows the Single Responsibility Principle - components handle their own behavior,
 * while the GameMode handles game state transitions.
 */
class GameStateEvents
{
public:
	/**
	 * Fired when the player dies (lives reach zero).
	 * Subscribers receive a pointer to the player entity that died.
	 */
	MulticastDelegate<Entity*> OnPlayerDied;

	/**
	 * Fired when a boss enemy (bull) is defeated.
	 * Subscribers receive a pointer to the boss entity that was defeated.
	 */
	MulticastDelegate<Entity*> OnBossDied;

	/**
	 * Fired when player health changes.
	 * Subscribers receive the new health value.
	 */
	MulticastDelegate<int> OnPlayerHealthChanged;

	/**
	 * Clear all event subscriptions.
	 * Called during scene cleanup/reset.
	 */
	void ClearAll()
	{
		OnPlayerDied.Clear();
		OnBossDied.Clear();
		OnPlayerHealthChanged.Clear();
	}
};
