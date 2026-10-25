#ifndef MVD_WINDOWS_VIEW_RESIZE_H_
#define MVD_WINDOWS_VIEW_RESIZE_H_

#include <windows.h>

namespace multi_view_desktop {

// Resizing a Flutter view's HWND makes the embedder draw a frame at the new
// size before the resize returns, running Dart on this thread. Inside a call
// from Dart that frame would nest in whatever Dart was doing, often a frame of
// its own, so view resizes requested during such a call are held back and
// applied from the message loop once the call has returned.

// Marks a call from Dart for its lifetime. Every FFI entry point that can
// resize a window opens one.
class MvdDartCallScope {
 public:
  MvdDartCallScope();
  ~MvdDartCallScope();

  MvdDartCallScope(const MvdDartCallScope&) = delete;
  MvdDartCallScope& operator=(const MvdDartCallScope&) = delete;
};

// Defers resizes of `view`, a Flutter view HWND filling its parent's client
// area, requested during a call from Dart.
void MvdWindowsDeferViewResizesInDartCalls(HWND view);

}  // namespace multi_view_desktop

#endif  // MVD_WINDOWS_VIEW_RESIZE_H_
