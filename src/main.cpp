#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include "Switcher.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	void destructor_() {
		setLastViewedLevel(this->m_level, kLastLevelTypePlay);
		LevelInfoLayer::destructor_();
	}
};

class $modify(EditLevelLayer) {
	void destructor_() {
		setLastViewedLevel(this->m_level, kLastLevelTypePlay);
		EditLevelLayer::destructor_();
	}
};

class $modify(LevelSelectLayer) {
	void destructor_() {

		auto pagSafe = ((from<int>(from<BoomScrollLayer*>(this, 0x150), 0x1cc) % 24 + 24) % 24) + 1;

		setLastViewedLevel(
			GameLevelManager::sharedState()->getMainLevel(pagSafe
				, true
			),
			kLastLevelTypeMain
		);


		LevelSelectLayer::destructor_();
	}
};

class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool idk) {
		if (
			!CCDirector::sharedDirector()->getIsTransitioning() &&
			key == switchKey()
			) {
			if (down) {
				Switcher::show();
			}
			else {
				Switcher::goTo();
			}
			return true;
		}
		else {
			if ((down) && Switcher::handleKey(key))
				return true;
		}

		

		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, idk);
	}
};

$on_mod(Loaded) {
}