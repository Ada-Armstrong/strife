# STRIFE
## What is strife?
Strife is a bot that will play [Feud](https://store.steampowered.com/app/863400/Feud) with you on the command line.

## How to Play
To swap enter the coordinate of the tiles to swap using the following format. \
Example: a4 b4 \
The piece at a4 and b4 will be swapped (equivalent to b4 a4)

To use an action enter the coordinate of the piece followed by the piece(s) to use the action on. \
Example: b3 b2 c2 \
The piece at b3 will use it's action on b2 and c2

## Compile 
```bash
gcc *.c -lreadline -lm -o strife
```

