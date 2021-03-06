#include "Cursor.h"


Cursor::Cursor(unsigned char pos, unsigned char screen){
	cursPos = pos;
	currScreen = screen;
}

/* Name: moveLeft
 * Description: moves the cursor position left and handles wrapping
 * Inputs: none
 * Outputs: none
 */
void Cursor::moveLeft(){
	if (cursPos == UPPER_LEFT) {
		if (currScreen == LCD_UPPER_SCR) {
			currScreen = LCD_LOWER_SCR;
			cursPos = LOWER_RIGHT;
		} else if (currScreen == LCD_LOWER_SCR) {
			currScreen = LCD_UPPER_SCR;
			cursPos = LOWER_RIGHT;
		}
	} else {
		cursPos -= 1;
	}
}

/* Name: moveRight
 * Description: moves the cursor position right and handles wrapping
 * Inputs: none
 * Outputs: none
 */
void Cursor::moveRight(){
	if (cursPos == LOWER_RIGHT) {
		if (currScreen == LCD_UPPER_SCR) {
			currScreen = LCD_LOWER_SCR;
			cursPos = UPPER_LEFT;
		} else if (currScreen == LCD_LOWER_SCR) {
			currScreen = LCD_UPPER_SCR;
			cursPos = UPPER_LEFT;
		}
	} else {
		cursPos += 1;
	}
}

/* Name: moveUp
 * Description: moves the cursor position up and handles wrapping
 * Inputs: none
 * Outputs: none
 */
void Cursor::moveUp(){
	if ((cursPos >= UPPER_LEFT) && (cursPos <= UPPER_RIGHT)) {
		if (currScreen == LCD_UPPER_SCR) {
			currScreen = LCD_LOWER_SCR;
			cursPos += ONE_LINE;
		} else if (currScreen == LCD_LOWER_SCR) {
			currScreen = LCD_UPPER_SCR;
			cursPos += ONE_LINE;
		}
	} else {
		cursPos -= ONE_LINE;
	}
}

/* Name: moveDown
 * Description: moves the cursor position down and handles wrapping
 * Inputs: none
 * Outputs: none
 */
void Cursor::moveDown(){
	if ((cursPos >= LOWER_LEFT) && (cursPos <= LOWER_RIGHT)) {
		if (currScreen == LCD_UPPER_SCR) {
			currScreen = LCD_LOWER_SCR;
			cursPos -= ONE_LINE;
		} else if (currScreen == LCD_LOWER_SCR) {
			currScreen = LCD_UPPER_SCR;
			cursPos -= ONE_LINE;
		}
	} else {
		cursPos += ONE_LINE;
	}
}
