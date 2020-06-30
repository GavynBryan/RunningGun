# RunningGun
Arcade-style game made in SFML

Download the SFML library at
https://www.sfml-dev.org/download/sfml/2.5.1/
The intended build target is Visual Studio 2017. Has not been tested on anything else.

# ToDo
The code was put together in about 8 days for a GameJam challenge and is currently in the process of being "cleaned up". Lots of decisions were made without me really knowing what the scope of some of these structures were going to be.

- Tidy up the collision loop to use std::pairs and simultaneously call OnCollide() for both objects
- Create an input manager class that runs outside of the player's controller, allowing for more fluid control of keyboard inputs (And would increase the difficulty by removing an exploit)
- Maybe expand on it a little bit? I have a decent codebase to work with now. :)
