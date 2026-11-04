#include <array>
#include <cassert>
#include <cstdint>
#include "logic.hpp"

int main() {
    std::array<std::uint8_t,2> report{0xff,0xff};
    assert(qol::pad_buttons(report.data()) == 0);
    assert(qol::combo_pressed(0x1300u,0,0x1300u));
    assert(!qol::combo_pressed(0x1300u,0x1300u,0x1300u));

    report = {0xff,0xec};
    qol::consume_buttons(report.data(),0x1300u);
    assert(report[0] == 0xff);
    assert(report[1] == 0xff);
}
