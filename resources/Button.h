#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#define BUTTON_STATES_COUNT 5

typedef enum {
	eButtonIdle,
	eButtonHover,
	eButtonHover1000,
	eButtonActive,
	eButtonInactive
} ButtonState;

class Button
{
public:
	Button();
	Button(const int, const int, const std::string*);
	~Button();

	void initButton(const int&,const int &, const std::string*);
	int getPosX() { return pos_x_; }
	int getPosY() { return pos_y_; }

	const std::string getImageString(const int&);

	void setButtonState(ButtonState);
	ButtonState getButtonState() { return state_; }

private:
	int pos_x_, pos_y_, hoverTime_;
	const std::string *buttonImage_;

	ButtonState state_;
};
#endif