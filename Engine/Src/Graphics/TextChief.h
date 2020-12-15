#pragma once
#include "../Core.h"
#include "../Global/GlobalFuncs.h"
#include "ShaderProg.h"

///Can be improved

class TextChief final{
public:
	enum struct TextAlignment: int{
		Left,
		Center,
		Right,
		Amt
	};

	struct TextAttribs final{
		str text = str();
		float x = 0.0f;
		float y = 0.0f;
		float scaleFactor = 1.0f;
		glm::vec4 colour = glm::vec4(1.0f);
		uint texRefID = 0;
		TextAlignment alignment = TextAlignment::Left;
	};

	TextChief();
	~TextChief();

	void RenderText(ShaderProg& SP, const TextAttribs& attribs);
private:
	struct CharMetrics final{
		uint texRefID = (uint)0;
		uint advance = (uint)0;
		glm::ivec2 bearing = glm::ivec2();
		glm::ivec2 size = glm::ivec2();
	};

	FT_Library ft;
	FT_Face face;
	std::map<char, CharMetrics> allChars;
	uint VAO;
	uint VBO;

	bool Init();
};