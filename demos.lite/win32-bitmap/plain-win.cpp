
#include "plain-win.h"

HINSTANCE ghInstance = 0;

#define WINDOW_CLASS_NAME L"sciter-plain-window"			// the main window class name

#include "../facade-resources.cpp"

bool window::init_class()
{
  static ATOM cls = 0;
  if (cls)
    return true;

  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = wnd_proc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = ghInstance;
  wcex.hIcon = NULL;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = 0;//MAKEINTRESOURCE(IDC_PLAINWIN);
  wcex.lpszClassName = WINDOW_CLASS_NAME;
  wcex.hIconSm = NULL;

  cls = RegisterClassEx(&wcex);

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "facade-resources.cpp") with the archive

  return cls != 0;
}

LRESULT window::handle_notification(LPSCITER_CALLBACK_NOTIFICATION pnm)
{
  switch (pnm->code)
  {
  case SC_INVALIDATE_RECT:
    InvalidateRect(hwnd, &((SCN_INVALIDATE_RECT*)pnm)->invalidRect,FALSE);
    return 0;
  }
  return __super::handle_notification(pnm);
}

LRESULT window::on_load_data(LPSCN_LOAD_DATA nm)
{
  sciter::debug_output::instance()->printf("%ws\n", nm->uri);

  return __super::on_load_data(nm);
}

bool window::handle_scripting_call(HELEMENT he, SCRIPTING_METHOD_PARAMS& params)
{
  sciter::debug_output::instance()->printf("he %p call '%s'\n", he, params.name);
  return __super::handle_scripting_call(he, params);
}

window::window()
  : hwnd()
  , loaded()
  , memDc()
  , memBm()
  , memSize()
  , mouseEnter()
{
  init_class();

  hwnd = CreateWindow(WINDOW_CLASS_NAME, L"demo", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 1024, 768, NULL, NULL, ghInstance, this);
  SetWindowLongPtr(hwnd, GWLP_USERDATA, LONG_PTR(this));

  if (!hwnd)
    return;

  init();

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);
}

window* window::ptr(HWND hwnd)
{
  return reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

bool window::init()
{
  auto version = SAPI()->version;

  // create an engine instance
  auto ok = SciterProcX(get_hwnd(), SCITER_X_MSG_CREATE(GFX_LAYER_SKIA, FALSE));

  // setup callbacks
  SciterSetOption(get_hwnd(), SCITER_SET_DEBUG_MODE, true);
  this->setup_callback();
  sciter::attach_dom_event_handler(get_hwnd(), this);

  sciter::debug_output::instance()->printf("initialized\n");
  // load file
  ok = this->load_file(WSTR("this://app/main.htm"));
  this->loaded = true;
  SetWindowTextA(this->hwnd, "Loaded");
  sciter::debug_output::instance()->printf("loaded\n");

  return true;
}


void window::invalidate_window()
{
  ::InvalidateRect(this->hwnd, NULL, FALSE);
}

LRESULT CALLBACK window::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  window* self = ptr(hWnd);

  switch (message)
  {
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  case WM_LBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_MOUSEMOVE:
  case WM_MOUSELEAVE:
  {

    unsigned mb = 0;

    switch (message) {
      case WM_LBUTTONDOWN:  case WM_LBUTTONUP:  case WM_LBUTTONDBLCLK:  mb |= MOUSE_BUTTONS::MAIN_MOUSE_BUTTON; break;
      case WM_RBUTTONDOWN:  case WM_RBUTTONUP:  case WM_RBUTTONDBLCLK:  mb |= MOUSE_BUTTONS::PROP_MOUSE_BUTTON; break;
      case WM_MBUTTONDOWN:  case WM_MBUTTONUP:  case WM_MBUTTONDBLCLK:  mb |= MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON; break;
      case WM_MOUSEMOVE: case WM_MOUSELEAVE:
        if (wParam & MK_LBUTTON)
          mb |= MOUSE_BUTTONS::MAIN_MOUSE_BUTTON;
        if (wParam & MK_RBUTTON)
          mb |= MOUSE_BUTTONS::PROP_MOUSE_BUTTON;
        if (wParam & MK_MBUTTON)
          mb |= MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON;
        break;
    }

    unsigned ks = 0;
    if (wParam & MK_SHIFT)
      ks |= KEYBOARD_STATES::SHIFT_KEY_PRESSED;
    if (wParam & MK_CONTROL)
      ks |= KEYBOARD_STATES::CONTROL_KEY_PRESSED;
    if (wParam & MK_ALT)
      ks |= KEYBOARD_STATES::ALT_KEY_PRESSED;


    MOUSE_EVENTS evt = MOUSE_EVENTS::MOUSE_MOVE;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      evt = MOUSE_EVENTS::MOUSE_DOWN;
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      evt = MOUSE_EVENTS::MOUSE_UP;
      break;

    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
      evt = MOUSE_EVENTS::MOUSE_DCLICK;
      break;

    case WM_MOUSEMOVE:
      evt = MOUSE_EVENTS::MOUSE_MOVE;
      if (!self->mouseEnter) {
        evt = MOUSE_EVENTS::MOUSE_ENTER;

        TRACKMOUSEEVENT me = { sizeof(TRACKMOUSEEVENT) };
        me.dwFlags = TME_LEAVE;
        me.hwndTrack = hWnd;
        TrackMouseEvent(&me);

        self->mouseEnter = true;
      }
      break;

    case WM_MOUSELEAVE:
      evt = MOUSE_EVENTS::MOUSE_LEAVE;
      self->mouseEnter = false;
      break;
    }

    POINT pos = { LOWORD(lParam), HIWORD(lParam) };

    auto ok = SciterProcX(self, SCITER_X_MSG_MOUSE(evt, MOUSE_BUTTONS(mb), KEYBOARD_STATES(ks), pos));
    if (!ok)
      ok = 0;

    break;
  }

  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_CHAR:
  {
    unsigned ks = 0;
    if (GetKeyState(VK_CONTROL) < 0)
      ks |= KEYBOARD_STATES::CONTROL_KEY_PRESSED;
    if (GetKeyState(VK_SHIFT) < 0)
      ks |= KEYBOARD_STATES::SHIFT_KEY_PRESSED;
    if (GetKeyState(VK_MENU))
      ks |= KEYBOARD_STATES::ALT_KEY_PRESSED;

    KEY_EVENTS evt;
    unsigned code;
    switch (message)
    {
    case WM_KEYDOWN: evt = KEY_EVENTS::KEY_DOWN; code = translate_key(wParam);  break;
    case WM_KEYUP: evt = KEY_EVENTS::KEY_UP; code = translate_key(wParam);  break;
    case WM_CHAR: evt = KEY_EVENTS::KEY_CHAR; code = wParam;  break;
    }
    auto ok = SciterProcX(self, SCITER_X_MSG_KEY(evt, code, KEYBOARD_STATES(ks)));
    if (!ok)
      ok = 0;
    break;
  }
  case WM_DESTROY:
  {
    SciterProcX(self, SCITER_X_MSG_DESTROY());
    PostQuitMessage(0);
    break;
  }
  case WM_SIZE:
  {
    auto width = LOWORD(lParam);
    auto height = HIWORD(lParam);
    auto ok = SciterProcX(self, SCITER_X_MSG_SIZE(width, height));
    break;
  }
  case WM_SETFOCUS:
  case WM_KILLFOCUS:
  {
    auto ok = SciterProcX(self, SCITER_X_MSG_FOCUS(message == WM_SETFOCUS));
    if (!ok)
      ok = 0;
    self->invalidate_window();
    break;
  }
  case WM_ERASEBKGND:
  {
    return 1;
  }

  case WM_PAINT:
  {
    if (!self->loaded)
      break;

    static int pcnt = 0;
    sciter::debug_output::instance()->printf("print %d\n",++pcnt);

    auto on_bitmap = [](LPCBYTE rgba, INT x, INT y, UINT width, UINT height, LPVOID param)
    {
      auto* self = reinterpret_cast<window*>(param);

      PAINTSTRUCT ps = {};
      if (HDC dc = BeginPaint(self->hwnd, &ps)) {


        if (!self->memDc || self->memSize.cx != width || self->memSize.cy != height) {
          if (self->memBm)
            DeleteObject(self->memBm);

          self->memDc = CreateCompatibleDC(dc);
          self->memBm = CreateCompatibleBitmap(dc, width, height);
        }

        auto oldBm = SelectObject(self->memDc, self->memBm);

        // Sciter: pointer to BGRA bitmap
        // GDI: each DWORD represents blue, green, and red for a pixel.
        // The value for blue is in the least significant 8 bits,
        // followed by 8 bits each for green and red
        BITMAPINFO bmi = {};
        auto& info = bmi.bmiHeader;
        info.biSize = sizeof(BITMAPINFOHEADER);
        info.biWidth = width;
        info.biHeight = -int(height);
        info.biPlanes = 1;
        info.biBitCount = 32;
        info.biCompression = BI_RGB;

        auto rcWidth = ps.rcPaint.right - ps.rcPaint.left;
        auto rcHeight = ps.rcPaint.bottom - ps.rcPaint.top;
        auto ok = StretchDIBits(self->memDc, 0, 0, width, height, 0, 0, width, height, rgba, &bmi, DIB_RGB_COLORS, SRCCOPY);

        BitBlt(dc, x, y, width, height, self->memDc, 0, 0, SRCCOPY);

        SelectObject(self->memDc, oldBm);

        EndPaint(self->hwnd, &ps);
      }

    };

    SCITER_X_MSG_PAINT paint;
    paint.targetType = SPT_RECEIVER;
    paint.target.receiver.param = self;
    paint.target.receiver.callback = on_bitmap;

    auto ok = SciterProcX(self, paint);

    break;
  }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

// the WinMain

int WINAPI wWinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  PWSTR     lpCmdLine,
  int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  ghInstance = hInstance;

  SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
    ALLOW_FILE_IO |
    ALLOW_SOCKET_IO |
    ALLOW_EVAL |
    ALLOW_SYSINFO);

  SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, true);
  SciterSetOption(NULL, SCITER_SET_UX_THEMING, true);

  // Perform application initialization:

  OleInitialize(NULL); // for shell interaction: drag-n-drop, etc.

  auto* cls = SciterClassName();

  sciter::debug_output _dbg;

  window wnd;

  if (!wnd.is_valid())
    return FALSE;

  // Main message loop:
  for (;;) {
    SciterProcX(wnd.get_hwnd(), SCITER_X_MSG_HEARTBIT(GetTickCount()));
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(msg.message == WM_QUIT)
        break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    Sleep(0);
  }

  OleUninitialize();

  return 0;
}

#include "sciter-x-lite-keycodes.h"

unsigned window::translate_key(unsigned vk)
{
  auto in_range = [vk](unsigned vk_start, unsigned vk_end, unsigned kb_first) -> unsigned
  {
    if (vk >= vk_start && vk <= vk_end)
      return (vk - vk_start) + kb_first;

    return 0;
  };

  if (auto kb = in_range('0', '9', KB_0))
    return kb;

  if (auto kb = in_range('A', 'Z', KB_A))
    return kb;

  if (auto kb = in_range(VK_NUMPAD0, VK_NUMPAD9, KB_NUMPAD0))
    return kb;

  if (auto kb = in_range(VK_F1, VK_F24, KB_F1))
    return kb;

  switch (vk)
  {
  case VK_ESCAPE: return KB_ESCAPE;
  case VK_BACK:   return KB_BACK;
  case VK_TAB:    return KB_TAB;
  case VK_SPACE:  return KB_SPACE;
  case VK_SHIFT:  return KB_SHIFT;
  case VK_CONTROL:return KB_CONTROL;
  case VK_MENU:   return KB_MENU;
  }
  return 0;
}
