#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
// added libriaries
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

vector<vector<float>> inputImage;
vector<vector<bool>> outputImage;
int height;
int width;

void read_image(char *filename)
{

    string type;
    int maxValue;
    int pixelValue;

    ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        cout << "Unable to open InputImageFile" << endl;
        return;
    }

    inputFile >> type >> width >> height >> maxValue;

    inputImage.resize(height);
    outputImage.resize(height);

    for (int i = 0; i < width; i++)
    {
        inputImage[i].resize(width);
        outputImage[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            inputFile >> pixelValue;
            inputImage[i][j] = (float)pixelValue / maxValue;
        }
    }
}

void write_image(char *filename)
{

    ofstream outputFile(filename);

    if (!outputFile.is_open())
    {
        cout << "Unable to open OutputImageFile" << endl;
        return;
    }

    outputFile << "P1" << endl
               << width << " " << height << endl
               << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            outputFile << !outputImage[i][j] << " ";
        }
        outputFile << endl;
    }
}

// add function check
void add(int i, int j, float v, int n, vector<vector<float>> &AccErr)
{
    if (i >= 0 && j >= 0 && i < n && j < n)
    {
        AccErr[i][j] += v;
    }
}

void convert_using_thresholding(void)
{

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            outputImage[i][j] = (float)(inputImage[i][j] > 0.5);
}

// thresholding with noise
void convert_using_thresholding_noise(void)
{

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            // create random value
            float HI = 1 - inputImage[i][j];
            float LO = -inputImage[i][j];
            float r = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));
            // generate the output
            outputImage[i][j] = (float)(inputImage[i][j] > 0.5 + (float)r);
        }
}

// dithering 3x3 version
void convert_using_dithering_3(void)
{
    // dithering 3x3 matrix
    float M[3][3] = {{(float)2 / (float)9, (float)6 / (float)9, (float)4 / (float)9},
                     {(float)5 / (float)9, (float)0, (float)1 / (float)9},
                     {(float)8 / (float)9, (float)3 / (float)9, (float)7 / (float)9}};
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            outputImage[i][j] = (float)(inputImage[i][j] > M[i % 3][j % 3]);
}

// 4x4 version
void convert_using_dithering_4(void)
{
    // dithering 4x4 matrix
    float M[4][4] = {
        {(float)2 / (float)16, (float)1, (float)3 / (float)16, (float)13 / float(16)},
        {(float)10 / (float)16, (float)6 / (float)16, (float)11 / (float)16, (float)7 / (float)16},
        {(float)4 / (float)16, (float)14 / (float)16, (float)1 / (float)16, (float)15 / (float)16},
        {(float)12 / (float)16, (float)8 / (float)16, (float)9 / (float)16, (float)5 / (float)16}};
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            outputImage[i][j] = (float)(inputImage[i][j] > M[i % 4][j % 4]);
}

// Floyd-Steinberg Error-Diffusion Algorithm
void convert_using_error_diffusion(void)
{
    vector<vector<float>> AccErr;
    AccErr.resize(height);
    // Error matrix initialization
    for (int i = 0; i < width; i++)
    {
        AccErr[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            AccErr[i][j] = (float)0;
        }
    }

    // main loop
    for (int i = 1; i < height - 1; i++)
    {

        for (int j = 0; j < width - 1; j++)
        {
            float err = 0;
            float val = inputImage[i][j] + AccErr[i][j];

            if (val > 0.5)
            {
                outputImage[i][j] = (float)1;
                err = val - 1;
            }
            else
            {
                outputImage[i][j] = (float)0;
                err = val;
            }
            AccErr[i + 1][j] += (float)7 / (float)16 * err;
            AccErr[i - 1][j + 1] += (float)3 / (float)16 * err;
            AccErr[i][j + 1] += (float)5 / (float)16 * err;
            AccErr[i + 1][j + 1] += (float)1 / (float)16 * err;
        }
    }
}

// Floyd-Steinberg Error-Diffusion Algorithm with noise
void convert_using_error_diffusion_noise(void)
{
    vector<vector<float>> AccErr;
    AccErr.resize(height);
    // Error matrix initialization
    for (int i = 0; i < width; i++)
    {
        AccErr[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            AccErr[i][j] = (float)0;
        }
    }

    // main loop
    for (int i = 1; i < height - 1; i++)
    {

        for (int j = 0; j < width - 1; j++)
        {
            float err = 0;
            float val = inputImage[i][j] + AccErr[i][j];

            // create random value
            float HI = 1 - val;
            float LO = -val;
            float r = LO + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HI - LO)));

            if (val > 0.5 + (float)r)
            {
                outputImage[i][j] = (float)1;
                err = val - 1;
            }
            else
            {
                outputImage[i][j] = (float)0;
                err = val;
            }
            AccErr[i + 1][j] += (float)7 / (float)16 * err;
            AccErr[i - 1][j + 1] += (float)3 / (float)16 * err;
            AccErr[i][j + 1] += (float)5 / (float)16 * err;
            AccErr[i + 1][j + 1] += (float)1 / (float)16 * err;
        }
    }
}

// Tone-Dependent Weights Error-Diffusion Algorithm
void convert_using_error_diffusion_tdw(void)
{
    int n = width;
    vector<vector<float>> AccErr;
    AccErr.resize(height);
    // Error matrix initialization
    for (int i = 0; i < width; i++)
    {
        AccErr[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            AccErr[i][j] = (float)0;
        }
    }

    // main loop
    for (int i = 2; i < height - 2; i++)
    {
        for (int j = 0; j < width - 2; j++)
        {
            float err = 0;
            float val = inputImage[i][j] + AccErr[i][j];

            if (val > 0.5)
            {
                outputImage[i][j] = (float)1;
                err = val - 1;
            }
            else
            {
                outputImage[i][j] = (float)0;
                err = val;
            }
            // Here the computation of the error depends on the pixel input value
            if (inputImage[i][j] < float(40) / float(255))
            { // Shadows
                add(i + 1, j, (float)7 / (float)16 * err, n, AccErr);
                add(i - 1, j + 1, (float)3 / (float)16 * err, n, AccErr);
                add(i, j + 1, (float)5 / (float)16 * err, n, AccErr);
                add(i + 1, j + 1, (float)1 / (float)16 * err, n, AccErr);
            }
            else if (inputImage[i][j] < float(215) / float(255))
            { // Midtones
                AccErr[i + 1][j] += (float)7 / (float)24 * err;
                AccErr[i + 2][j] += (float)2 / (float)24 * err;
                AccErr[i - 2][j + 1] += (float)1 / (float)24 * err;
                AccErr[i - 1][j + 1] += (float)3 / (float)24 * err;
                AccErr[i][j + 1] += (float)5 / (float)24 * err;
                AccErr[i + 1][j + 1] += (float)1 / (float)24 * err;
                AccErr[i + 2][j + 1] += (float)0.5 / (float)24 * err;
                AccErr[i - 2][j + 2] += (float)1 / (float)24 * err;
                AccErr[i - 1][j + 2] += (float)1 / (float)24 * err;
                AccErr[i][j + 2] += (float)2 / (float)24 * err;
                AccErr[i + 1][j + 2] += (float)0.5 / (float)24 * err;
            }
            else
            { // Highlights
                add(i + 1, j, (float)7 / (float)16 * err, n, AccErr);
                add(i - 1, j + 1, (float)3 / (float)16 * err, n, AccErr);
                add(i, j + 1, (float)5 / (float)16 * err, n, AccErr);
                add(i + 1, j + 1, (float)1 / (float)16 * err, n, AccErr);
            }
        }
    }
}

// Functions to process pixels followning an Hilbert Curve

// Rotation function, depending in which part of the block the pixel is
void rot(int n, int *x, int *y, int rx, int ry)
{
    if (ry == 0)
    {
        if (rx == 1)
        {
            *x = n - 1 - *x;
            *y = n - 1 - *y;
        }
        // Swap x and y
        int t = *x;
        *x = *y;
        *y = t;
    }
}

// function to convert (i,j) pixel coord into Hilbert curve 1D coord
int xy2d(int n, int x, int y)
{
    int rx, ry, s, d = 0;
    for (s = n / 2; s > 0; s /= 2)
    {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(n, &x, &y, rx, ry);
    }
    return d;
}

// function to convert back to (i,j) pixel coord from Hilbert curve 1D coord
void d2xy(int n, int d, int *x, int *y)
{
    int rx, ry, s, t = d;
    *x = *y = 0;
    for (s = 1; s < n; s *= 2)
    {
        rx = 1 & (t / 2);
        ry = 1 & (t ^ rx);
        rot(s, x, y, rx, ry);
        *x += s * rx;
        *y += s * ry;
        t /= 4;
    }
}

// Floyd-Steinberg Error-Diffusion Algorithm following the Hilbert curve
void convert_using_error_diffusion_h(void)
{
    vector<vector<float>> AccErr;
    AccErr.resize(height);
    // Error matrix initialization
    for (int i = 0; i < width; i++)
    {
        AccErr[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            AccErr[i][j] = (float)0;
        }
    }

    // main loop
    int n = width;
    for (int k = 0; k < n * n; ++k)
    {
        int i, j;
        d2xy(n, k, &i, &j);
        j = n - 1 - j;
        float err = 0;
        float val = inputImage[i][j] + AccErr[i][j];

        if (val > 0.5)
        {
            outputImage[i][j] = (float)1;
            err = val - 1;
        }
        else
        {
            outputImage[i][j] = (float)0;
            err = val;
        }
        add(i + 1, j, (float)7 / (float)16 * err, n, AccErr);
        add(i - 1, j + 1, (float)3 / (float)16 * err, n, AccErr);
        add(i, j + 1, (float)5 / (float)16 * err, n, AccErr);
        add(i + 1, j + 1, (float)1 / (float)16 * err, n, AccErr);
    }
}

//Tone-Dependent Weights Floyd-Steinberg Error-Diffusion Algorithm following the Hilbert curve
void convert_using_error_diffusion_combo(void)
{
    vector<vector<float>> AccErr;
    AccErr.resize(height);
    // Error matrix initialization
    for (int i = 0; i < width; i++)
    {
        AccErr[i].resize(width);
        for (int j = 0; j < height; j++)
        {
            AccErr[i][j] = (float)0;
        }
    }

    // main loop
    int n = width;
    for (int k = 0; k < n * n; ++k)
    {
        int i, j;
        d2xy(n, k, &i, &j);
        j = n - 1 - j;
        float err = 0;
        float val = inputImage[i][j] + AccErr[i][j];

        if (val > 0.5)
        {
            outputImage[i][j] = (float)1;
            err = val - 1;
        }
        else
        {
            outputImage[i][j] = (float)0;
            err = val;
        }
        // Here the computation of the error depends on the pixel input value
        if (inputImage[i][j] < float(40) / float(255))
        { // Shadows
            add(i + 1, j, (float)7 / (float)16 * err, n, AccErr);
            add(i - 1, j + 1, (float)3 / (float)16 * err, n, AccErr);
            add(i, j + 1, (float)5 / (float)16 * err, n, AccErr);
            add(i + 1, j + 1, (float)1 / (float)16 * err, n, AccErr);
        }
        else if (inputImage[i][j] < float(215) / float(255))
        { // Midtones
            add(i + 1, j,(float)7 / (float)24 * err, n , AccErr);
            add(i + 2, j,(float)2 / (float)24 * err, n , AccErr);
            add(i - 2, j+1,(float)1 / (float)24 * err, n , AccErr);
            add(i - 1, j+1,(float)3 / (float)24 * err, n , AccErr);
            add(i, j+1,(float)5 / (float)24 * err, n , AccErr);
            add(i + 1, j+1,(float)1 / (float)24 * err, n , AccErr);
            add(i + 2, j+1,(float)0.5 / (float)24 * err, n , AccErr);
            add(i - 2, j+2,(float)2 / (float)24 * err, n , AccErr);
            add(i - 1, j+2,(float)1 / (float)24 * err, n , AccErr);
            add(i , j+2,(float)2 / (float)24 * err, n , AccErr);
            add(i + 1, j+2,(float)0.5 / (float)24 * err, n , AccErr);
        
        }
        else
        { // Highlights
            add(i + 1, j, (float)7 / (float)16 * err, n, AccErr);
            add(i - 1, j + 1, (float)3 / (float)16 * err, n, AccErr);
            add(i, j + 1, (float)5 / (float)16 * err, n, AccErr);
            add(i + 1, j + 1, (float)1 / (float)16 * err, n, AccErr);
        }
    }
}

int main(int argc, char **argv)
{
    auto start = high_resolution_clock::now();

    // if(argc < 4){
    //     cout<<"Usage: Halftoning IntputImageFileName OutputImageFileName Method"<<endl;
    //     cout<<"Method = Thresholding | Dithering | ErrorDiffusion"<<endl;
    //     return 0;
    // }

    read_image(argv[1]);

    if (strcmp(argv[3], "ErrorDiffusionExtension") == 0)
    {
        if ((argc == 5) && (strcmp(argv[4], "Hilbert") == 0))
            convert_using_error_diffusion_h();
        else if ((argc == 5) && (strcmp(argv[4], "combo") == 0))
            convert_using_error_diffusion_combo();
        else
            convert_using_error_diffusion_tdw();
    }

    else if (strcmp(argv[3], "Thresholding") == 0)
    {
        if ((argc == 5) && (strcmp(argv[4], "noise") == 0))
            convert_using_thresholding_noise();
        else
            convert_using_thresholding();
    }

    else
    {
        if (strcmp(argv[3], "Dithering") == 0)
        {
            if (argc < 5)
                convert_using_dithering_4(); // default
            else if (strcmp(argv[4], "3") == 0)
                convert_using_dithering_3();
            else if (strcmp(argv[4], "4") == 0)
                convert_using_dithering_4();
        }

        else
        {
            if (strcmp(argv[3], "ErrorDiffusion") == 0)
            {
                if ((argc == 5) && (strcmp(argv[4], "noise") == 0))
                    convert_using_error_diffusion_noise();
                else
                    convert_using_error_diffusion();
            }

            else
            {
                cout << "Usage: Halftoning IntputImageFileName OutputImageFileName Method" << endl;
                cout << "Method = Thresholding | Dithering | ErrorDiffusion" << endl;
                return 0;
            }
        }
    }

    write_image(argv[2]);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "time:" << duration.count() << endl;

    return 0;
}
