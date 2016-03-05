#pragma once

#include <TextArea.h>
#include <shader/ComponentShaderText.h>
#include <EventManager.h>
#include <functional>

class PD_UI_Text : public TextArea {
public:

	PD_UI_Text(BulletWorld* _bulletWorld, Font* _font, ComponentShaderText * _textShader);
	
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;

	void setTextColour(float _r, float _g, float _b);
	glm::vec3 getTextColour() const;

	void setOverColour(float _r, float _g, float _b);
	glm::vec3 getOverColour() const;

	void setDownColour(float _r, float _g, float _b);
	glm::vec3 getDownColour() const;

	std::function<void(sweet::Event * _event)> onOver;
	std::function<void(sweet::Event * _event)> onClick;
	std::function<void(sweet::Event * _event)> onDown;

	void enable();
	void disable();

	bool isEnabled();

private:
	glm::vec3 curTextColour;
	glm::vec3 textColour;
	glm::vec3 overColour;
	glm::vec3 downColour;
	bool enabled;
};
