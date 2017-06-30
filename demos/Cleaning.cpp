#include <MACE/MACE.h>
#include <random>
#include <ctime>
#include <iostream>

using namespace mc;

class TestComponent: public gfx::Component {
	void init() override {
		parent->setProperty(gfx::Entity::MAINTAIN_X, true);
		parent->setProperty(gfx::Entity::MAINTAIN_Y, true);
		parent->setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
		parent->setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);
	}

	bool update() override {
		return false;
	}

	void render() override {}

	void hover() override {
		if( os::Input::isKeyDown(os::Input::MOUSE_LEFT) ) {
			parent->makeDirty();
		}
	}

	void destroy() override {}

	void clean() override {
		dynamic_cast<gfx::Image*>(parent)->getTexture().setPaint(Color((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 0.5f));
	}
};

gfx::Group group = gfx::Group();

gfx::Image left;
gfx::Image leftBot;
gfx::Image rightTop;
gfx::Image rightBot;

void create(os::WindowModule&) {
	srand((unsigned) time(0));

	const Size elementNum = 10;

	left = gfx::Image(gfx::Texture(Colors::GREEN));
	leftBot = gfx::Image(gfx::Texture(Colors::GRAY));
	rightTop = gfx::Image(gfx::Texture(Colors::YELLOW));
	rightBot = gfx::Image(gfx::Texture(Colors::ORANGE));

	left.setY(0.0f);
	left.setX(-0.5f);
	left.setHeight(0.9f);
	left.setWidth(0.45f);

	leftBot.setX(0.0f);
	leftBot.setY(-0.5f);
	leftBot.setWidth(0.7f);
	leftBot.setHeight(0.4f);
	leftBot.setProperty(gfx::Entity::MAINTAIN_WIDTH, true);
	leftBot.setProperty(gfx::Entity::MAINTAIN_HEIGHT, true);

	left.addChild(leftBot);

	rightTop.setX(0.5f);
	rightTop.setY(0.5f);
	rightTop.setHeight(0.4f);
	rightTop.setWidth(0.4f);

	rightBot.setHeight(0.4f);
	rightBot.setWidth(0.4f);
	rightBot.setX(0.5f);
	rightBot.setY(-0.5f);

	left.addComponent(SmartPointer<gfx::Component>(new TestComponent(), true));
	leftBot.addComponent(SmartPointer<gfx::Component>(new TestComponent(), true));
	rightTop.addComponent(SmartPointer<gfx::Component>(new TestComponent(), true));
	rightBot.addComponent(SmartPointer<gfx::Component>(new TestComponent(), true));

	group.addChild(left);
	group.addChild(rightTop);
	group.addChild(rightBot);
}

int main() {
	try {
		os::WindowModule::LaunchConfig config = os::WindowModule::LaunchConfig(600, 500, "Cleaning Demo");
		config.onCreate = &create;
		config.resizable = true;
		os::WindowModule module = os::WindowModule(config);

		MACE::addModule(module);

		module.addChild(group);

		gfx::FPSComponent f = gfx::FPSComponent();
		f.setTickCallback([] (gfx::FPSComponent* com, gfx::Entity*) {
			std::cout << "UPS: " << com->getUpdatesPerSecond() << " FPS: " << com->getFramesPerSecond() << " Frame Time: " << float(1000.0f) / com->getFramesPerSecond() << std::endl;
		});
		module.addComponent(f);

		os::SignalModule sigModule = os::SignalModule();
		MACE::addModule(sigModule);

		MACE::start();
	} catch( const std::exception& e ) {
		Error::handleError(e);
		return -1;
	}
	return 0;
}
