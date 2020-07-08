# Playah!

**Work in progress** media player written in Qt (and Ncurses for the CLI version), because I like jumping on bandwagons

**Todo**

- Album art display
- Playlist loading (m3u, etc.)
- Info handling
- idk what else

## Build

Create shadowbuild directory

`mkdir _build && cd _build`

Make the library first and let the system see it

`mkdir lib && cd lib`

`qmake ../../lib && make`

`export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH`

Make the rest of the project

`cd ..`

`qmake .. && make`

## Run

`cli/playah_cli` = ncurses cli version

`gui/playah` = qt gui version
