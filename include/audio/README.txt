Project by Group 31 (Filippo Garetti, Marco Mezzanotte, Tommaso Calcina)

This is the audio feedback for the pedometer.

It'll work only if 
	>you insert into the Makefile, 
	 in the variable SRC this files: "<path>/player.cpp" "<path>/slice-and-play.cpp" "<path>/adpcm.c"  .

	>you include "<path>/slice-and-play.h" in your main file

	>you use miosix
	
	
It has inside a converter to create new file suitable for the player.
The class to call all the different functionalities is "ring".

This are the functionalities:

> void play_n_of_step(int,int)

	it gets an int variable and it will play "fatto"-number of step-"passi", the second int is for the volume (from 0 to 100)

> void looserSong(int)

	it will play "perso" -the int parameter is for the volume (from 0 to 100)

>void victorySong(int)
	
	it will play " vinto" -the int parameter is for the volume (from 0 to 100)
