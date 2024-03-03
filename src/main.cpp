#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/CCScene.hpp>
#include "Switcher.hpp"
#include "Keybinds.hpp"
#include "CustomSettings.hpp"

using namespace geode::prelude;

$execute{
	using namespace keybinds;

	BindManager::get()->registerBindable({
		"open-scene-switcher-key"_spr,
		"Open S.S",
		"Key/key combination that opens the Scene Switcher",
		{ Keybind::create(KEY_CapsLock, Modifier::Control) },
		Category::GLOBAL
	});
}

class $modify(LevelInfoLayer) {
	void destructor() {
		setLastViewedLevel(this->m_level, kLastLevelTypePlay);
		LevelInfoLayer::~LevelInfoLayer();
	}
};

class $modify(EditLevelLayer) {
	void destructor() {
		setLastViewedLevel(this->m_level, kLastLevelTypePlay);
		EditLevelLayer::~EditLevelLayer();
	}

};

class $modify(LevelSelectLayer) {

	void destructor() {
		auto pagSafe = ((from<int>(from<BoomScrollLayer*>(this, 0x150), 0x1cc) % 24 + 24) % 24) + 1;

		setLastViewedLevel(
			GameLevelManager::sharedState()->getMainLevel(pagSafe
				, true
			),
			kLastLevelTypeMain
		);
		LevelSelectLayer::~LevelSelectLayer();
	}
};

class $modify(CCScene) {
	static CCScene* create() {
		auto ret = CCScene::create();

		ret->template addEventListener<keybinds::InvokeBindFilter>([=](keybinds::InvokeBindEvent* event) {
			if (!CCDirector::sharedDirector()->getIsTransitioning()) {
				if (event->isDown()) {
					Switcher::show();
				}
				else {
					Switcher::goTo();
				}
			}	
			return ListenerResult::Propagate;
		}, "open-scene-switcher-key"_spr);

		return ret;
	}

};

$on_mod(Loaded) {
	Mod::get()->addCustomSetting<SettingSectionValue>("title-seccion-layers-selection", "none");
}