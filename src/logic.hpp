#pragma once
#include <cstdint>
#include <string>
namespace qol {
inline std::uint16_t pad_buttons(const std::uint8_t* report) {
 return static_cast<std::uint16_t>((~report[0])&0xffu)|static_cast<std::uint16_t>(((~report[1])&0xffu)<<8);
}
inline bool combo_pressed(std::uint16_t now,std::uint16_t before,std::uint16_t mask){ return (now&mask)==mask && (before&mask)!=mask; }
inline std::string summary(int total,int gold,int info,int skill){
 return "Gold "+std::to_string(total)+"/40 | Here G:"+std::to_string(gold)+" I:"+std::to_string(info)+" S:"+std::to_string(skill);
}
}
