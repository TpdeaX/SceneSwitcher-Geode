#include "Switcher.hpp"
#include "switchMacro.hpp"

// jaesus

SwitchTo g_eLastSwitch = kSwitchToMainMenu;
SwitchTo g_eCurrentScene = kSwitchToMainMenu;
GJGameLevel* g_pLastLevel = nullptr;
LastLevelType g_eLastLevelWasEdit = kLastLevelTypeMain;
enumKeyCodes g_eKey = KEY_CapsLock;
bool g_bAnimate = false;
int g_nAnimSpeed = 5;
bool g_bLoadOnlyAtStartup = false;
bool g_bHasBeenLoadedOnce = false;
int g_nArbitaryMagicTransitionNumber = 0;
float g_fTransitionSpeed = .5f;
std::vector<std::vector<SwitchTo>> g_vConfig;

CCSize Switcher::s_obItemSeparation = { 90.f, 90.f };
CCSize Switcher::s_obItemSize = { 80.f, 80.f };

static constexpr const char* g_sConfigFile = "SceneSwitcher.config";

constexpr const int SWITCHER_TAG = 0x517E5;

CCPoint g_obMousePos;
CCPoint g_obHighlightPos;

constexpr const char* SwitchToToString(SwitchTo to) {
	switch (to) {
	case kSwitchToMainMenu: return "Main Menu";
	case kSwitchToIconKit: return "Icon Kit";
	case kSwitchToOfficialLevels: return "Official Levels";
	case kSwitchToMyLevels: return "My Levels";
	case kSwitchToOnlineLevels: return "Online Levels";
	case kSwitchToSavedLevels: return "Saved Levels";
	case kSwitchToMapPacks: return "Map Packs";
	case kSwitchToSearch: return "Search";
	case kSwitchToTheChallenge: return "The Challenge";
	case kSwitchToFeatured: return "Featured";
	case kSwitchToGauntlets: return "Gauntlets";
	case kSwitchToLeaderboards: return "Leaderboards";
	case kSwitchToDaily: return "Daily";
	case kSwitchToWeekly: return "Weekly";
	case kSwitchToLast: return "Last";
	case kSwitchToLastLevel: return "Last Level";
	case kSwitchToSettings: return "Settings";
	case kSwitchToMoreOptions: return "More Options";
	case kSwitchToProfile: return "Profile";
	case kSwitchToQuests: return "Quests";
	case kSwitchToTheTower: return "The Tower";
	case kSwitchToLists: return "Lists";
	case kSwitchToPaths: return "Paths";
	default: return "Unknown";
	}
}

constexpr const char* SwitchToToSprName(SwitchTo to) {
	switch (to) {
	case kSwitchToMainMenu: return "GJ_playBtn_001.png";
	case kSwitchToIconKit: return "GJ_garageBtn_001.png";
	case kSwitchToOfficialLevels: return "GJ_playBtn2_001.png";
	case kSwitchToMyLevels: return "GJ_createBtn_001.png";
	case kSwitchToOnlineLevels: return "GJ_creatorBtn_001.png";
	case kSwitchToSearch: return "GJ_searchBtn_001.png";
	case kSwitchToSavedLevels: return "GJ_savedBtn_001.png";
	case kSwitchToMapPacks: return "GJ_mapPacksBtn_001.png";
	case kSwitchToSettings: return "GJ_optionsBtn_001.png";
	case kSwitchToMoreOptions: return "GJ_optionsBtn02_001.png";
	case kSwitchToFeatured: return "GJ_featuredBtn_001.png";
	case kSwitchToGauntlets: return "GJ_gauntletsBtn_001.png";
	case kSwitchToLeaderboards: return "GJ_highscoreBtn_001.png";
	case kSwitchToProfile: return "GJ_profileButton_001.png";
	case kSwitchToTheChallenge: return "difficulty_10_btn_001.png";
	case kSwitchToDaily: return "GJ_dailyBtn_001.png";
	case kSwitchToWeekly: return "GJ_weeklyBtn_001.png";
	case kSwitchToQuests: return "GJ_challengeBtn_001.png";
	case kSwitchToTheTower: return "theTowerDoor_001.png";
	case kSwitchToLists: return "GJ_listsBtn_001.png";
	case kSwitchToPaths: return "GJ_pathsBtn_001.png";
	default: return nullptr;
	}
}

const char* diffToSprName(GJGameLevel* lvl) {
	GJDifficulty diff = static_cast<GJDifficulty>(lvl->getAverageDifficulty());
	if (lvl->m_levelType == GJLevelType::Local) {
		diff = static_cast<GJDifficulty>(from<int>(lvl, 0x210));
	}

	std::stringstream vgfdx;
	vgfdx << lvl->m_demon << "\t" << lvl->m_demonDifficulty << "\t" << static_cast<int>(lvl->m_difficulty) << "\t" << static_cast<int>(diff);

	if (diff == GJDifficulty::Auto) {
		return "difficulty_auto_btn_001.png";
	}
	if (lvl->m_demon && lvl->m_levelType != GJLevelType::Local) {
		auto hardDemonWTF = lvl->m_demonDifficulty;
		if (hardDemonWTF == 0) {
			hardDemonWTF = 6;
		}
		else {
			hardDemonWTF += 4;
		}
		return CCString::createWithFormat(
			"difficulty_%02d_btn2_001.png",
			hardDemonWTF
		)->getCString();
	}
	return CCString::createWithFormat(
		"difficulty_%02d_btn_001.png", diff
	)->getCString();
}

void setLastViewedLevel(GJGameLevel* lvl, LastLevelType b) {
	if (g_pLastLevel) {
		g_pLastLevel->release();
	}
	g_pLastLevel = lvl;
	g_pLastLevel->retain();
	g_eLastLevelWasEdit = b;
}

enumKeyCodes switchKey() {
	return g_eKey;
}

enumKeyCodes stringToKey(std::string const& s) {
	auto key = stringToLower(s);
	switch (hash(key.c_str())) {
	case hash("tab"):                           return KEY_Tab;
	case hash("caps"): case hash("capslock"):   return KEY_CapsLock;
	case hash("esc"): case hash("escape"):      return KEY_Escape;
	case hash("alt"):                           return KEY_Alt;
	case hash("shift"):                         return KEY_Shift;
	case hash("control"): case hash("ctrl"):    return KEY_Control;
	case hash("space"):                         return KEY_Space;
	case hash("enter"):                         return KEY_Enter;
	}
	try {
		auto k = std::stoi(key);
		return static_cast<enumKeyCodes>(k);
	}
	catch (...) {}
	return KEY_CapsLock;
}

CCScene* createTransitionFromArbitaryMagicNumber(CCScene* scene, int num) {
	auto durationAux = Mod::get()->getSettingValue<double>("transition-speed-option");

	switch (num) {
	case 0: return scene;
	case 1: return cocos2d::CCTransitionFade::create(durationAux, scene);
	case 2: return cocos2d::CCTransitionCrossFade::create(durationAux, scene);
	case 3: return cocos2d::CCTransitionFadeBL::create(durationAux, scene);
	case 4: return cocos2d::CCTransitionFadeDown::create(durationAux, scene);
	case 5: return cocos2d::CCTransitionFadeTR::create(durationAux, scene);
	case 6: return cocos2d::CCTransitionFadeUp::create(durationAux, scene);
	case 7: return cocos2d::CCTransitionFlipAngular::create(durationAux, scene);
	case 8: return cocos2d::CCTransitionFlipX::create(durationAux, scene);
	case 9: return cocos2d::CCTransitionFlipY::create(durationAux, scene);
	case 10: return cocos2d::CCTransitionJumpZoom::create(durationAux, scene);
	case 11: return cocos2d::CCTransitionMoveInB::create(durationAux, scene);
	case 12: return cocos2d::CCTransitionMoveInL::create(durationAux, scene);
	case 13: return cocos2d::CCTransitionMoveInR::create(durationAux, scene);
	case 14: return cocos2d::CCTransitionMoveInT::create(durationAux, scene);
	case 15: return cocos2d::CCTransitionPageTurn::create(durationAux, scene, true);
	case 16: return cocos2d::CCTransitionPageTurn::create(durationAux, scene, false);
	case 17: return cocos2d::CCTransitionProgressHorizontal::create(durationAux, scene);
	case 18: return cocos2d::CCTransitionProgressInOut::create(durationAux, scene);
	case 19: return cocos2d::CCTransitionProgressOutIn::create(durationAux, scene);
	case 20: return cocos2d::CCTransitionProgressRadialCW::create(durationAux, scene);
	case 21: return cocos2d::CCTransitionProgressRadialCCW::create(durationAux, scene);
	case 22: return cocos2d::CCTransitionProgressVertical::create(durationAux, scene);
	case 23: return cocos2d::CCTransitionRotoZoom::create(durationAux, scene);
	case 24: return cocos2d::CCTransitionShrinkGrow::create(durationAux, scene);
	case 25: return cocos2d::CCTransitionSlideInB::create(durationAux, scene);
	case 26: return cocos2d::CCTransitionSlideInL::create(durationAux, scene);
	case 27: return cocos2d::CCTransitionSlideInR::create(durationAux, scene);
	case 28: return cocos2d::CCTransitionSlideInT::create(durationAux, scene);
	case 29: return cocos2d::CCTransitionSplitCols::create(durationAux, scene);
	case 30: return cocos2d::CCTransitionSplitRows::create(durationAux, scene);
	case 31: return cocos2d::CCTransitionTurnOffTiles::create(durationAux, scene);
	case 32: return cocos2d::CCTransitionZoomFlipAngular::create(durationAux, scene);
	case 33: return cocos2d::CCTransitionZoomFlipX::create(durationAux, scene);
	case 34: return cocos2d::CCTransitionZoomFlipY::create(durationAux, scene);
	}



	return scene;
}


bool Switch::init(const char* text, const char* sprName, SwitchTo to, float scale) {
	if (!CCNode::init())
		return false;

	this->m_eTo = to;

	this->setContentSize(Switcher::s_obItemSize);

	if (!text) {
		text = SwitchToToString(to);
	}

	auto label = CCLabelBMFont::create(text, "bigFont.fnt");
	label->setPosition(this->getContentSize().width / 2, 10.f);
	label->limitLabelWidth(this->getContentSize().width - 5.f, .5f, .1f);
	this->addChild(label);

	if (!sprName) {
		sprName = SwitchToToSprName(to);
	}

	if (sprName) {
		auto spr = CCSprite::createWithSpriteFrameName(sprName);
		if (spr) {
			spr->setPosition({
				this->getContentSize().width / 2,
				this->getContentSize().height / 2 + 5.f
				});
			limitNodeSize(spr, this->getContentSize() / 1.5f, scale, .1f);
			this->addChild(spr);
		}
	}

	return true;
}

void Switch::draw() {
	if (this->m_bHovered) {
		if (Mod::get()->getSettingValue<bool>("animation-option")) {
			if (m_nAnim + Mod::get()->getSettingValue<int64_t>("animation-speed-option") < m_nAnimTarget) {
				m_nAnim += Mod::get()->getSettingValue<int64_t>("animation-speed-option");
			}
			else {
				m_nAnim = m_nAnimTarget;
			}
		}
		else {
			m_nAnim = m_nAnimTarget;
		}
	}
	else {
		if (Mod::get()->getSettingValue<bool>("animation-option")) {
			if (m_nAnim - Mod::get()->getSettingValue<int64_t>("animation-speed-option") > 0) {
				m_nAnim -= Mod::get()->getSettingValue<int64_t>("animation-speed-option");
			}
			else {
				m_nAnim = 0;
			}
		}
		else {
			m_nAnim = 0;
		}
	}
	ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ccDrawSolidRect({ 0, 0 }, this->getContentSize(), { 1, 1, 1, m_nAnim / 255.f });

	CCNode::draw();
}

void Switch::updateHover(CCPoint const& pos) {
	this->m_bHovered = nodeIsHovered(this, pos - this->getContentSize() / 2);
}

SwitchTo Switch::getWhere() const {
	return this->m_eTo;
}

bool Switch::isHovered() const {
	return this->m_bHovered;
}

Switch* Switch::create(const char* text, const char* spr, SwitchTo to, CCPoint const& pos, CCNode* t, float scale) {
	auto ret = new Switch;

	if (ret && ret->init(text, spr, to, scale)) {
		ret->autorelease();
		auto p = pos + CCDirector::sharedDirector()->getWinSize() / 2;
		ret->setPosition(p);
		t->addChild(ret);
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}


constexpr bool Switcher::isPopupSwitchType(SwitchTo to) {
	switch (to) {
	case kSwitchToSettings:
	case kSwitchToProfile:
	case kSwitchToDaily:
	case kSwitchToWeekly:
	case kSwitchToQuests:
	case kSwitchToMoreOptions:
	case kSwitchToPaths:
		return true;
	}
	return false;
}

void Switcher::loadConfig(decltype(Switcher::m_vConfig) cfg) {

	Switcher::s_obItemSize = CCSize{ static_cast<float>(Mod::get()->getSettingValue<double>("size-buttons"))
		, static_cast<float>(Mod::get()->getSettingValue<double>("size-buttons")) };

	Switcher::s_obItemSeparation = CCSize{ static_cast<float>(Mod::get()->getSettingValue<double>("spacing-buttons"))
		, static_cast<float>(Mod::get()->getSettingValue<double>("spacing-buttons")) };

	for (auto const& s : this->m_vSwitches) {
		s->removeFromParent();
	}
	this->m_vSwitches.clear();

	this->m_vConfig = cfg;

	auto c_ix = 0;
	for (auto const& c : cfg) {
		auto h_ix = 0;
		for (auto const& h : c) {
			auto posx = s_obItemSeparation.width *
				(static_cast<float>(h_ix % c.size()) - c.size() / 2.f);
			auto posy = s_obItemSeparation.height *
				((cfg.size()) / 2.f - static_cast<float>(c_ix % cfg.size()) - 1);
			switch (h) {
			case kSwitchToLastLevel: {
				auto lastLevel = g_pLastLevel ?
					CCString::createWithFormat("Last Level (%s)", g_pLastLevel->m_levelName.c_str())->getCString() :
					"Last Level (None)";
				auto diffSpr = g_pLastLevel ?
					diffToSprName(g_pLastLevel) :
					"difficulty_00_btn_001.png";

				this->m_vSwitches.push_back(
					Switch::create(lastLevel, diffSpr, h, { posx, posy }, this)
				);
			} break;

			case kSwitchToLast: {
				const char* switchName = SwitchToToSprName(g_eLastSwitch);
				const char* spriteName = switchName ? switchName : (g_pLastLevel ? diffToSprName(g_pLastLevel) : "difficulty_00_btn_001.png");
				auto str = CCString::createWithFormat("Last Scene (%s)", SwitchToToString(g_eLastSwitch));
				this->m_vSwitches.push_back(Switch::create(str->getCString(), spriteName, h, { posx, posy }, this));
			} break;


			case kSwitchToSettings:
			case kSwitchToProfile: {
				this->m_vSwitches.push_back(
					Switch::create(
						nullptr, nullptr,
						h,
						{ posx, posy },
						this, .8f
					)
				);
			} break;

			default: {
				this->m_vSwitches.push_back(
					Switch::create(
						nullptr, nullptr,
						h,
						{ posx, posy },
						this
					)
				);
			}
			}
			h_ix++;
		}
		c_ix++;
	}
}

bool Switcher::init() {
	if (!CCLayerColor::initWithColor({ 0, 0, 0, 230 }))
		return false;

	this->setTag(SWITCHER_TAG);

	m_vConfig = g_vConfig;

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto titleLabel = CCLabelBMFont::create("Go To Layer", "bigFont.fnt");
	titleLabel->setPosition(winSize.width / 2, winSize.height - 15.f);
	titleLabel->setScale(.4f);
	this->addChild(titleLabel);

	this->addSwitchToConfig();

	return true;
}

void Switcher::addSwitchToConfig() {
	std::vector<SwitchTo> row1, row2, row3;

	std::vector<std::string> enabledNamesVariables = {
		"enabled-mainMenu",
		"enabled-iconKit",
		"enabled-officialLevels",
		"enabled-myLevels",
		"enabled-onlineLevels",
		"enabled-savedLevels",
		"enabled-mapPacks",
		"enabled-theChallenge",
		"enabled-featured",
		"enabled-gauntlets",
		"enabled-leaderboards",
		"enabled-search",
		"enabled-daily",
		"enabled-weekly",
		"enabled-last",
		"enabled-lastLevel",
		"enabled-settings",
		"enabled-moreOptions",
		"enabled-profile",
		"enabled-quests",
		"enabled-theTower",
		"enabled-lists",
		"enabled-paths"
	};

	int sum = 0;

	for (size_t i = 0; i < enabledNamesVariables.size(); i++)
	{
		if (Mod::get()->getSettingValue<bool>(enabledNamesVariables[i].c_str())) {
			sum++;
			switch (sum) {
			case 1:
				row1.push_back(static_cast<SwitchTo>(i));
				break;
			case 2:
				row2.push_back(static_cast<SwitchTo>(i));
				break;
			case 3:
				row3.push_back(static_cast<SwitchTo>(i));
				break;
			}
			if (sum == 3) {
				sum = 0;
			}
		}
	}
	this->loadConfig({
			row1,
			row2,
			row3,
		});
}

void Switcher::visit() {
	if (this->isVisible()) {
		auto pos = getMousePos();
		if (pos.x != g_obMousePos.x || pos.y != g_obMousePos.y) {
			g_obHighlightPos = pos;
			g_obMousePos = pos;
		}
		for (auto s : this->m_vSwitches) {
			s->updateHover(g_obHighlightPos);
		}
	}
	CCLayer::visit();
}

void Switcher::snap() {
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	Switch* nearest = nullptr;
	float dis = 0.f;
	CCRect borders = {
		winSize.width / 2,
		winSize.height / 2,
		.0f, .0f
	};
	for (auto const& item : this->m_vSwitches) {
		auto idis = ccpDistance(
			item->getPosition() + s_obItemSize / 2,
			g_obHighlightPos
		);

		if (item->getPositionX() < borders.origin.x)
			borders.origin.x = item->getPositionX();
		if (item->getPositionX() > borders.origin.x + borders.size.width)
			borders.size.width = item->getPositionX() - borders.origin.x;
		if (item->getPositionY() < borders.origin.y)
			borders.origin.y = item->getPositionY();
		if (item->getPositionY() > borders.origin.y + borders.size.height)
			borders.size.height = item->getPositionY() - borders.origin.y;

		if (!nearest || dis > idis) {
			nearest = item;
			dis = idis;
		}
	}
	if (nearest) {
		bool flip = !borders.containsPoint(g_obHighlightPos);
		g_obHighlightPos = nearest->getPosition() + s_obItemSize / 2;
		if (flip) {

		}
	}
}

bool Switcher::handle(enumKeyCodes key) {
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	switch (key) {
	case KEY_Left:
		g_obHighlightPos.x -= s_obItemSeparation.width;
		this->snap();
		return true;

	case KEY_Right:
		g_obHighlightPos.x += s_obItemSeparation.width;
		this->snap();
		return true;

	case KEY_Up:
		g_obHighlightPos.y += s_obItemSeparation.height;
		this->snap();
		return true;

	case KEY_Down:
		g_obHighlightPos.y -= s_obItemSeparation.height;
		this->snap();
		return true;
	}

	return false;
}

bool Switcher::handleKey(enumKeyCodes key) {
	auto scene = CCDirector::sharedDirector()->getRunningScene();
	if (scene->getChildByTag(SWITCHER_TAG)) {
		return as<Switcher*>(scene->getChildByTag(SWITCHER_TAG))->handle(key);
	}
	return false;
}

void Switcher::show() {
	auto scene = CCDirector::sharedDirector()->getRunningScene();
	if (scene->getObjType() == CCObjectType::PlayLayer ||
		scene->getObjType() == CCObjectType::LevelEditorLayer)
		return;

	auto z = scene->getHighestChildZ();

	if (scene->getChildByTag(SWITCHER_TAG)) {
		scene->getChildByTag(SWITCHER_TAG)->setVisible(true);
		scene->getChildByTag(SWITCHER_TAG)->setZOrder(z + 0x99);
	}
	else {
		scene->addChild(Switcher::create());
		scene->getChildByTag(SWITCHER_TAG)->setZOrder(z + 0x99);
	}
}

void Switcher::go() {
	Switch* target = nullptr;
	auto pos = getMousePos();
	for (auto s : this->m_vSwitches) {
		if (s->isHovered()) {
			target = s;
			break;
		}
	}
	if (target) {
		auto targetScene = target->getWhere();
		if (isPopupSwitchType(targetScene)) {
			switch (targetScene) {
			case kSwitchToProfile: {
				auto page = ProfilePage::create(
					GJAccountManager::sharedState()->m_accountID, true
				);
				page->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				page->show();
			} break;

			case kSwitchToSettings: {
				auto pl = OptionsLayer::create();
				CCDirector::sharedDirector()->getRunningScene()->addChild(
					pl, CCDirector::sharedDirector()->getRunningScene()->getHighestChildZ()
				);
				pl->showLayer(Mod::get()->getSettingValue<bool>("enabled-animation-popup"));
			} break;

			case kSwitchToMoreOptions: {
				auto l = MoreOptionsLayer::create();
				l->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				l->show();
			} break;

			case kSwitchToDaily: {
				auto page = DailyLevelPage::create(GJTimedLevelType::Daily);
				page->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				page->show();
			} break;

			case kSwitchToWeekly: {
				auto page = DailyLevelPage::create(GJTimedLevelType::Weekly);
				page->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				page->show();
			} break;

			case kSwitchToQuests: {
				auto page = ChallengesPage::create();
				page->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				page->show();
			} break;
			case kSwitchToPaths: {
				auto page = GJPathsLayer::create();
				page->m_noElasticity = !Mod::get()->getSettingValue<bool>("enabled-animation-popup");
				page->show();
			} break;
			}
		}
		else {
			auto scene = CCScene::create();
			if (targetScene == kSwitchToLast) {
				targetScene = g_eLastSwitch;
				g_eLastSwitch = g_eCurrentScene;
			}
			else if (targetScene != kSwitchToLastLevel) {
				g_eLastSwitch = g_eCurrentScene;
			}
			g_eCurrentScene = targetScene;
			switch (targetScene) {
			case kSwitchToMainMenu:
				scene = MenuLayer::scene(0);
				break;

			case kSwitchToIconKit:
				scene->addChild(GJGarageLayer::node());
				break;

			case kSwitchToMyLevels:
				scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(SearchType::MyLevels)));
				break;
			case kSwitchToFeatured:
				scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(SearchType::Featured)));
				break;

			case kSwitchToLeaderboards:
				scene->addChild(LeaderboardsLayer::create(LeaderboardState::Top100));
				break;

			case kSwitchToGauntlets:
				scene->addChild(GauntletSelectLayer::create());
				break;

			case kSwitchToSavedLevels:
				scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(SearchType::SavedLevels)));
				break;

			case kSwitchToMapPacks:
				scene->addChild(LevelBrowserLayer::create(GJSearchObject::create(SearchType::MapPack)));
				break;

			case kSwitchToTheChallenge:
				scene->release();
				scene = PlayLayer::scene(GameLevelManager::sharedState()->getMainLevel(0xbb9, false), false, false);
				break;

			case kSwitchToSearch:
				scene->addChild(LevelSearchLayer::create(0));
				break;

			case kSwitchToOnlineLevels:
				scene->addChild(CreatorLayer::create());
				break;

			case kSwitchToOfficialLevels:
				scene->addChild(LevelSelectLayer::create(0));
				break;

			case kSwitchToLastLevel:
				if (g_pLastLevel) {
					switch (g_eLastLevelWasEdit) {
					case kLastLevelTypeEditor:
						scene->addChild(EditLevelLayer::create(g_pLastLevel));
						break;

					case kLastLevelTypePlay:
						scene->addChild(LevelInfoLayer::create(g_pLastLevel, false));
						break;

					case kLastLevelTypeMain:
						scene->addChild(LevelSelectLayer::create(g_pLastLevel->m_levelIndex));
						break;
					}
				}
				else {
					this->removeFromParent();
					return;
				}
				break;
			case kSwitchToTheTower:
				scene->addChild(LevelAreaLayer::create());
				break;
			}
			CCDirector::sharedDirector()->replaceScene(
				createTransitionFromArbitaryMagicNumber(
					scene, Mod::get()->getSettingValue<int64_t>("transition-option")
				)
			);
		}
	}
	this->removeFromParent();
}

void Switcher::goTo() {
	auto scene = CCDirector::sharedDirector()->getRunningScene();
	if (scene->getChildByTag(SWITCHER_TAG)) {
		as<Switcher*>(scene->getChildByTag(SWITCHER_TAG))->go();
	}
}

Switcher::~Switcher() {
	g_vConfig = m_vConfig;
}

Switcher* Switcher::create() {
	auto ret = new Switcher;

	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}