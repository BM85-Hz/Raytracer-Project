#include <vector>
#include <string>
#include <iostream>
#include "common.h"
using namespace std;

void Rasterize(Pixel* pixels, int width, int height, const std::vector<Triangle>& tris)
{
    /*double xmin, xmax, ymin, ymax; //optimization for drawing, skips area of image that has no triangle
    //cout << tris.at(0).A << " " << tris.at(0).B << " " << tris.at(0).C;
    for (u_int i = 0; i < tris.size(); i++){
        xmin = min(min(tris.at(i).A[0], tris.at(i).B[0]), tris.at(i).C[0]);
        xmax = max(max(tris.at(i).A[0], tris.at(i).B[0]), tris.at(i).C[0]);
        ymin = min(min(tris.at(i).A[1], tris.at(i).B[1]), tris.at(i).C[1]);
        ymax = max(max(tris.at(i).A[1], tris.at(i).B[1]), tris.at(i).C[1]);
    }*/

    //cout << xmin << " " << xmax << " " << ymin << " " << ymax << " " << endl;

    double area, alpha, beta, gamma, w_a, w_b, w_c, k, alpha_prime, beta_prime, gamma_prime, z_buff, z_best;
    //for(int y = ymin; y > ymin && y < ymax; y++){ //optimaztion version of for loop
    //    for(int x = xmin; x > xmin && x < xmax; x++){

    for(int y = 0; y < height; y++){ //unoptimized for-loop scan
        for(int x = 0; x < width; x++){

            z_buff = numeric_limits<double>::max(); //z-buffer test variable

            for (u_int tri = 0; tri < tris.size(); tri++){
                //convert homogenous to euclidean (divide xyz by w)
                Triangle copyTri = tris[tri];
                w_a = copyTri.A[3];
                w_b = copyTri.B[3];
                w_c = copyTri.C[3];
                copyTri.A = copyTri.A/w_a;
                copyTri.B = copyTri.B/w_b;
                copyTri.C = copyTri.C/w_c;

                //compute barycentric for x,y (projection & edge function)
                vec2 point(2 * ((x + 0.5) / width) - 1, 2 * ((y + 0.5) / height) - 1);
                area = ((copyTri.C[0] - copyTri.A[0]) * (copyTri.B[1] - copyTri.A[1]) - (copyTri.C[1] - copyTri.A[1]) * (copyTri.B[0] - copyTri.A[0]));
                alpha = ((copyTri.C[0] - point[0]) * (copyTri.B[1] - point[1]) - (copyTri.C[1] - point[1]) * (copyTri.B[0] - point[0])) / area;
                beta = ((copyTri.A[0] - point[0]) * (copyTri.C[1] - point[1]) - (copyTri.A[1] - point[1]) * (copyTri.C[0] - point[0])) / area;
                gamma = 1 - alpha - beta;

                //perspective correct interpolation
                k = alpha/w_a + beta/w_b + gamma/w_c;
                alpha_prime = (alpha/w_a)/k;
                beta_prime = (beta/w_b)/k;
                gamma_prime = (gamma/w_c)/k;

                //check z-buffer, barycentric and return
                z_best = alpha * copyTri.A[2] + beta * copyTri.B[2] + gamma * copyTri.C[2];

                if (alpha_prime >= 0 && beta_prime >= 0 && gamma_prime >= 0 && z_buff >= z_best){
                    z_buff = z_best;
                    vec3 color = alpha_prime * copyTri.Ca + beta_prime * copyTri.Cb + gamma_prime * copyTri.Cc;
                    //draw pixel with color c
                    set_pixel(pixels, width, height, x, y, color);
                }
            }

        }
    }
}