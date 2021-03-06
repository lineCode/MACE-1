/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <Catch.hpp>
#include <MACE/Core/Instance.h>

namespace mc {
	class TestModule:public mc::Module {
	public:
		TestModule() : mc::Module() {};

		int updates = 0;
		bool isInit = false;


		void init() {
			isInit = true;
		};

		void update() {
			updates++;
		}
		void destroy() {
			isInit = false;
		};
		std::string getName() const {
			return "Test";
		};
	};

	TEST_CASE("Testing reset() and numberOfModules()") {
		Instance MACE = Instance();

		REQUIRE(MACE.numberOfModules() == 0);

		MACE.reset();

		REQUIRE(MACE.numberOfModules() == 0);

		TestModule m = TestModule();
		TestModule m1 = TestModule();
		TestModule m2 = TestModule();
		
		MACE.addModule(m);

		SECTION("Testing with 1 module") {
			REQUIRE(MACE.moduleExists(&m));
			REQUIRE_FALSE(MACE.moduleExists(&m1));
			REQUIRE_FALSE(MACE.moduleExists(&m2));
			REQUIRE(MACE.numberOfModules() == 1);
		}

		MACE.addModule(m1);

		SECTION("Testing with 2 modules") {
			REQUIRE(MACE.moduleExists(&m));
			REQUIRE(MACE.moduleExists(&m1));
			REQUIRE_FALSE(MACE.moduleExists(&m2));
			REQUIRE(MACE.numberOfModules() == 2);
		}

		MACE.addModule(m2);

		SECTION("Testing with 3 modules") {
			REQUIRE(MACE.moduleExists(&m));
			REQUIRE(MACE.moduleExists(&m1));
			REQUIRE(MACE.moduleExists(&m2));
			REQUIRE(MACE.numberOfModules() == 3);
		}

		MACE.reset();

		SECTION("Testing reset()") {
			REQUIRE(MACE.numberOfModules() == 0);
			REQUIRE_FALSE(MACE.moduleExists(&m));
			REQUIRE_FALSE(MACE.moduleExists(&m1));
			REQUIRE_FALSE(MACE.moduleExists(&m2));

			REQUIRE_FALSE(MACE.getFlag(MACE.INIT));
			REQUIRE_FALSE(MACE.getFlag(MACE.DESTROYED));
			REQUIRE_FALSE(MACE.getFlag(MACE.STOP_REQUESTED));
		}

	}

	TEST_CASE("Testing modules", "[module][system]") {
		Instance MACE = Instance();

		TestModule m = TestModule();
		TestModule m2 = TestModule();

		MACE.addModule(m2);

		SECTION("Testing moduleExists()") {
			REQUIRE(MACE.moduleExists(&m2));
			REQUIRE_FALSE(MACE.moduleExists(&m));

			REQUIRE(MACE.getModule("") == nullptr);
		}

		MACE.addModule(m);

		SECTION("Testing init() and update() with modules") {
			REQUIRE(MACE.moduleExists(m.getName()));
			REQUIRE(MACE.moduleExists(&m));
			REQUIRE(MACE.moduleExists(&m2));
			REQUIRE(MACE.moduleExists(m2.getName()));

			mc::Initializer i(MACE);

			REQUIRE(MACE.moduleExists(m2.getName()));
			REQUIRE(MACE.moduleExists(&m2));
			MACE.update();
		}

		MACE.removeModule(m);

		SECTION("Testing moduleExists()") {
			REQUIRE_FALSE(MACE.moduleExists(&m));
			REQUIRE(MACE.moduleExists(&m2));
		}

		MACE.removeModule(m2);
	}

	TEST_CASE("Testing flags") {
		Instance MACE = Instance();

		TestModule m = TestModule();

		MACE.addModule(m);
		SECTION("Testing with module") {
			SECTION("Testing init() and update()") {
				MACE.init();

				REQUIRE(MACE.getFlag(MACE.INIT));
				REQUIRE_FALSE(MACE.getFlag(MACE.DESTROYED));
				REQUIRE_FALSE(MACE.getFlag(MACE.STOP_REQUESTED));


				for( Index i = 0; i < 10; i++ ) {
					REQUIRE(m.updates == i);
					MACE.update();
					REQUIRE(MACE.isRunning());
					REQUIRE_FALSE(MACE.getFlag(MACE.STOP_REQUESTED));
				}

				MACE.requestStop();

				REQUIRE(MACE.getFlag(MACE.STOP_REQUESTED));
				REQUIRE(!MACE.isRunning());

				MACE.destroy();

				REQUIRE(MACE.getFlag(MACE.DESTROYED));
				REQUIRE_FALSE(MACE.getFlag(MACE.INIT));
				REQUIRE_FALSE(MACE.getFlag(MACE.STOP_REQUESTED));

			}
		}

		MACE.removeModule(m);
	}

	TEST_CASE("Modules getting updated", "[module][system]") {
		Instance MACE = Instance();
		TestModule m = TestModule();


		REQUIRE(MACE.numberOfModules() == 0);

		MACE.addModule(m);

		SECTION("Testing with module") {
			SECTION("Testing init() and update()") {

				REQUIRE_FALSE(m.isInit);
				REQUIRE(m.updates == 0);

				MACE.init();

				REQUIRE(m.isInit);
				REQUIRE(m.updates == 0);

				for( Index i = 0; i < 10; i++ ) {
					REQUIRE(m.updates == i);
					MACE.update();
				}

				MACE.destroy();

				REQUIRE(m.updates == 10);
				REQUIRE_FALSE(m.isInit);

			}
		}

		MACE.removeModule(m);

	}
}