/*	File: win32_handmadehero.cpp
 *	Created: 25th September, 2018
 *	Revised: 25th September, 2018
 */

#include <Windows.h>




#define local_persist static	// Local variable that persists beyond scope lifetime
#define global_variable static	// Global variable
#define internal static			// Scope limited to this translation unit

// TODO: This is a global for now
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;




internal void
Win32ResizeDIBSection(int Width, int Height)
{
	// TODO: Bulletproof this
	// Maybe don't free first, free after, then free first if that fails.

	// Check that we have created a bitmap handle previously
	if (BitmapHandle)
	{
		DeleteObject(BitmapHandle);
	}
	
	// Create a bitmap DC if we haven't previously
	if(!BitmapDeviceContext)
	{
		// TODO: Should we recreate these under certain special circumstances?
		// Pass NULL to get a DC compatible with the current screen
		BitmapDeviceContext = CreateCompatibleDC(NULL);
	}

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	// All other fields are cleared to 0 by the static declaration

	BitmapHandle = CreateDIBSection(
		BitmapDeviceContext,
		&BitmapInfo,
		DIB_RGB_COLORS,
		&BitmapMemory,
		0, 0);
}




internal void
Win32UpdateWindow(HDC DeviceContext,
				  int X,
				  int Y,
				  int Width,
				  int Height)
{
	StretchDIBits(DeviceContext,
				  X, Y, Width, Height,
				  X, Y, Width, Height,
				  BitmapMemory,
				  &BitmapInfo,
				  DIB_RGB_COLORS,
				  SRCCOPY);
}




// Recieve messages from Windows
LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
						UINT Message,
						WPARAM WParam,
						LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
		// The window's size has changed
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");

			RECT ClientRect;

			// Get the rectangle that we are able to draw to,
			// not the whole window as with GetWindowRect, which includes the borders
			GetClientRect(Window, &ClientRect);

			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;

			// The screen must be redrawn every time the window size changes
			Win32ResizeDIBSection(Width, Height);

			break;
		}

		// The window's 'X' has been clicked
		case WM_CLOSE:
		{
			// TODO: Handle this with a message to the user?
			OutputDebugStringA("WM_CLOSE\n");
			Running = false;
			break;
		}

		// The window is now the active window
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
			break;
		}

		// The window has been deleted
		case WM_DESTROY:
		{
			// TODO: Handle this as an error - recreate window?
			OutputDebugStringA("WM_DESTROY\n");
			Running = false;
			break;
		}

		// Paint our window
		case WM_PAINT:
		{
			OutputDebugStringA("WM_PAINT\n");

			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			Win32UpdateWindow(DeviceContext, X, Y, Width, Height);

			EndPaint(Window, &Paint);

			break;
		}

		// The message was not handled
		// Will then be passed on to Windows to execute the default behaviour
		default:
		{
//			OutputDebugStringA("default\n");
			Result = DefWindowProc(Window, Message, WParam, LParam);
			break;
		}
	}

	return (Result);
}




int CALLBACK
WinMain( HINSTANCE Instance,
		 HINSTANCE PrevInstance,
		 LPSTR CommandLine,
		 int ShowCode)
{
	// Open a Windows window with all fields cleared to 0
	WNDCLASS WindowClass = {};

	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;		// Properties of the window
	WindowClass.lpfnWndProc = Win32MainWindowCallback;		// Function to handle messages from Windows
	WindowClass.hInstance = Instance;						// What instance is using the window?
//	WindowClass.hIcon = ;									// Window icon
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";	// Name needed to use created a class using this window

	// Register the window class for later use with CreateWindow
	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowEx(
			NULL,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			Instance,
			NULL);

		if (WindowHandle)
		{
			Running = true;

			// Start pulling messages from the queue so Windows
			// will start to send our window messages
			while (Running)
			{
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, NULL, NULL, NULL); // Get all messages for all of our windows
				if (MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			// TODO: Logging
		}
	}
	else
	{
		// TODO: Logging
	}

	return (0);
}
