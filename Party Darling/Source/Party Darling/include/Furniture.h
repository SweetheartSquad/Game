#pragma once

#include <RoomObject.h>

class Furniture: public RoomObject {
public:

	Furniture(BulletWorld * _world, MeshInterface * _mesh, Anchor_t _anchor = Anchor_t::GROUND);
	~Furniture(void);

	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};
