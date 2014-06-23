#ifndef  SLICE-AND-PLAY_H
#define  SLICE-AND-PLAY_H

class ring {

public:
	
	//return an instance of ring;
	static ring&  instance();
	
	/**it will ring in word the number of step passed by parameter, 
	*\it'll include "fatto" - number of step in word - "passi"
	*int  the first parameter  is the number of step
	*int the second parameter is the volume of the player, 
	*      it has to be a value between 0 and 100 (0-lower 100-louder)
	*/
	void play_n_of_step(int,int);

	//ring the song of the defeat
	//the parameter is the volume of the player, 
	//      it has to be a value between 0 and 100 (0-lower 100-louder)
	void looser_Song(int);
	
	//ring the song of victory
	//the parameter is the volume of the player, 
	//      it has to be a value between 0 and 100 (0-lower 100-louder)
	void victory_Song(int);
	
private:
	
	//ring the numbers between 10 and 19 
	void say_tens_and_unit(int);
	
	//ring the tens between 20 and 90
	void say_tens (int);
	
	//ring the numbers between 1 and 9
	void say_unit (int);


	
	void addItemToVector( const unsigned char *,const unsigned int);
	void  numb_impl();



};
#endif
