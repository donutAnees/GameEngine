#pragma once

#include <Windows.h>
#include <iostream>
#include <thread>
#include "geometry.h"

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

/*
•	PIXEL_SOLID (0x2588): Represents a solid block character (█).
•	PIXEL_THREEQUARTERS (0x2593): Represents a three-quarters block character (▓).
•	PIXEL_HALF (0x2592): Represents a half block character (▒).
•	PIXEL_QUARTER (0x2591): Represents a quarter block character (░).
*/
enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class console {
private:
	short m_screenWidth;
	short m_screenHeight;
	// Reference to the console output screen buffer
	HANDLE m_hConsole;
	// Reference to the console input buffer (Keyboard, mouse...)
	HANDLE m_hConsoleIn;
	CHAR_INFO* m_screenBuffer;
	std::wstring m_appName;
	// To select a window size
	SMALL_RECT m_windowCoord;
public:
	console() {
		// height and width are in characters
		m_screenHeight = 100;
		m_screenWidth = 100;
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		m_appName = L"GameEngine";
		m_windowCoord = { 0, 0, 1, 1 };

		try {

			// Set the window size to the minimum
			SetConsoleWindowInfo(m_hConsole, TRUE, &m_windowCoord);

			// Set the screen buffer size to the requested size
			COORD coord = { m_screenWidth, m_screenHeight };
			SetConsoleScreenBufferSize(m_hConsole, coord);

			// Assign the screen buffer to the console
			SetConsoleActiveScreenBuffer(m_hConsole);

			// Set the console size back to the requested size
			m_windowCoord = { 0, 0, static_cast<short>(m_screenWidth - 1), static_cast<short>(m_screenHeight - 1) };
			SetConsoleWindowInfo(m_hConsole, TRUE, &m_windowCoord);

			// Allocate memory for the screen buffer
			m_screenBuffer = new CHAR_INFO[m_screenWidth * m_screenHeight];
			memset(m_screenBuffer, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}

	console(short int width, short int height, short int fontWidth, short int fontHeight) {
		m_screenHeight = height;
		m_screenWidth = width;
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
		m_appName = L"GameEngine";
		m_screenBuffer = nullptr;

		try {

			// Shrink the console window to a minimum size to avoid flickering 
			// and ensure a controlled setup
			m_windowCoord = { 0, 0, 1, 1 };

			// Set the window size to the minimum
			if(!SetConsoleWindowInfo(m_hConsole, TRUE, &m_windowCoord))
				throw std::runtime_error("SetConsoleWindowInfo failed");

			// Set the screen buffer size to the requested size
			COORD coord = { m_screenWidth, m_screenHeight };

			// Set the screen buffer size to the requested size
			if(!SetConsoleScreenBufferSize(m_hConsole, coord))
				throw std::runtime_error("SetConsoleScreenBufferSize failed");

			// Assign the screen buffer to the console
			if(!SetConsoleActiveScreenBuffer(m_hConsole))
				throw std::runtime_error("SetConsoleActiveScreenBuffer failed");

			// Set the font size
			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = fontWidth;
			cfi.dwFontSize.Y = fontHeight;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;
			wcscpy_s(cfi.FaceName, L"Consolas");
			if(!SetCurrentConsoleFontEx(m_hConsole, FALSE, &cfi))
				throw std::runtime_error("SetCurrentConsoleFontEx failed");

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
				throw std::runtime_error("GetConsoleScreenBufferInfo failed");
			if (m_screenWidth > csbi.dwMaximumWindowSize.X || m_screenHeight > csbi.dwMaximumWindowSize.Y)
				throw std::runtime_error("Screen size is too large");

			// Set the console size back to the requested size
			m_windowCoord = { 0, 0, static_cast<short>(m_screenWidth - 1), static_cast<short>(m_screenHeight - 1) };
			if(!SetConsoleWindowInfo(m_hConsole, TRUE, &m_windowCoord))
				throw std::runtime_error("SetConsoleWindowInfo failed");

			// Allocate memory for the screen buffer
			m_screenBuffer = new CHAR_INFO[m_screenWidth * m_screenHeight];
			memset(m_screenBuffer, 0, sizeof(CHAR_INFO) * m_screenWidth * m_screenHeight);

			SetConsoleTitle(m_appName.c_str());

		}
		catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
		}
	}

	~console() {
		delete[] m_screenBuffer;
	}

	void draw(int x, int y, short c = PIXEL_SOLID, short color = FG_WHITE) {
		if (x >= 0 && x < m_screenWidth && y >= 0 && y < m_screenHeight) {
			m_screenBuffer[y * m_screenWidth + x].Char.UnicodeChar = c;
			m_screenBuffer[y * m_screenWidth + x].Attributes = color;
		}
	}

	void drawLine(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short color = FG_WHITE) {
		// Uses DDAs algorithm to draw a line between two points
		// How this works is that it calculates the slope of the line and then
		// increments the x and y values by the slope until it reaches the end point
		// Basically, when we move one pixel in the x direction, we move the slope in the y direction
		int dx = x2 - x1;
		int dy = y2 - y1;
		// Steps is the number of pixels we need to draw, which is the maximum of the absolute value of dx and dy
		// This is because we need to draw the longest distance
		int steps = (abs(dx) > abs(dy) ? abs(dx) : abs(dy));
		// Calculate the increment in x and y direction
		float Xinc = dx / (float)steps;
		float Yinc = dy / (float)steps;
		// Start drawing from the first point
		float X = static_cast<float>(x1);
		float Y = static_cast<float>(y1);
		for (int i = 0; i <= steps; i++) {
			draw(static_cast<int>(X), static_cast<int>(Y), c, color);
			X += Xinc;
			Y += Yinc;
		}
	}

	void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		drawLine(x1, y1, x2, y2, c, col);
		drawLine(x2, y2, x3, y3, c, col);
		drawLine(x3, y3, x1, y1, c, col);
	}

	// To render the screen buffer to the console
	void render() {
		// Write the screen buffer to the console output
		WriteConsoleOutput(m_hConsole, m_screenBuffer, { m_screenWidth, m_screenHeight }, { 0, 0 }, &m_windowCoord);
	}

	void fill(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short color = FG_WHITE) {
		for (int x = x1; x <= x2; x++) {
			for (int y = y1; y <= y2; y++) {
				draw(x, y, c, color);
			}
		}
	}
};

class camera {
public:
	// This is the vector in world space that points to the camera's position.
	vec3 m_pos;
	// This is the vector in world space the camera is looking at, initially it is the origin (0,0,0)
	vec3 m_target;
	// This is the vector which gives the z direction of the camera, this is derived from the target and position
	// Note that m_forward points to the reverse direction to where the camera is looking at
	vec3 m_forward;
	// This is the vector which gives the x direction of the camera, this is derived from the cross product 
	// of up vector which points upwards in world space (0,1,0) - up, which gives us x since it is perpendicular to up and z
	vec3 m_right;
	// This is the vector which gives the y direction of the camera, this is derived from the cross product of x and z
	vec3 m_up;
	// The speed at which the camera moves
	float m_speed;
	camera() {
		m_speed = 0.5f;
		m_pos = { 0.0, 0.0, 3.0 };
		m_target = { 0.0 , 0.0 , 0.0 };
		updateCameraFields();
	}
	void setSpeed(float speed) {
		m_speed = speed;
	}
	void updateCameraFields() {
		m_forward = vec3_sub(m_target, m_pos);
		normalize(m_forward);
		vec3 up = { 0.0, 1.0, 0.0 };
		m_right = cross_product(up, m_forward);
		normalize(m_right);

		m_up = cross_product(m_forward, m_right);
	}
	void updateCameraForward() {
		m_pos = vec3_add(m_pos, vec3_mul(m_forward, m_speed));
		updateCameraFields();
	}
	void updateCameraBackward() {
		m_pos = vec3_sub(m_pos, vec3_mul(m_forward, m_speed));
		updateCameraFields();
	}
	void updateCameraRight() {
		vec3 cp = cross_product(m_forward, m_up);
		normalize(cp);
		m_pos = vec3_add(m_pos, vec3_mul(cp, m_speed));
		updateCameraFields();
	}
	void updateCameraLeft() {
		vec3 cp = cross_product(m_forward, m_up);
		normalize(cp);
		m_pos = vec3_sub(m_pos, vec3_mul(cp, m_speed));
		updateCameraFields();
	}
};

class engine {
private:
	static std::atomic<bool> m_engineActive;
	void engineMainThread() {
		while (m_engineActive) {
			updateFrame();
			m_console.render();
		}
	}
protected:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
public:
	console m_console;
	camera m_camera;
	engine() : m_console(), m_camera() {
	}

	engine(short int width, short int height, short int fontWidth, short int fontHeight) : m_console(width, height, fontWidth, fontHeight), m_camera() {
	}

	void start() {
		m_engineActive = true;
		std::thread t = std::thread(&engine::engineMainThread,this);
		t.join();
	}

	virtual void updateFrame() = 0;
};

std::atomic<bool> engine::m_engineActive = false;
