#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h";

struct ProjectedPoint {
    Vector2 ndc;
    bool visible; // false = za near/far plane lub poza frustumem
};

class LightController {
    
private:
    Vector3 cameraPos = { 0,0, 30 };

    int& width;
    int& height;

    // Gradient f(u) = u^T D u => grad = 2 D u
    Vector3 computeNormal(const Matrix4x4& Dp, const Vector4& p) const {
        Vector4 vec = Dp * p;
        return Vector3(vec.x, vec.y, vec.z).normalize();
    }

public:
    LightController(int& width, int& height)
        : width(width), height(height) {
    }

    float intensity(const Vector3& v, const Vector3& n, float m) const {
        float dot = std::max(0.0f, v.dot(n));
        return std::pow(dot, m);
    }

    void fillPixels(std::vector<unsigned char>& pixels) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int baseIdx = y * width + x;

                pixels[baseIdx * 3] = 0;
                pixels[baseIdx * 3 + 1] = 0;
                pixels[baseIdx * 3 + 2] = 0;

                if (y > height / 3 && y < 2 * height / 3 && x < 2 * width / 3 && x > width / 3) {
                    pixels[baseIdx * 3 + 1] = 255;
                }
            }
        }
    }

    void drawGrid(std::vector<unsigned char>& pixels) const {
        float aspect = (float)width / height;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Mapowanie na współrzędne -1 do 1
                float xf = (2.0f * x / width - 1.0f) * aspect;
                float yf = 1.0f - 2.0f * y / height;

                int baseIdx = (y * width + x) * 3;

                // Rysowanie osi X (czerwona) i Y (zielona)
                // Używamy małego marginesu (epsilon), aby linia miała grubość
                float epsilon = 0.01f;

                // Oś X (yf bliskie 0)
                if (std::abs(yf) < epsilon / 2.0f) {
                    pixels[baseIdx] = 255;   // R
                }
                // Oś Y (xf bliskie 0)
                if (std::abs(xf) < epsilon / 2.0f) {
                    pixels[baseIdx + 1] = 255; // G
                }

                // Opcjonalnie: Siatka (cieńsze linie co 0.5 jednostki)
                if (std::abs(std::fmod(xf, 0.5f)) < 0.005f || std::abs(std::fmod(yf, 0.5f)) < 0.005f) {
                    if (pixels[baseIdx] == 0 && pixels[baseIdx + 1] == 0) { // Nie nadpisuj osi głównych
                        pixels[baseIdx] = 50;
                        pixels[baseIdx + 1] = 50;
                        pixels[baseIdx + 2] = 50;
                    }
                }

                bool isGrid = (x % 50 == 0 || y % 50 == 0); // Linia co 50 pikseli
                if (isGrid) {
                    pixels[baseIdx] = 30; // Ciemnoszary kolor siatki
                    pixels[baseIdx + 1] = 30;
                    pixels[baseIdx + 2] = 30;
                }
            }
        }
    }


    Vector3 projectPoint(const Vector3& p3D, const Matrix4x4& M,
        const Matrix4x4& P, int W, int H)
    {
        Vector4 clipSpace = P * (M * Vector4(p3D.x, p3D.y, p3D.z, 1.0f));

        if (clipSpace.w <= 0.0f)              return {-10,-10, -10};

        Vector3 ndc(clipSpace.x / clipSpace.w,
            clipSpace.y / clipSpace.w,
            clipSpace.z / clipSpace.w);



        return ndc;
    }

    // Prosty algorytm rysowania linii w tablicy pixels
    void drawLine(std::vector<unsigned char>& pixels, Vector3 start, Vector3 end,
        int W, int H, Vector3 color)
    {
        int x0 = (int)((start.x + 1.0f) * 0.5f * W),
            x1 = (int)((end.x + 1.0f) * 0.5f * W),
            y0 = (int)((1.0f - (start.y + 1.0f) * 0.5f) * H),
            y1 = (int)((1.0f - (end.y + 1.0f) * 0.5f) * H);

        int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2;

        // Długość linii w pikselach – do interpolacji z
        int totalSteps = std::max(std::abs(x1 - x0), std::abs(y1 - y0));
        int step = 0;

        while (true) {
            // Interpolacja NDC.z wzdłuż linii (0.0 = start, 1.0 = end)
            float t = (totalSteps > 0) ? (float)step / totalSteps : 0.0f;
            float ndcZ = start.z + t * (end.z - start.z);
            float ndcX = start.x + t * (end.x - start.x);
            float ndcY = start.y + t * (end.y - start.y);

            // Sprawdzenie czy piksel jest w frustumie [-1,1] na każdej osi
            bool inFrustum = ndcX >= -1.0f && ndcX <= 1.0f &&
                ndcY >= -1.0f && ndcY <= 1.0f &&
                ndcZ >= -1.0f && ndcZ <= 1.0f;

            // Sprawdzenie czy piksel jest w granicach ekranu
            bool inScreen = x0 >= 0 && x0 < W && y0 >= 0 && y0 < H;

            if (inFrustum && inScreen) {
                int idx = (y0 * W + x0) * 3;
                pixels[idx] = (unsigned char)color.x;
                pixels[idx + 1] = (unsigned char)color.y;
                pixels[idx + 2] = (unsigned char)color.z;
            }

            if (x0 == x1 && y0 == y1) break;
            step++;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
};
