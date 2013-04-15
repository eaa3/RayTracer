#ifndef QUADRIC_H
#define QUADRIC_H
 
typedef struct vec {            // Vector structor
   double  x, y, z;            // Coefficents of vector direction
}Vec;

typedef struct light {            // Point light structure
   Vec    dir;                // Direction to light
   double  Int;                // Light intensity
}Light;
 
typedef struct mat {            // Material structure
   double  r;                // Red component
   double  b;                // Blue component
   double  g;                // Green component
   double  Ka;                // Ambient reflection coefficent
   double  Kd;                // Diffuse reflection coefficent
   double  Ks;                // Specular reflection coefficent
   int  n;                // Specular reflection exponent
  
}Mat;
 
typedef struct quad {            // Quadric surface structure
   double  a, b, c, d, e;        // Surface coefficents
   double  f, g, h, j, k;
   Mat     m;                // Surface's material properties
}Quad;
 
typedef struct ray {            // Ray structure
   Vec  org;                // Origin of ray
   Vec  dir;                // Direction of ray
   int  depth;                // Depth (or length) of ray
}Ray;
 
 
double  dot( Vec, Vec );

double  intersect( Ray ray, Quad *obj );	

Vec     mkvec( double, double, double );	//Cria um vetor a partir das coordenadas dadas.

Vec     normalize( Vec );	//Normaliza o vetor dado no proprio vetor passado.

Vec     svmpy( double, Vec );	//Multiplica um vetor por uma constante.

Vec     vadd( Vec, Vec );	//Adiciona o 1 vetor com segundo.

Vec     vsub( Vec, Vec );	//Sutrai o 1 vetor pelo segundo.

Vec		multVetorial( Vec, Vec );   // multiplicação vetorial
 
#endif