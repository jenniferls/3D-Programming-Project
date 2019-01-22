#pragma once

class RawModel {
public:
	// this is how we will structure the input data for the vertex shader
	struct TriangleVertex {
		float x, y, z;
		float u, v;
		float normalX, normalY, normalZ;
	};

	RawModel(int id, int vertCount);
	~RawModel();

	int getID() const;
	int getvertCount() const;

private:
	int id;
	int vertCount;
};

