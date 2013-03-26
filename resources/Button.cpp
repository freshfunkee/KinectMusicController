#include "Button.h"

Button::Button()
{
	state_ = eButtonIdle;
	hoverTime_ = 0;
}

Button::Button(const int x, const int y, const std::string *img) : pos_x_(x), pos_y_(y), buttonImage_(img)
{
	state_ = eButtonIdle;
	hoverTime_ = 0;
}

Button::~Button()
{
	delete [] buttonImage_;
}

void Button::initButton(const int &x, const int &y, const std::string *images)
{
	pos_x_ = x;
	pos_y_ = y;
	buttonImage_ = images;
}

const std::string Button::getImageString(const int& index)
{
	return buttonImage_[index];
}

void Button::setButtonState(ButtonState state)
{
	state_ = state;
}