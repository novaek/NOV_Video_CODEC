# NOV_Video_CODEC
I wanted to make my own video codec (because why not ? )

I present to you my own video encoder.
My goal was not to optimise as much as i could that programme, but only to make it working.

Its actually in developpement, so **It does not (really) work**.

## *1) How does it work ?*
Currently, the only thing that works is just the following :

- Check if a file is a correct video file : with `0xADDA` HEADER
- Check its number of frames (up to 65535, almost 45m30s) but developpement can change this limite
- My own filetype (not a different extension but a different way to handle data)
- Check the number of frames
- Optimise the data within the file : homemade Delta frame methode + other solutions I will find (WIP)
- Get a video Output from a file (most useful I'd say)
- recording screen video feed


## *2) What do I plan to add ?:*
I plan to add the followings

- nothing more, if you have suggestions, send me a mail :)

## *3) What I won't implement ?*

I won't implement the following :

- Sound with the video, or sound. (no music not anything)
- Conversion from my codec to another, or the other way around.

**Please keep in mind that I'm currently learning C++ with this project, and that i just plan to make a POC (Proof Of Concept).**

## 4) What can you try ? 

For the moment, the only files i will Upload are : 

- ENCD.cpp : the masterpeace : the code that check for everything said in part 1) .
- LOD.cpp : the file I use to encode raw HEX values to a file to try ENDC.cpp
- KGB : the file i use as a test file.
- SCRD.cpp : used for screen recording, writes the KGB file.

## 5) to use it : 

- Compile the .cpp files (i'm using `cl ENCD.cpp` and `cl SCRD.cpp` on windows).
- Run SCRD.exe to record your screen (by feault it's on 24 frames/s and using 255 frames, you can modify the source code to recorde more)
- Enter your filename and wait for the programme to exit.
- Run ENCD.cpp, enter your filename and wait for the video feed (long timespan was optimised and patched).

### ***Please note that the colours might not be as accurate as wanted, its not a featur but a bug that will be patched soon***

### Last notice : for now its just a Windows programme (using User32.dll, and i might patch it later)
