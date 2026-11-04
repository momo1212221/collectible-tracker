#include <cassert>
#include "logic.hpp"
int main(){ const auto s=qol::summary(12,2,1,0); assert(s.find("12/40")!=std::string::npos); }
