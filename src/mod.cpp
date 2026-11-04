#include <cstdint>
#include <string>
#include "openrac/mod.hpp"
#include "openrac/games/rac1.hpp"
#include "logic.hpp"

namespace {
openrac::Mod* g=nullptr;
openrac::UiPage page;
std::uint16_t previous=0;
bool open_requested=false;

void counts(int& gold,int& info,int& skill){
    gold=info=skill=0;
    for(const auto& m:g->moby_list()){
        if((m.flags&OPENRAC_MOBY_DEAD)!=0) continue;
        if(m.oclass==1134) ++gold;
        else if(m.oclass==750) ++info;
        else if(m.oclass==1559) ++skill;
    }
}
}

void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_world()) throw openrac::Error("Collectible Tracker requires RAC1 interface 1.9+");
    g=&mod;
    mod.hook<openrac::rac1::GoldBolts_CountCollected>([](auto original){ return original(); });

    page=mod.ui_page([&mod](const openrac::UiPage& p,const openrac::UiEvent&){
        int gold=0,info=0,skill=0;
        counts(gold,info,skill);
        auto fn=mod.original<openrac::rac1::GoldBolts_CountCollected>();
        const int total=fn!=nullptr ? fn() : 0;
        const auto status=mod.play_status();

        openrac::UiContent c;
        c.title="Collectibles";
        c.items={
            openrac::UiItem::text(status.level>=0 ? "Planet: "+mod.level_name(status.level) : "Planet: -"),
            openrac::UiItem::text("Gold Bolts: "+std::to_string(total)+"/40"),
            openrac::UiItem::text("Remaining here - Gold: "+std::to_string(gold)),
            openrac::UiItem::text("Remaining here - Infobots: "+std::to_string(info)),
            openrac::UiItem::text("Skill triggers here: "+std::to_string(skill))
        };
        c.footer={"L2 + R2 + Triangle opens this page"};
        p.set(c);
    });

    mod.on_pad([](int port,std::uint8_t* report,int size){
        if(port!=0 || report==nullptr || size<2) return;
        const auto now=qol::pad_buttons(report);
        const auto mask=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_TRIANGLE);
        if(qol::combo_pressed(now,previous,mask)) open_requested=true;
        if((now&mask)==mask) qol::consume_buttons(report,mask);
        previous=now;
    });

    mod.on_frame([]{
        if(open_requested){
            open_requested=false;
            page.open();
        }
    });
}
OPENRAC_MOD_CPP("collectible-tracker","0.2.0",load)
