# tempus-player-run-fetcher
A small tool written in C++ to quickly grab all of a player's Soldier and Demoman map runs on the Tempus Jump Network.

This fork was made to both update the map list (as Hat Kid's was 8 months out of date) and to make compiling the project easier for me. 

## Usage
To use the program, simply run the given executable:

`./tprf`

When prompted, enter a valid SteamID in STEAM_X:Y:ZZZZ format. After running for a while the script will output a `runs.txt` file next to the executable that contains all of the given runs, as well as a short stat summary at the bottom

Example of what the output looks like: https://pastebin.com/kPZdjPnU

## Compiling (Linux)
Install the required packages
`sudo apt install -y nlohmann-json3-dev libpoco-dev libcurlpp-dev`

Compile the program using the given Makefile
`make`

## Compiling (Windows)
idk
