# RunningGun
Arcade game that I made for a Game Jam back in 2020. Maintaining and expanding upon the codebase now.

## Dependencies
- SDL3
- SDL_image
- SDL_ttf
- simdjson
- Visual Studio 2017 toolchain (project targets VS2017)

## Engine Structure
- **Core loop**: `Game` owns the main loop and frame timing, and `GameMode` drives game-specific setup.
- **World/entities**: `World` stores active `Entity` instances; entities are composed from `Component` types.
- **Components**: behavior is built from modular components, registered via the prefab/component registry.
- **Prefabs**: prefab factories assemble entities with their component sets for quick spawning.
- **Input**: `InputManager` wraps SDL events and maps them to game actions.
- **Rendering**: `Sprite` and `UIText` wrap SDL rendering; `ResourceHandler` loads textures and fonts.
