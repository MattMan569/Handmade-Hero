/*	File: win32_handmadehero.cpp
 *	Created: 25th September, 2018
 *	Revised: 25th September, 2018
 */

#include <Windows.h>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	MessageBox(
		NULL,
		"This is Handmade Hero.",
		"Handmade Hero",
		MB_OK | MB_ICONINFORMATION);

	return (0);
}
