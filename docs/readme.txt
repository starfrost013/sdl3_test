   _________    ______  ______  ___    ____  ___       _       _____    ____  _____    ____  ____  ____      ____________________
  / ____/   |  / __ \ \/ / __ )/   |  / __ \/   |     | |     / /   |  / __ \/ ___/   / __ \/ __ \/ __ \    / / ____/ ____/_  __/
 / /   / /| | / /_/ /\  / __  / /| | / /_/ / /| |     | | /| / / /| | / /_/ /\__ \   / /_/ / /_/ / / / /_  / / __/ / /     / /   
/ /___/ ___ |/ ____/ / / /_/ / ___ |/ _, _/ ___ |     | |/ |/ / ___ |/ _, _/___/ /  / ____/ _, _/ /_/ / /_/ / /___/ /___  / /    
\____/_/  |_/_/     /_/_____/_/  |_/_/ |_/_/  |_|     |__/|__/_/  |_/_/ |_|/____/  /_/   /_/ |_|\____/\____/_____/\____/ /_/     
                                                          __                __                                                 
    ____       _____     ___                  ____ _     / /    ____       / /_       ____ _         _   __     ___       _____
   / __ \     / ___/    / _ \     ______     / __ `/    / /    / __ \     / __ \     / __ `/        | | / /    / _ \     / ___/
  / /_/ /    / /       /  __/    /_____/    / /_/ /    / /    / /_/ /    / / / /    / /_/ /         | |/ /    /  __/    / /    
 / .___/    /_/        \___/                \__,_/    /_/    / .___/    /_/ /_/     \__,_/          |___/     \___/    /_/     
/_/                                                         /_/                                                                

Pre-Alpha 6.0
February 2026
README

Capybara Wars is a 2d sidescrolling destruction, building, mining and team game...with capybaras. Inspired by Worms, Minecraft, and Fortnite it is "somewhere near the medium-high range of fun you can have online". This is an extremely, unbelievably early network test build with very little to do. 

Notes:

* There are no controls.
* If the server is there, you will see the world.
* "Win64" contains a Windows binary using (MinGW-w64 GCC 15.2.0 Windows 11 24H2), "Linux64" contains a Linux binary (GCC 13.3.0, Mint 22.3). The experience should be identical.
* World download takes a long time, this will be fixed in the future.

Command line options:
-memdebug - log all memory allocations (huge amounts of spam)
-netdebug - some network debug stuff
+set - set convar.

Convars that exist:
    - map - Map name.
    - mapSizeX - Map size X to create on the server. Must be above zero.
    - mapSizeY - Map size Y to create on the server. Must be above zero.
    - fsBasedir - base directory for filesystem operations
    - netMaxPlayers - max players (must be 1-32, default is 32) 
    - netMode - network mode (0 = client, 1 = listen server - doesn't work yet, 2 = dedicated server)
    - netPort - port
    - netServerAddress - server address
    - playerName - player name


