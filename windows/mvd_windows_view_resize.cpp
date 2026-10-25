#include "mvd_windows_view_resize.h"

#include <commctrl.h>

#include <set>

#pragma comment(lib, "comctl32.lib")

namespace {

constexpr UINT_PTR kSubclassId = 1;

int g_dart_call_depth = 0;
std::set<HWND> g_deferred_views;

UINT DeferredResizeMessage() {
  static const UINT message =
      RegisterWindowMessageW(L"MultiViewDesktopDeferredViewResize");
  return message;
}

// Sizes `view` to its parent's client area. A view already at that size
// still had a resize requested, such as the 1px nudge that makes it pick up
// a DPI change, so it is nudged.
void ApplyDeferredResize(HWND view) {
  HWND host = GetParent(view);
  RECT client{};
  RECT current{};
  if (!host || !GetClientRect(host, &client) || !GetWindowRect(view, &current)) {
    return;
  }
  const int width = client.right - client.left;
  const int height = client.bottom - client.top;
  if (width <= 0 || height <= 0) {
    return;
  }
  const UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
  if (current.right - current.left == width &&
      current.bottom - current.top == height) {
    SetWindowPos(view, nullptr, client.left, client.top, width + 1, height,
                 flags);
  }
  SetWindowPos(view, nullptr, client.left, client.top, width, height, flags);
}

LRESULT CALLBACK ViewSubclassProc(HWND hwnd, UINT message, WPARAM wparam,
                                  LPARAM lparam, UINT_PTR, DWORD_PTR) {
  if (message == DeferredResizeMessage()) {
    // A modal loop inside a call from Dart can dispatch this; the call's
    // scope posts it again when it ends.
    if (g_dart_call_depth > 0) {
      g_deferred_views.insert(hwnd);
    } else {
      ApplyDeferredResize(hwnd);
    }
    return 0;
  }

  if (message == WM_WINDOWPOSCHANGING && g_dart_call_depth > 0) {
    auto* pos = reinterpret_cast<WINDOWPOS*>(lparam);
    RECT current{};
    if (!(pos->flags & SWP_NOSIZE) && GetWindowRect(hwnd, &current) &&
        (pos->cx != current.right - current.left ||
         pos->cy != current.bottom - current.top)) {
      pos->flags |= SWP_NOSIZE;
      g_deferred_views.insert(hwnd);
    }
  } else if (message == WM_NCDESTROY) {
    g_deferred_views.erase(hwnd);
    RemoveWindowSubclass(hwnd, ViewSubclassProc, kSubclassId);
  }

  return DefSubclassProc(hwnd, message, wparam, lparam);
}

}  // namespace

namespace multi_view_desktop {

MvdDartCallScope::MvdDartCallScope() { ++g_dart_call_depth; }

MvdDartCallScope::~MvdDartCallScope() {
  if (--g_dart_call_depth > 0) {
    return;
  }
  for (HWND view : g_deferred_views) {
    PostMessageW(view, DeferredResizeMessage(), 0, 0);
  }
  g_deferred_views.clear();
}

void MvdWindowsDeferViewResizesInDartCalls(HWND view) {
  if (view) {
    SetWindowSubclass(view, ViewSubclassProc, kSubclassId, 0);
  }
}

}  // namespace multi_view_desktop
