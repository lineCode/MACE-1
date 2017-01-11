/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <vector>

namespace mc {
	namespace gfx {
		namespace {
			int IMAGE_PROTOCOL = -1;
			int PROGRESS_BAR_PROTOCOL = -1;
			int LETTER_PROTOCOL = -1;

			ogl::Texture whiteTexture;

			FT_Library freetype;
			//error if freetype init failed or -1 if it hasnt been created
			int freetypeStatus = -1;

			std::vector<FT_Face> fonts = std::vector<FT_Face>();
		}//anon namespace

		Entity2D::Entity2D() : GraphicsEntity() {}

		//IMAGE

		int Image::getProtocol() {
			return IMAGE_PROTOCOL;
		}

		Image::Image() noexcept {}

		Image::Image(const ogl::Texture & tex) : Image() {
			texture = tex;
		}

		Image::Image(const Color & col) : Image(whiteTexture) {
			if( !texture.isCreated() ) {
				texture.init();

				float data[] = { 1,1,1,1 };

				texture.setData(data, 1, 1, GL_FLOAT, GL_RGBA);

				texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}

			setPaint(col);
		}

		void Image::onInit() {
			if( !texture.isCreated() ) {
				texture.init();
			}

			if( IMAGE_PROTOCOL < 0 ) {
				IMAGE_PROTOCOL = Renderer::registerProtocol<Image>();
			}
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			Renderer::queue(this, IMAGE_PROTOCOL);
		}

		void Image::onDestroy() {
			if( texture.isCreated() ) {
				texture.destroy();
			}
		}


		void Image::setTexture(const ogl::Texture & tex) {
			if( tex != texture ) {
				makeDirty();

				texture = tex;
			}
		}

		ogl::Texture & Image::getTexture() {
			makeDirty();

			return texture;
		}

		const ogl::Texture & Image::getTexture() const {
			return texture;
		}

		bool Image::operator==(const Image & other) const {
			return Entity2D::operator==(other) && texture == other.texture;
		}

		bool Image::operator!=(const Image & other) const {
			return !operator==(other);
		}

		void RenderProtocol<Image>::resize(const Size, const Size) {}

		void RenderProtocol<Image>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
		#	include <MACE/Graphics/Shaders/image.v.glsl>
			};
			const char* fragmentShader2D = {
		#	include <MACE/Graphics/Shaders/image.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);
		}//init

		void RenderProtocol<Image>::setUp(os::WindowModule*, RenderQueue*) {};

		void RenderProtocol<Image>::render(os::WindowModule*, GraphicsEntity* e) {
			Image* entity = dynamic_cast<Image*>(e);
			if( entity == nullptr ) {
				throw NullPointerException("You must provide an Image for RenderProtocol<Image>");
			}

			entity->texture.bind();

			renderer.bind();

			renderer.draw(e);
		}//render

		void RenderProtocol<Image>::tearDown(os::WindowModule*, RenderQueue*) {}

		void RenderProtocol<Image>::destroy() {
			renderer.destroy();
		}//destroy

		//PROGRESS BAR

		int ProgressBar::getProtocol() {
			return PROGRESS_BAR_PROTOCOL;
		}

		ProgressBar::ProgressBar() noexcept: ProgressBar(0, 0, 0) {}

		ProgressBar::ProgressBar(const float minimum, const float maximum, const float prog) noexcept : min(minimum), max(maximum), progress(prog) {}

		void ProgressBar::setBackgroundTexture(const ogl::Texture & tex) {
			if( backgroundTexture != tex ) {
				makeDirty();

				backgroundTexture = tex;
			}
		}

		ogl::Texture & ProgressBar::getBackgroundTexture() {
			makeDirty();

			return backgroundTexture;
		}

		const ogl::Texture & ProgressBar::getBackgroundTexture() const {
			return backgroundTexture;
		}

		void ProgressBar::setForegroundTexture(const ogl::Texture & tex) {
			if( foregroundTexture != tex ) {
				makeDirty();

				foregroundTexture = tex;
			}
		}

		ogl::Texture & ProgressBar::getForegroundTexture() {
			makeDirty();

			return foregroundTexture;
		}

		const ogl::Texture & ProgressBar::getForegroundTexture() const {
			return foregroundTexture;
		}

		void ProgressBar::setSelectionTexture(const ogl::Texture & tex) {
			if( selectionTexture != tex ) {
				makeDirty();

				selectionTexture = tex;
			}
		}

		ogl::Texture & ProgressBar::getSelectionTexture() {
			makeDirty();

			return selectionTexture;
		}

		const ogl::Texture & ProgressBar::getSelectionTexture() const {
			return selectionTexture;
		}


		void ProgressBar::setMinimum(const float minimum) {
			if( min != minimum ) {
				makeDirty();

				min = minimum;
			}
		}

		float & ProgressBar::getMinimum() {
			makeDirty();

			return min;
		}

		const float & ProgressBar::getMinimum() const {
			return min;
		}

		void ProgressBar::setMaximum(const float maximum) {
			if( max != maximum ) {
				makeDirty();

				max = maximum;
			}
		}

		float & ProgressBar::getMaximum() {
			makeDirty();

			return max;
		}

		const float & ProgressBar::getMaximum() const {
			return max;
		}

		void ProgressBar::setProgress(const float prog) {
			if( progress != prog ) {
				makeDirty();

				progress = prog;
			}
		}

		void ProgressBar::addProgress(const float prog) {
			if( prog != 0 ) {
				makeDirty();

				progress += prog;
			}
		}

		float & ProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const float & ProgressBar::getProgress() const {
			return progress;
		}


		void ProgressBar::easeTo(const float progress, const float time, const EaseFunction function, const EaseDoneCallback callback) {
			class EaseComponent: public Component {
			public:
				EaseComponent(const float p, const float t, const float sp, const EaseFunction f, const EaseDoneCallback cb) : Component(), startProg(sp), prog(p), time(t), func(f), start(0), done(cb) {};

				const float prog;
				const float startProg;
				float start;
				const float time;
				const EaseDoneCallback done;
				const EaseFunction func;
			protected:
				void init(Entity* e) override {}
				bool update(Entity* e) override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
					if( bar == nullptr ) {
						//should never happen, as this class is only ever defined and used here, but just in caes
						throw NullPointerException("Internal error: EaseComponent did not receive a progress bar in update()");
					}

					//combine 2 operations into 1
					float difference = start++ / time;
					float percentDone;

					if( func == EaseFunction::SINUSOIDAL ) {
						percentDone = static_cast<float>(std::sin(difference * (math::pi() / 2)));
					} else if( func == EaseFunction::COSINE ) {
						percentDone = 1.0f - static_cast<float>(std::cos(difference * (math::pi() / 2)));
					} else if( func == EaseFunction::QUADRATIC ) {
						percentDone = difference*difference;
					} else if( func == EaseFunction::CUBIC ) {
						percentDone = difference*difference*difference;
					} else if( func == EaseFunction::QUARTIC ) {
						percentDone = difference*difference*difference*difference;
					} else if( func == EaseFunction::QUINTIC ) {
						percentDone = difference*difference*difference*difference*difference;
					} else if( func == EaseFunction::SQUARE_ROOT ) {
						percentDone = std::sqrt(difference);
					} else if( func == EaseFunction::SQUARE_ROOT ) {
						percentDone = std::cbrt(difference);
					} else {
						//linear as a fallback
						percentDone = difference;
					}

					//meaning that we are easing backwards
					if( startProg > prog ) {
						bar->setProgress(startProg - (startProg - prog)*(percentDone));
					} else {
						bar->setProgress(startProg + (prog - startProg)*(percentDone));
					}

					//if we got there or time has run out
					if( bar->getProgress() == prog || difference >= 1.0f ) {
						return true;
					}
					return false;
				}
				void render(Entity* e) override {}
				void destroy(Entity* e) override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
					if( bar == nullptr ) {
						throw NullPointerException("Internal error: EaseComponent did not receive a progress bar in destroy()");
					}
					done(bar);

                    //honorable suicide
					delete this;
				}
			};

			//it deletes itself in destroy();
			EaseComponent* com = new EaseComponent(progress, time, this->progress, function, callback);
			addComponent(com);

			makeDirty();
		}

		bool ProgressBar::operator==(const ProgressBar & other) const {
			return Entity2D::operator==(other) && max == other.max&&min == other.min&&progress == other.progress&&backgroundTexture == other.backgroundTexture&&foregroundTexture == other.foregroundTexture&&selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar & other) const {
			return !operator==(other);
		}

		void ProgressBar::onInit() {
			if( !backgroundTexture.isCreated() ) {
				backgroundTexture.init();
			}

			if( !foregroundTexture.isCreated() ) {
				foregroundTexture.init();
			}

			if( !selectionTexture.isCreated() ) {
				selectionTexture.init();
			}

			if( PROGRESS_BAR_PROTOCOL < 0 ) {
				PROGRESS_BAR_PROTOCOL = Renderer::registerProtocol<ProgressBar>();
			}
		}

		void ProgressBar::onUpdate() {}

		void ProgressBar::onRender() {
			Renderer::queue(this, PROGRESS_BAR_PROTOCOL);
		}

		void ProgressBar::onDestroy() {
			if( backgroundTexture.isCreated() ) {
				backgroundTexture.destroy();
			}

			if( foregroundTexture.isCreated() ) {
				foregroundTexture.destroy();
			}

			if( selectionTexture.isCreated() ) {
				selectionTexture.destroy();
			}
		}

		void RenderProtocol<ProgressBar>::resize(const Size, const Size) {}

		void RenderProtocol<ProgressBar>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
#	include <MACE/Graphics/Shaders/progressbar.v.glsl>
			};
			const char* fragmentShader2D = {
#	include <MACE/Graphics/Shaders/progressbar.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("backgroundTexture");
			prog.createUniform("foregroundTexture");
			prog.createUniform("selectionTexture");

			prog.createUniform("progress");

			prog.setUniform("backgroundTexture", 0);
			prog.setUniform("foregroundTexture", 1);
			prog.setUniform("selectionTexture", 2);
		}//init

		void RenderProtocol<ProgressBar>::setUp(os::WindowModule*, RenderQueue*) {};

		void RenderProtocol<ProgressBar>::render(os::WindowModule*, GraphicsEntity* e) {
			ProgressBar* entity = dynamic_cast<ProgressBar*>(e);
			if( entity == nullptr ) {
				throw NullPointerException("You must provide an ProgressBar for RenderProtocol<ProgressBar>");
			}

			entity->backgroundTexture.bindToLocation(0);
			entity->foregroundTexture.bindToLocation(1);
			entity->selectionTexture.bindToLocation(2);

			renderer.bind();
			renderer.getShader().setUniform("progress", (entity->progress - entity->min) / (entity->max - entity->min));

			ogl::checkGLError(__LINE__, __FILE__);

			renderer.draw(e);
		}//render

		void RenderProtocol<ProgressBar>::tearDown(os::WindowModule*, RenderQueue*) {}

		void RenderProtocol<ProgressBar>::destroy() {
			renderer.destroy();
		}//destroy

		Font Font::loadFont(const std::string& name){
            loadFont(name.c_str());
		}

		Font Font::loadFont(const char* name){
            if( freetypeStatus < 0 ) {
				if( freetypeStatus = FT_Init_FreeType(&freetype) ) {
					throw InitializationError("Freetype failed to initailize with error code "+freetypeStatus);
				}
			}

            Index id = fonts.size();

            fonts.push_back(FT_Face());
            if(int result = FT_New_Face(freetype, name, 0, &fonts[id])){
                throw InitializationError("Freetype failed to initialize font with result "+std::to_string(result));
            }

            return Font(id);
		}

		void Font::setSize(const Size h){
            this->height = h;
            FT_Set_Pixel_Sizes(fonts[id], 0, h);
        }

        Size& Font::getSize(){
            return height;
        }

        const Size& Font::getSize() const{
            return height;
        }

		Font::Character Font::getCharacter(const char c){
            if(int result = FT_Load_Char(fonts[id], c, FT_LOAD_RENDER)){
                throw InitializationError("Failed to load glyph with error code "+std::to_string(result));
            }

            Character character = Character();
            character.width = fonts[id]->glyph->bitmap.width;
            character.height = fonts[id]->glyph->bitmap.rows;
            character.bearingX = fonts[id]->glyph->bitmap_left;
            character.bearingY = fonts[id]->glyph->bitmap_top;
            character.advance = fonts[id]->glyph->advance.x;

            //because the buffer is 1 byte long
            character.texture = ogl::Texture();
            character.texture.init();
            character.texture.bind();

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            character.texture.setData(fonts[id]->glyph->bitmap.buffer, character.width, character.height, GL_UNSIGNED_BYTE, GL_RED, GL_RED);
            character.texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            character.texture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            character.texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            character.texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            return character;
		}

		Font::Font(const Index fontID): id(fontID){}

        int Letter::getProtocol() {
			return LETTER_PROTOCOL;
		}

		Letter::Letter(const ogl::Texture& tex): texture(tex) {}

		const ogl::Texture& Letter::getTexture() const{
            return texture;
		}

        bool Letter::operator==(const Letter& other) const{
            return Entity2D::operator==(other)&&texture==other.texture;
        }

        bool Letter::operator!=(const Letter& other) const{
            return !operator==(other);
        }

        void Letter::onInit(){
            if( !texture.isCreated() ) {
				texture.init();
			}

			if( LETTER_PROTOCOL < 0 ) {
				LETTER_PROTOCOL = Renderer::registerProtocol<Letter>();
			}
        }

        void Letter::onUpdate(){

        }

        void Letter::onRender(){
            Renderer::queue(this, LETTER_PROTOCOL);
        }

        void Letter::onDestroy(){
            if(texture.isCreated()){
                texture.destroy();
            }
        }

		bool Text::operator==(const Text & other) const {
			return Entity2D::operator==(other) && letters == other.letters;
		}

		bool Text::operator!=(const Text & other) const {
			return !operator==(other);
		}

		void Text::onInit() {
			if( !hasChild(letters) ) {
				addChild(letters);
			}
		}

		void Text::onUpdate() {}

		void Text::onRender() {}

		void Text::onDestroy() {}

		void Text::onClean() {}
	}//gfx
}//mc
