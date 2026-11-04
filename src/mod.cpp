#include <cstdint>
#include <limits>
#include <string>
#include "openrac/mod.hpp"
#include "openrac/games/rac1.hpp"
#include "logic.hpp"

namespace {
openrac::Mod* g=nullptr;
std::uint16_t previous=0;
bool requested=false, auto_show=true;
std::uint32_t loads=std::numeric_limits<std::uint32_t>::max();
int settle=0;
int notice_frames=0;
void show(){
    int gold=0, info=0, skill=0;
    for(const auto& m:g->moby_list()){
        if((m.flags&OPENRAC_MOBY_DEAD)!=0) continue;
        if(m.oclass==1134) ++gold;
        else if(m.oclass==750) ++info;
        else if(m.oclass==1559) ++skill;
    }
    auto count=g->original<openrac::rac1::GoldBolts_CountCollected>();
    const int total=count!=nullptr ? count() : 0;
    if(g->has_notice()){ g->play_notice(qol::summary(total,gold,info,skill)); notice_frames=120; }
}
}
void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_world()) throw openrac::Error("Collectible Tracker requires RAC1 interface 1.9+");
    g=&mod; auto_show=mod.setting<bool>("show_on_level_start");
    mod.hook<openrac::rac1::GoldBolts_CountCollected>([](auto original){ return original(); });
    mod.on_pad([](int port,std::uint8_t* report,int size){
        if(port!=0 || report==nullptr || size<2) return;
        const auto now=qol::pad_buttons(report);
        const auto mask=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_TRIANGLE);
        if(qol::combo_pressed(now,previous,mask)) requested=true;
        if((now&mask)==mask) qol::consume_buttons(report,mask);
        previous=now;
    });
    mod.on_frame([]{
        const auto s=g->play_status();
        if(s.loads!=loads){ loads=s.loads; settle=0; }
        if(s.phase==OPENRAC_PLAY_PLAYING && settle<180) ++settle;
        if(auto_show && settle==120){ show(); ++settle; }
        if(requested){ requested=false; if(s.phase==OPENRAC_PLAY_PLAYING) show(); }
        if(notice_frames>0 && --notice_frames==0 && g->has_notice()) g->play_notice("");
    });
    mod.info("L2+R2+Triangle shows collectible summary");
}
OPENRAC_MOD_CPP("collectible-tracker","0.1.3",load)
