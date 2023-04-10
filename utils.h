#pragma once

class utils
{
public:
	static void debug_log(const char* format, ...)
	{
#ifdef DEBUG
		va_list args;
		va_start(args, format);
		printf("[whiterose] ");
		vprintf(format, args);
		printf("\n");
		va_end(args);
#endif
	}

	static inline float pigreek = 3.14159265358979323846264338327950288419716939937510;

	static void log(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		printf("[whiterose] ");
		vprintf(format, args);
		printf("\n");
		va_end(args);
	}

	static HWND get_gdi_window(int dwProcessID)
	{
		HWND hCurWnd = nullptr;
		while (true)
		{
			hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
			DWORD checkProcessID;
			GetWindowThreadProcessId(hCurWnd, &checkProcessID);
			if (checkProcessID == dwProcessID)
			{
				char className[126];
				GetClassNameA(hCurWnd, className, 126);
				if (strstr(className, "GDI+ Hook Window Class"))
					return hCurWnd;
			}
		}
	}
};