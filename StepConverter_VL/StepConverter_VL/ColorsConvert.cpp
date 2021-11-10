//Last update time : 18.09.2013 02:22:19
#include <math.h>

/*void inline rgb2lab( double R, double G, double B, double& L, double& a,  double& b)
{
//RGB -> XYZ
	
	__declspec(align(16)) double var_rgb[4] = { R, G, B, 0.0 };
	//float var_R = ( (float)R / 255.0f );        //R from 0 to 255
	//float var_G = ( (float)G / 255.0f );        //G from 0 to 255
	//float var_B = ( (float)B / 255.0f );        //B from 0 to 255

	var_rgb[0] /= 255.0;
	var_rgb[1] /= 255.0;
	var_rgb[2] /= 255.0;
	
 if ( var_rgb[0] > 0.04045 )	var_rgb[0] = pow( ( var_rgb[0] + 0.055 ) / 1.055 , 2.4 );
 else							var_rgb[0] = var_rgb[0] / 12.92f;
 if ( var_rgb[1] > 0.04045 )	var_rgb[1] = pow( ( var_rgb[1] + 0.055 ) / 1.055 , 2.4 );
 else							var_rgb[1] = var_rgb[1] / 12.92;
 if ( var_rgb[2] > 0.04045 )	var_rgb[2] = pow( ( var_rgb[2] + 0.055 ) / 1.055 , 2.4 );
 else							var_rgb[2] = var_rgb[2] / 12.92;
 
 	var_rgb[0] *= 100.0;
	var_rgb[1] *= 100.0;
	var_rgb[2] *= 100.0;

//Observer. = 2°, Illuminant = D65
__declspec(align(16)) double var_xyz[4];
 var_xyz[0] = var_rgb[0] * 0.4124 + var_rgb[1] * 0.3576 + var_rgb[2] * 0.1805;
 var_xyz[1] = var_rgb[0] * 0.2126 + var_rgb[1] * 0.7152 + var_rgb[2] * 0.0722;
 var_xyz[2] = var_rgb[0] * 0.0193 + var_rgb[1] * 0.1192 + var_rgb[2] * 0.9505;

 //XYZ -> Lab
//Observer= 2°, Illuminant= D65
 var_xyz[0] /= 95.047;
 var_xyz[1] /= 100.00;
 var_xyz[2] /= 108.883;

if ( var_xyz[0] > 0.008856 )	var_xyz[0] = pow(var_xyz[0] , 1.0/3.0 );
else							var_xyz[0] = ( 7.787 * var_xyz[0] ) + ( 16.0 / 116.0 );
if ( var_xyz[1] > 0.008856 )	var_xyz[1] = pow(var_xyz[1] , 1.0/3.0 );
else							var_xyz[1] = ( 7.787 * var_xyz[1] ) + ( 16.0 / 116.0 );
if ( var_xyz[2] > 0.008856 )	var_xyz[2] = pow(var_xyz[2] , 1.0/3.0 );
else							var_xyz[2] = ( 7.787 * var_xyz[2] ) + ( 16.0 / 116.0 );

 L = double(( 116.0 * var_xyz[1] ) - 16.0);
 a = double(500.0 * ( var_xyz[0] - var_xyz[1] ));
 b = double(200.0 * ( var_xyz[1] - var_xyz[2] ));
}*/

void inline rgb2lab( float R, float G, float B, float& L, float& a,  float& b) //use sse41
{
//RGB -> XYZ
	
	__declspec(align(16)) float var_rgb[4] = { R, G, B, 0.0f };

	__m128 rgba = _mm_load_ps( var_rgb );
	rgba = _mm_div_ps( rgba, _mm_set1_ps(255.0f) );
	_mm_store_ps(var_rgb, rgba);
	
 if ( var_rgb[0] > 0.04045f )	var_rgb[0] = powf( ( var_rgb[0] + 0.055f ) / 1.055f , 2.4f );
 else							var_rgb[0] = var_rgb[0] / 12.92f;
 if ( var_rgb[1] > 0.04045f )	var_rgb[1] = powf( ( var_rgb[1] + 0.055f ) / 1.055f , 2.4f );
 else							var_rgb[1] = var_rgb[1] / 12.92f;
 if ( var_rgb[2] > 0.04045f )	var_rgb[2] = powf( ( var_rgb[2] + 0.055f ) / 1.055f , 2.4f );
 else							var_rgb[2] = var_rgb[2] / 12.92f;
 
 rgba = _mm_load_ps( var_rgb );
 rgba = _mm_mul_ps( rgba, _mm_set1_ps(100.0f) );
 _mm_store_ps(var_rgb, rgba);
 
//Observer. = 2°, Illuminant = D65
 __m128 vX = _mm_dp_ps( rgba, _mm_set_ps( 0.0f, 0.1805f, 0.3576f, 0.4124f), 0x71 );
 __m128 vY = _mm_dp_ps( rgba, _mm_set_ps( 0.0f, 0.0722f, 0.7152f, 0.2126f), 0x72 );
 __m128 vZ = _mm_dp_ps( rgba, _mm_set_ps( 0.0f, 0.9505f, 0.1192f, 0.0193f), 0x74 );
 vX = _mm_or_ps( vX, _mm_or_ps(vY,vZ));

 //XYZ -> Lab
 //Observer= 2°, Illuminant= D65
 vX = _mm_div_ps( vX, _mm_set_ps( 0.0, 108.883f, 100.00f, 95.047f) );
 
 __declspec(align(16)) float var_xyz[4];
 
 _mm_store_ps(var_xyz, vX);
 
if ( var_xyz[0] > 0.008856f )	var_xyz[0] = powf(var_xyz[0] , 1.0f/3.0f );
else							var_xyz[0] = ( 7.787f * var_xyz[0] ) + ( 16.0f / 116.0f );
if ( var_xyz[1] > 0.008856f )	var_xyz[1] = powf(var_xyz[1] , 1.0f/3.0f );
else							var_xyz[1] = ( 7.787f * var_xyz[1] ) + ( 16.0f / 116.0f );
if ( var_xyz[2] > 0.008856f )	var_xyz[2] = powf(var_xyz[2] , 1.0f/3.0f );
else							var_xyz[2] = ( 7.787f * var_xyz[2] ) + ( 16.0f / 116.0f );

 L = ( 116.0f * var_xyz[1] ) - 16.0f;
 a = 500.0f * ( var_xyz[0] - var_xyz[1] );
 b = 200.0f * ( var_xyz[1] - var_xyz[2] );
}

int CieLab2Hue( float var_a, float var_b )          //Function returns CIE-H° value
{
    float var_bias = 0.0f;
    if ( var_a >= 0 && var_b == 0 ) return 0;
    if ( var_a <  0 && var_b == 0 ) return 180;
    if ( var_a == 0 && var_b >  0 ) return 90;
    if ( var_a == 0 && var_b <  0 ) return 270;
    if ( var_a >  0 && var_b >  0 ) var_bias = 0;
    if ( var_a <  0               ) var_bias = 180;
    if ( var_a >  0 && var_b <  0 ) var_bias = 360;
	float at = atanf( var_b / var_a );
	at = at * 180.0f / 3.14159265358979323846f; //3.1415926535897932384626433832795;
					  
    return ( at + var_bias );
}

float Delta2000b(float R, float G, float B)
{
	float L;
	float a;
	float b;
	
	rgb2lab(R, G, B, L, a, b);

	float xC1 = sqrtf( ( a * a ) + ( b * b ) );

 float xCX = xC1 / 2.0f;
 float xGX = 0.5f * ( 1.0f - sqrtf( powf(xCX , 7.0f) / ( powf(xCX,7.0f) + 6103515625.0f ) ) );
 float xNN = ( 1.0f + xGX ) * a;
 xC1 = sqrt( xNN * xNN + b * b );
// float xH1 = CieLab2Hue( xNN, b );

 float xDL = -L;
 float xDC = -xC1;

 float xLX = L / 2.0f;
 float xCY = xC1 / 2.0f;

 //float xHX=xH1;
 
 
 //float xTX = 1.0f - 0.17f * cos( ( xHX - 30.0f ) / 180.0f * 3.14159265358979323846f ) + 0.24f
 //               * cos( ( 2.0f * xHX ) / 180.0f * 3.14159265358979323846f ) + 0.32f
 //               * cos( ( 3.0f * xHX + 6.0f ) / 180.0f * 3.14159265358979323846f ) - 0.20f
 //               * cos( ( 4.0f * xHX - 63.0f ) / 180.0f * 3.14159265358979323846f );
 //float xPH = 30.0f * exp( - ( ( xHX  - 275.0f ) / 25.0f ) * ( ( xHX  - 275.0f ) / 25.0f ) );
// float xRC = 2.0f * sqrt( powf(xCY,7.0f) / ( powf(xCY,7.0f) + 6103515625.0f ) );
 float xSL = 1.0f + ( ( 0.015f * ( ( xLX - 50.0f ) * ( xLX - 50.0f ) ) )
         / sqrt( 20.0f + ( ( xLX - 50.0f ) * ( xLX - 50.0f ) ) ) );
 float xSC = 1.0f + 0.045f * xCY;
 //float xRT = - sin( ( 2.0f * xPH ) / 180.0f * 3.14159265358979323846f ) * xRC;
 xDL = xDL / xSL;
 xDC = xDC / xSC;

 return sqrtf( xDL*xDL + xDC*xDC );
}

void getDiffColor(const void* rgba, unsigned int len_in, int h, int w, float* outval) //outval - 256 * 256 * sizeof(double)
{
	unsigned char* irgba = (unsigned char*)rgba;
	for(unsigned int i=0; i<len_in; i+=4)
	{
		float B = (float)irgba[ i   ];
		float G = (float)irgba[ i+1 ];
		float R = (float)irgba[ i+2 ];
		outval[i/4] = Delta2000b( R, G, B );
	}
}