#include <cmath>
#include <vector>
#include <tuple>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 

Model *model = NULL;
const int width = 800;
const int height = 800;

std::vector<std::tuple<int,int>> line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    std::vector<std::tuple<int,int>> points;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        steep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dy = y1 - y0;
    int dx = x1 - x0;
    int derror = std::abs(dy)*2;
    int error = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) {
            // Undo transpose
            image.set(y, x, color);
            points.push_back(std::make_tuple(y,x));
        } else {
            image.set(x, y, color);
            points.push_back(std::make_tuple(x,y));
        }
        error += derror;
        if (error > dx) {
            y += (y1 > y0 ? 1:-1);
            error -= dx*2;
        }
    }
    return points;
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t1.y > t0.y) {
        std::swap(t0, t1);
    }
    if (t2.y > t0.y) {
        std::swap(t0, t2);
    }
    if (t2.y > t1.y) {
        std::swap(t1, t2);
    }
    std::vector<std::tuple<int,int>> points_a = line(t0.x, t0.y, t1.x, t1.y, image, color); 
    std::vector<std::tuple<int,int>> points_b = line(t0.x, t0.y, t2.x, t2.y, image, color);

    std::vector<int>::size_type end = points_a.size() > points_b.size() ? points_a.size():points_b.size();
    int x0 = 0;
    int y0 = 0;
    int x1 = 0;
    int y1 = 0;
    for (std::vector<int>::size_type i = 0; i < end; i++) {
        if (i >= points_a.size()) {
            x1 = std::get<0>(points_b[i]);
            y1 = std::get<1>(points_b[i]);

        } else if (i >= points_b.size()) {
            x0 = std::get<0>(points_a[i]);
            y0 = std::get<1>(points_a[i]);

        } else {
            x0 = std::get<0>(points_a[i]);
            y0 = std::get<1>(points_a[i]);
            x1 = std::get<0>(points_b[i]);
            y1 = std::get<1>(points_b[i]);
        }

        std::vector<std::tuple<int,int>> garbage = line(x0, y0, x1, y1, image, color);
        (void)garbage;
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else { 
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    // triangle(t0[0], t0[1], t0[2], image, red); 
    // triangle(t1[0], t1[1], t1[2], image, white); 
    // triangle(t2[0], t2[1], t2[2], image, green);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image, white);
        }
    }


    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}