#pragma once
#include <vector>

class Grid {
private:
	int gridSize;
	
public:
	std::vector<float> vertices;

	Grid(int gridSize) : gridSize(gridSize) {}

    void generate() {
        vertices.clear();
        float i = -gridSize;
        while (i <= gridSize) {
            float fI = (float)i;
            float fS = (float)gridSize;

            vertices.insert(vertices.end(), { -fS, 0.0f, fI,  fS, 0.0f, fI });
            vertices.insert(vertices.end(), { fI, 0.0f, -fS,  fI, 0.0f, fS });

            i += 0.5;
        }
    }
};