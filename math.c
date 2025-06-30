#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_PI 3.14159265358979323846
#define MY_2PI 6.28318530717958647692

// Taylor series implementation of sin(x)
float my_sin(float x) {
    while (x > MY_PI) x -= MY_2PI;
    while (x < -MY_PI) x += MY_2PI;

    float x3 = x * x * x;
    float x5 = x3 * x * x;
    float x7 = x5 * x * x;

    // sin(x) = x - x^3/3! + x^5/5! - x^7/7!
    return x - (x3/6.0f) + (x5/120.0f) - (x7/5040.0f);
}

// Taylor series implementation of cos(x)
float my_cos(float x) {
    while (x > MY_PI) x -= MY_2PI;
    while (x < -MY_PI) x += MY_2PI;

    float x2 = x * x;
    float x4 = x2 * x2;
    float x6 = x4 * x2;

    // cos(x) = 1 - x^2/2! + x^4/4! - x^6/6!
    return 1.0f - (x2/2.0f) + (x4/24.0f) - (x6/720.0f);
}

#define WIDTH 80
#define HEIGHT 24
#define BUFFER_SIZE (WIDTH * HEIGHT)

#define R1 1.0f
#define R2 2.0f
#define K2 5.0f

const char LUMINANCE_CHARS[] = " .:-=+*#@";

void render_frame(float rotation_a, float rotation_b, char *buffer, float *depth_buffer) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = ' ';
    }
    memset(depth_buffer, 0, BUFFER_SIZE * sizeof(float));

    float sin_a = my_sin(rotation_a);
    float cos_a = my_cos(rotation_a);
    float sin_b = my_sin(rotation_b);
    float cos_b = my_cos(rotation_b);

    for (float theta = 0; theta < MY_2PI; theta += 0.07) {
        for (float phi = 0; phi < MY_2PI; phi += 0.01) {
            float sin_phi = my_sin(phi);
            float cos_phi = my_cos(phi);
            float sin_theta = my_sin(theta);
            float cos_theta = my_cos(theta);

            float circle_x = R2 + R1 * cos_theta;
            float circle_y = R1 * sin_theta;

            float x = circle_x * (cos_b * cos_phi + sin_a * sin_b * sin_phi) -
                      circle_y * cos_a * sin_b;
            float y = circle_x * (sin_b * cos_phi - sin_a * cos_b * sin_phi) +
                      circle_y * cos_a * cos_b;
            float z = K2 + cos_a * circle_x * sin_phi + circle_y * sin_a;
            float ooz = 1.0f / z;

            int xp = (int)(WIDTH/2 + 30 * ooz * x);
            int yp = (int)(HEIGHT/2 - 15 * ooz * y);

            int pos = xp + WIDTH * yp;

            float lum = 0.7f * sin_phi * cos_theta * sin_b -
                        0.2f * cos_phi * cos_theta * cos_b;
            int luminance_index = (int)((lum + 1.0f) * 4);
            if (luminance_index < 0) luminance_index = 0;
            if (luminance_index > 8) luminance_index = 8;
            if (yp >= 0 && yp < HEIGHT && xp >= 0 && xp < WIDTH && pos >= 0 && pos < BUFFER_SIZE && ooz > depth_buffer[pos]) {
                depth_buffer[pos] = ooz;
                buffer[pos] = LUMINANCE_CHARS[luminance_index];
            }
        }
    }
}

void display_buffer(const char *buffer) {
    printf("\x1b[H");  // Move cursor to top-left
    for (int i = 0; i < HEIGHT; i++) {
        char line[WIDTH + 1];
        memcpy(line, &buffer[i * WIDTH], WIDTH);
        line[WIDTH] = '\0';
        printf("%s\n", line);
    }
}

int main() {
    float rotation_a = 0; rotation_a += 0.9;
    float rotation_b = 0; rotation_b += 0.9;
    float z_buffer[BUFFER_SIZE];
    char output_buffer[BUFFER_SIZE];

    printf("\x1b[2J");

    while (1) {
        render_frame(rotation_a, rotation_b, output_buffer, z_buffer);
        display_buffer(output_buffer);

        rotation_a += 0.12;
        rotation_b += 0.06;

        usleep(8000);
    }

    return 0;
}