#ifdef NDEBUG
#undef NDEBUG
#endif
#include "pad_pc_profile.h"
#include <cassert>
#include <cmath>
#include <cstring>

static PadPcOutput eval(const PadPcConfig& c, const unsigned char (&keys)[256],
                        float dx = 0, float dy = 0, float seconds = 1.0f / 60.0f) {
    PadPcOutput o{};
    pad_pc_evaluate(&c, keys, dx, dy, seconds, &o);
    return o;
}

int main() {
    PadPcConfig c{};
    pad_pc_defaults(&c);
    assert(c.move_keys[0][0] == 'W' && c.move_keys[1][0] == 'S');
    assert(c.move_keys[2][0] == 'A' && c.move_keys[3][0] == 'D');
    assert(c.sensitivity == 0.08f && c.invert_y == 0 && c.mouse_enabled == 1);
    assert(c.capture_key == 0x70);
    unsigned char keys[256]{};
    auto press = [&](int key) { std::memset(keys, 0, sizeof keys); keys[key] = 1; return eval(c, keys); };
    assert(press('W').ly == 1 && press('W').buttons == 0);
    assert(press('A').lx == 1 && press('A').buttons == 0);
    const int vk[] = {0x26,0x28,0x25,0x27,0x20,1,2,'E','Q','R',0x10,0x11,0x1b,0x09,0,0};
    const u16 bits[] = {CELL_PAD_CTRL_UP,CELL_PAD_CTRL_DOWN,CELL_PAD_CTRL_LEFT,CELL_PAD_CTRL_RIGHT,
        CELL_PAD_CTRL_CROSS,CELL_PAD_CTRL_CIRCLE,CELL_PAD_CTRL_SQUARE,CELL_PAD_CTRL_TRIANGLE,
        CELL_PAD_CTRL_L1,CELL_PAD_CTRL_R1,CELL_PAD_CTRL_L2,CELL_PAD_CTRL_R2,
        CELL_PAD_CTRL_START,CELL_PAD_CTRL_SELECT,CELL_PAD_CTRL_L3,CELL_PAD_CTRL_R3};
    for (int i = 0; i < PAD_PC_ACTION_COUNT - 2; ++i) {
        auto o = press(vk[i]);
        assert(o.buttons == bits[i]);
        assert(o.lx == 128 && o.ly == 128);
        assert(o.l2 == (i == PAD_PC_L2 ? 255 : 0));
        assert(o.r2 == (i == PAD_PC_R2 ? 255 : 0));
    }
    assert(press(0x0d).buttons == CELL_PAD_CTRL_START);
    keys['W'] = keys['S'] = keys['A'] = keys['D'] = 1;
    auto o = eval(c, keys); assert(o.lx == 128 && o.ly == 128);
    std::memset(keys, 0, sizeof keys); keys['W'] = keys['D'] = 1;
    o = eval(c, keys);
    assert(o.lx > 128 && o.ly < 128);
    assert(std::abs((int)o.lx - 128) <= 90 && std::abs((int)o.ly - 128) <= 90);
    std::memset(keys, 0, sizeof keys);
    o = eval(c, keys); assert(o.rx == 128 && o.ry == 128);
    o = eval(c, keys, 8, 4); assert(o.rx > 128 && o.ry > 128);
    o = eval(c, keys, -8, -4); assert(o.rx < 128 && o.ry < 128);
    c.invert_y = 1; assert(eval(c, keys, 0, 4).ry < 128); c.invert_y = 0;
    assert(eval(c, keys, 100000, -100000).rx == 255);
    assert(eval(c, keys, 100000, -100000).ry == 1);
    assert(eval(c, keys, 1, 1, 0).rx == 128);
    assert(eval(c, keys, 1, 1, NAN).ry == 128);
    assert(eval(c, keys, 8, 0, 1.0f/60).rx == eval(c, keys, 16, 0, 1.0f/30).rx);
    c.mouse_enabled = 0; assert(eval(c, keys, 50, 50).rx == 128); c.mouse_enabled = 1;
    assert(pad_pc_key_from_name("f12") == 0x7b);
    assert(pad_pc_key_from_name("mouse5") == 6);
    assert(pad_pc_key_from_name("none") == 0);
    assert(pad_pc_key_from_name("F13") == -1);
    int binding[2] = {9, 9};
    assert(pad_pc_parse_binding("space, enter", binding) == 1);
    assert(binding[0] == 0x20 && binding[1] == 0x0d);
    assert(pad_pc_parse_binding("q", binding) == 1 && binding[0] == 'Q' && binding[1] == 0);
    assert(pad_pc_parse_binding("Q,BOGUS", binding) == 0 && binding[0] == 'Q' && binding[1] == 0);
    assert(pad_pc_parse_binding("Q,R,E", binding) == 0);
    assert(pad_pc_parse_binding("", binding) == 0);
    c.move_keys[0][0] = 'I'; c.move_keys[0][1] = 0;
    c.button_keys[PAD_PC_CROSS][0] = 'Z'; c.button_keys[PAD_PC_CROSS][1] = 0;
    assert(press('I').ly == 1);
    assert(press('Z').buttons == CELL_PAD_CTRL_CROSS);
    return 0;
}
