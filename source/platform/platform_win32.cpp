internal LRESULT CALLBACK win32_window_proc(HWND handle, UINT messages, WPARAM wparam,
                                            LPARAM lparam)
{

 switch (messages) {
 case WM_DESTROY:
 Win32Running = false;
 break;
 case WM_PAINT: {
 } break;
 case WM_SYSKEYDOWN:
 case WM_SYSKEYUP:
 case WM_KEYDOWN:
 case WM_KEYUP: {
 // TODO: ...
 } break;
 }

 return DefWindowProcA(handle, messages, wparam, lparam);
 }

 internal HWND win32_create_window(HINSTANCE instance, int height, int width, int display_x,
 int display_y, const char *window_name)
 {
 Win32Running          = true;
 WNDCLASS window_class = {0};

 // TODO(nasr): check if CS_OWNDC CS_HREDRAW CS_VEREDRAW still matter
 window_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
 window_class.lpfnWndProc   = win32_window_proc;
 window_class.hInstance     = instance;
 window_class.lpszClassName = window_name;

 HWND window_handle = {};
 if (RegisterClass(&window_class)) {
 window_handle = CreateWindowEx(0, window_class.lpszClassName, window_name,
 WS_OVERLAPPEDWINDOW | WS_VISIBLE, height, width, display_x,
 display_y, 0, 0, instance, 0);
 }

 Assert(window_handle);
 ShowWindow(window_handle, true);

 MSG message = {};
 while (Win32Running) {
 Log("status : %d", Win32Running);
 b32 result = GetMessage(&message, 0, 0, 0);
 if (result > 0) {
 TranslateMessage(&message);
 DispatchMessage(&message);
 } else {
 break;
 }
 } // cleanup get's handled by the OS, so there is no need worry about that

 return window_handle;
}

