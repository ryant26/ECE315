#ifndef _LCD_H_
#define _LCD_H_

#define LOWER_RIGHT 		79
#define UPPER_RIGHT			39
#define UPPER_LEFT			0
#define LOWER_LEFT			40
#define ONE_LINE			40
#define LCD_UPPER_SCR		1
#define LCD_LOWER_SCR		2

class Cursor
{

private:
	unsigned char cursPos, currScreen;

public:
	Cursor(unsigned char, unsigned char);
	~Cursor();

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
};

#endif
