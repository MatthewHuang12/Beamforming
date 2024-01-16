#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    //Creating a double pointer of float type named RFData
    //Creating a 2d matrix of size numElement and numSample
    float **RFData;
    RFData = new float*[numElement];
    for (int i = 0; i<numElement; i++)
    {
        RFData[i] = new float[numSample];
    }

    return RFData; //RFDATA is returned
}

int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    ifstream infile(fileName); // Opening file named "fileName" with ifstream
    if (infile.fail()) //If the function failed to open the file -1 is returned indicating this failure
    {
        return -1;
    }

  

    for (int i=0; i<numElement; i++)
    {
       for (int j=0; j<numSample; j++) 
       {
            infile >> RFData[i][j];  // Reading the data in "fileName" and storing into RFData
       }

    }
    infile.close(); //Closing the file
    return 0; //0 is returned
}
   
float *genScanlineLocation(int &numPixel)
{
    float scanlinedepth; //float that stores the depth of the scanline
    cout << "Please enter the desired depth of the scanline" << endl;
    cin >> scanlinedepth;
    cout << "Please enter the desired number of pixels of the scanline" << endl;
    cin >> numPixel; //numPixel is a call by reference variable allowing the original data to be modified in the variable's memory location

    float *scanlineLocation = new float[numPixel]; //Array is created for scanlineLocation which has numPixels amount of elements
    for (int i=0; i<numPixel; i++)
    {
        scanlineLocation[i]= (scanlinedepth/(numPixel-1))*i; //First element will always be 0 and last element will be the imagimng depth
       
    }
    return scanlineLocation;//scanlineLocation is returned
}

// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
   float *eleLocation= new float[numElement]; //An array, eleLocation of float type was created with 128 elements(The size of numElement) and it stores the element location (in x-direction)
   int N = numElement; //int N stores numElement
   for (int i=0; i<numElement; i++)
   {

    eleLocation[i] = ((i-((N-1)/2.0)) * PITCH);//Formula to find the element location (in x-direction), dividing by 2.0 ensures the answer will be a float   
   }
   return eleLocation; //eleLocation is returned
}
  

float *createScanline(int numPixel)
{
    float *scanline = new float[numPixel];// Allocated float array to store the beamformed results called scanline
    return scanline; //Returned scanline
}

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    float tforward[numPixel]; //An array, tforward of float type and numPixel elements big was created to store the forward time of the flight of the ultrasound wave
    
    float **tbackward; //A doubler pointer, tbackward of float type was created to store the backward time of the flight of the ultrasound wave
    tbackward = new float*[numPixel];//Dynamically allocates memory for the array of pointers to float. The array is numPixel elements big
   
    float **ttotal; //A doubler pointer, ttotal of float type was created to store the total time of the flight of the ultrasound wave
    ttotal= new float*[numPixel]; //Dynamically allocates memory for the array of pointers to float. The array is numPixel elements big

    int **s; //A doubler pointer, s of float type was created to determine which sample from the k^th transducer eleement matched with the i^th scanline location
    s = new int*[numPixel]; //Dynamically allocates memory for the array of pointers to float. The array is numPixel elements big

    float Preal[numPixel];//An array, Preal of float type and numPixel elements big was created to store the real part of the pulse signal
    float Pimag[numPixel];//An array, Pimag of float type and numPixel elements big was created to store the imaginary part of the pulse signal
    float mag[numPixel];//An array, mag of float type and numPixel elements big was created to store the echo magnitude
   
    for (int i=0; i<numPixel; i++)
    {
    tforward[i]= scanlinePosition[i]/SoS; //Calculates forward time travelled
    tbackward[i]= new float[numElement]; //Creates 2d matrix for tbackward of size numPixel and numElement
    ttotal[i]= new float[numElement]; //Creates 2d matrix for ttotal of size numPixel and numElement
    s[i]= new int[numElement]; //Creates 2d matrix for s of size numPixel and numElement
    Preal[i] = 0.0; //Initialize Preal to 0.0
    Pimag[i] = 0.0; //Initialize Pimag to 0.0
    
    for (int k=0; k<numElement; k++)
        {
            tbackward[i][k]= sqrt((pow(scanlinePosition[i], 2)) + pow(elementPosition[k], 2))/SoS;//Uses pythagorean theorem to determine backwards time traveled

            ttotal[i][k]= tforward[i] + tbackward[i][k];//Calculates total time of flight for the ultrasound wave
            s[i][k]= floor(ttotal[i][k]*FS);//Determines which sample from the k^th transducer eleement matched with the i^th scanline location
            Preal[i]+=realRFData[k][s[i][k]];//Generating real part of pulse signal
            Pimag[i]+=imagRFData[k][s[i][k]];//Generating imaginary part of pulse signal
        }
         mag[i]= sqrt(pow(Preal[i], 2) + pow(Pimag[i], 2)); //Deriving echo magnitude at i^th scanline location
         scanline[i] = mag[i]; //Scanline stores the magnitude at the i^th scanline location, void function returns nothing
    }




}
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{

    ofstream outfile(fileName); //Opened an output file "fileName" using ofstream
    if (outfile.fail()) //If file failed to create or open -1 is returned indicating this failure
    {
        return -1;
    }
    for (int i=0; i<numPixel; i++)
    {
        outfile << scanlinePosition[i] << "," << scanline[i] << endl; //Iterating through each of the scanline locations and elements, and storing them in the output file
    }
    return 0;
}
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    // Release all the allocated memory in scanline, scanlinePosition, elementPosition, realRFDATA, and imag RFDATA
    delete[] scanline; 
    delete[] scanlinePosition;
    delete[] elementPosition;

    for (int i=0; i< numElement; i++)
    {
        delete[] realRFData[i];
        delete[] imagRFData[i];
    }
    //Had to release the memory allocated for each floater point therefore realRFDATA and imagRFDATA had to be deleted twice
    delete[] realRFData;
    delete[] imagRFData;

}