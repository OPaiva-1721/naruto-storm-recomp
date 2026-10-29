#ifdef NDEBUG
#undef NDEBUG
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <atomic>
static int focused=1;
static bool clipped=false;
static POINT cursor={500,400};
static unsigned char keys[256]{};
static ULONGLONG clock_ms=100;
static HWND fake_foreground() {return reinterpret_cast<HWND>(uintptr_t(focused?focused:2));}
static DWORD fake_owner(HWND,LPDWORD p) {*p=focused?GetCurrentProcessId():GetCurrentProcessId()+1;return 1;}
static BOOL fake_rect(HWND,LPRECT r) {*r=RECT{0,0,800,600};return TRUE;}
static BOOL fake_to_screen(HWND,LPPOINT p) {p->x+=100;p->y+=100;return TRUE;}
static BOOL fake_get_cursor(LPPOINT p) {*p=cursor;return TRUE;}
static BOOL fake_set_cursor(int x,int y) {cursor=POINT{x,y};return TRUE;}
static BOOL fake_clip(const RECT* r) {clipped=r!=nullptr;return TRUE;}
static SHORT fake_key(int k) {return keys[k]?SHORT(0x8000):0;}
static ULONGLONG fake_tick() {return clock_ms;}
static HCURSOR fake_cursor(HCURSOR) {return nullptr;}
#define GetForegroundWindow fake_foreground
#define GetWindowThreadProcessId fake_owner
#define GetClientRect fake_rect
#define ClientToScreen fake_to_screen
#define GetCursorPos fake_get_cursor
#define SetCursorPos fake_set_cursor
#define ClipCursor fake_clip
#define GetAsyncKeyState fake_key
#define GetTickCount64 fake_tick
#define SetCursor fake_cursor
#include "pad_pc_win32.h"
static PadPcOutput poll(bool deliver=true) {
    clock_ms+=20;PadPcOutput o;pad_pc_win_poll(&o);
    pad_pc_win_begin_report(&o);pad_pc_win_end_report(deliver);return o;
}
static void ack() {PadPcOutput o;pad_pc_win_begin_report(&o);pad_pc_win_end_report(1);}
int main(int argc,char** argv) {
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    if(argc>1&&!std::strcmp(argv[1],"--legacy")) {
        _putenv_s("PAD_PROFILE","legacy");_putenv_s("PAD_NO_KEYBOARD","");
        assert(!pad_pc_win_enabled());pad_pc_win_set_window(fake_foreground());
        auto o=poll();assert(!clipped&&!o.buttons&&o.lx==128&&o.rx==128);return 0;
    }
    char dir[MAX_PATH],file[MAX_PATH];GetTempPathA(MAX_PATH,dir);GetTempFileNameA(dir,"pcc",0,file);
    FILE* f=std::fopen(file,"wb");assert(f);
    std::fputs("[move]\nforward=I\n[buttons]\ncross=K\n[mouse]\ncapture_key=F2\nsensitivity=0.1\ninvert_y=1\n",f);std::fclose(f);
    _putenv_s("PAD_PROFILE",argc>1&&!std::strcmp(argv[1],"--default")?"":"naruto-pc");
    _putenv_s("PAD_CONFIG",file);_putenv_s("PAD_NO_KEYBOARD","");
    assert(pad_pc_win_enabled());
    pad_pc_win_set_window(fake_foreground());
    auto o=poll();assert(o.buttons==0&&o.lx==128&&o.ly==128);
    assert(clipped&&pad_pc_win_captured());
    keys['W']=1;o=poll();assert(o.ly==128);keys['W']=0;
    keys['I']=1;keys['K']=1;o=poll();assert(o.ly==1&&(o.buttons&CELL_PAD_CTRL_CROSS));
    std::memset(keys,0,sizeof(keys));cursor.x+=10;cursor.y+=5;o=poll();
    assert(o.rx==178&&o.ry==103);assert(cursor.x==500&&cursor.y==400);
    pad_pc_win_key_pressed('K');o=poll(false);assert(o.buttons&CELL_PAD_CTRL_CROSS);
    o=poll(false);assert(o.buttons&CELL_PAD_CTRL_CROSS); // polling info is not delivery
    ack();o=poll();assert(!(o.buttons&CELL_PAD_CTRL_CROSS));
    cursor.x+=10;o=poll(false);assert(o.rx>128);
    o=poll(false);assert(o.rx>128); // movement must survive an info-only poll
    ack();o=poll();assert(o.rx==128);
    pad_pc_win_key_pressed('K');pad_pc_win_begin_report(&o);assert(o.buttons&CELL_PAD_CTRL_CROSS);
    std::atomic<bool> entered{false},finished{false};
    std::thread producer([&]{entered=true;pad_pc_win_key_pressed('K');finished=true;});
    while(!entered.load())std::this_thread::yield();
    assert(!finished.load()); // a new press cannot be erased by the old report's ack
    pad_pc_win_end_report(1);producer.join();o=poll(false);assert(o.buttons&CELL_PAD_CTRL_CROSS);ack();
    cursor.x+=10;poll(false);pad_pc_win_begin_report(&o);assert(o.rx>128);
    clock_ms+=20;cursor.x+=10;entered=false;finished=false;
    std::thread info([&]{entered=true;PadPcOutput ignored;pad_pc_win_poll(&ignored);finished=true;});
    while(!entered.load())std::this_thread::yield();
    assert(!finished.load());pad_pc_win_end_report(1);info.join();
    o=poll(false);assert(o.rx>128);ack();
    focused=3;o=poll();assert(!clipped&&!o.buttons); // same PID, different dialog
    focused=1;poll();
    focused=false;o=poll();assert(!clipped&&!pad_pc_win_captured());
    assert(!o.buttons&&o.lx==128&&o.ly==128&&o.rx==128&&o.ry==128);
    focused=true;keys[VK_LBUTTON]=1;poll();o=poll();assert(!(o.buttons&CELL_PAD_CTRL_CIRCLE));
    keys[VK_LBUTTON]=0;poll();keys[VK_LBUTTON]=1;o=poll();assert(o.buttons&CELL_PAD_CTRL_CIRCLE);
    keys[VK_LBUTTON]=0;keys[VK_ESCAPE]=1;o=poll();assert(o.buttons&CELL_PAD_CTRL_START);
    assert(!clipped);keys[VK_ESCAPE]=0;poll();
    keys[VK_F2]=1;poll();assert(clipped);poll();assert(clipped);
    keys[VK_F2]=0;poll();keys[VK_F2]=1;poll();assert(!clipped);
    keys[VK_F2]=0;poll();keys[VK_F2]=1;poll();assert(clipped);
    pad_pc_win_focus_lost();assert(!clipped&&!pad_pc_win_captured());
    std::remove(file);
    std::puts("PASS PC profile config, focus release, click arming, camera, pause and capture toggle (mocked Win32 input)");
}
