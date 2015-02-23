#ifndef CURSOR_H
#define CURSOR_H

#define LOWER_RIGHT 		79
#define UPPER_RIGHT			39
#define UPPER_LEFT			0
#define LOWER_LEFT			40
#define ONE_LINE			40
#define LCD_UPPER_SCR		1
#define LCD_LOWER_SCR		2

class Cursor
{

public:		// No need for setters or getters in this small application
	unsigned char cursPos, currScreen;


	Cursor(unsigned char, unsigned char);
	~Cursor();

	// Moves cursor left, handles edges of screens
	void moveLeft();

	// Moves cursor right, handles edges of screens
	void moveRight();

	// Moves cursor up, handles edges of screens
	void moveUp();

	// Moves cursor down, handles edges of screens
	void moveDown();
};

#endif
