#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



typedef struct{
    unsigned char r,g,b;


}Pixel;

typedef struct{
    char *type;
    double *position;
    double *color;
    double radius;
    double *normal;

}Object;

typedef struct{
    double width, height;
    Object object[128];

}Scene;

Scene scene;

Pixel *PixelBuffer;
lastIndex = 0;


int line = 1;

//creating a square operator
static inline double sqr(double n){
    return n*n;
}

//Sphere intersection
double sphereIntersection(double *Ro, double *Rd, double *position, double radius){
    double a,b,c;
    //sphere intersection equation provided by Dr. Palmer
    a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
    b = 2*(Rd[0]*(Ro[0]-position[0])+ Rd[0]*(Ro[1]-position[1])+Rd[2]*(Ro[2]-position[2]));
    c = sqr(Ro[0]-position[0]) + sqr(Ro[1]-position[1]) + sqr(Ro[2]-position[2]) - sqr(radius);

    double t0, t1;

    t0 = (-b - sqrt(sqr(b) - 4*(a*c)))/2*a;
    t1 = (-b + sqrt(sqr(b) - 4*(a*c)))/2*a;

    //discriminant
    double dis = sqr(b) - 4*(a*c);

    //Checking if the discriminant is 0
    //If so, there was no intersection
    if(dis = 0){
        return -1;
    }
    //The intersection is behind the camera
    //Don't render
    if(t0 < 0){
        return t1;
    }
    //Render the object
    return t0;

}

static inline double normalize(double *v){
    //getting the length of the vector
    double length = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));

    //normalizing our vector
    //turning it into a unit vector
    v[0] = v[0]/length;
    v[1] = v[1]/length;
    v[2] = v[2]/length;

}

double planeIntersection(double *Ro, double *Rd, double *position, double *normal){
    normalize(normal);

    //The length from camera to plane
    double d = -(normal[0]*position[0] + normal[1]*position[1] + normal[2]*position[2]);

    //denominator
    double denominator = (normal[0]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);
    if(denominator == 0)
        return -1;
    //plane intersection equation provided by Dr. Palmer
    //t = -(AX0 + BY0 + CZ0 + D) / (AXd + BYd + CZd)
    double t = -(normal[0]*Ro[0] + normal[1]*Ro[1] + normal[2]*Ro[2] + d)/(normal[1]*Rd[0] + normal[1]*Rd[1] + normal[2]*Rd[2]);
    //else
        return t;



}

void rayCast(double N, double M){
	int index;
	int x = 0;
	int y = 0;

	double Ro[3] = {0,0,0};

   	//M = width in pixels
	//N = height in pixels
	double c[3] = {0,0,0};
	double w = scene.width;
	double h = scene.height;
	double pixelHeight = h/M;
	double pixelWidth = w/N;

	for(int y = 0; y < M; y += 1){
		for(int x = 0; x < N; x += 1){
			double p_y = c[1] - h/2.0 + pixelHeight * (y+0.5);
			double p_x = c[0] - w/2.0 + pixelWidth * (x+0.5);
			double p_z = 1; //
			double Rd[3] = {p_x, p_y, p_x};

			normalize(Rd);


            double closestT = INFINITY; //closet point to the camera
            double *colorT; //what ever color closestT is, that is colorT


			for(index = 0; index <= lastIndex; index += sizeof(Object)){
				double t = 0;
				//Shoot function
				if(scene.object[index].type = "sphere"){
					t = sphereIntersection(Ro, Rd, scene.object[index].position, scene.object[index].radius);
				}
				//Shoot function
				if(scene.object[index].type = "plane"){
					t = planeIntersection(Ro, Rd, scene.object[index].position, scene.object[index].normal);

				}
				 if(t > 0 && t < closestT){
                    closestT = t;

                    colorT = scene.object[index].color;

                }

                if(closestT > 0 && closestT != INFINITY){

                	int  BufferPosition = (int)((M - y -1)*N +x);

                    double r = colorT[0] * 255;
                    double g = colorT[1] * 255;
                    double b = colorT[2] * 255;

                    int R = (int)r;
                    int G = (int)g;
                    int B = (int)b;

                    PixelBuffer[BufferPosition].r = R;
                    PixelBuffer[BufferPosition].g = G;
                    PixelBuffer[BufferPosition].b = B;

                }


		}


		}
	}








}


// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%lf", &value);
  // Error check this..
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


void read_scene(char* filename) {
  int index = -1;


  int c;
  FILE* json = fopen(filename, "r");

  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {

    //
    index++;
    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
	exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);



      if (strcmp(value, "camera") == 0) {
            //assuming camera is not first in the array
          index--;
      } else if (strcmp(value, "sphere") == 0) {
          scene.object[index].type = "sphere";
      } else if (strcmp(value, "plane") == 0) {
          scene.object[index].type = "plane";
      } else {
	fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
	exit(1);
      }

      skip_ws(json);

      while (1) {
	// , }
	c = next_c(json);
	if (c == '}') {
	  // stop parsing this object
	  break;
	} else if (c == ',') {
	  // read another field
	  skip_ws(json);
	  char* key = next_string(json);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);
        //populating object array with our json contents
        if (strcmp(key, "width") == 0) {
            scene.width = next_number(json);
        } else if (strcmp(key, "height") == 0){
            scene.height = next_number(json);
        } else if (strcmp(key, "radius") == 0){
            scene.object[index].radius = next_number(json);
        } else if ((strcmp(key, "color") == 0)){
            scene.object[index].color = next_vector(json);
        } else if (strcmp(key, "position") == 0){
            scene.object[index].position = next_vector(json);
        } else if (strcmp(key, "normal") == 0) {
            scene.object[index].normal = next_vector(json);
        } else {
            fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
		    key, line);
	    //char* value = next_string(json);
	  }
	  skip_ws(json);

	} else {
	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
	  exit(1);
	}
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
	// noop
	skip_ws(json);
      } else if (c == ']') {
	fclose(json);
	return;
      } else {
	fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	exit(1);
      }
    }
  }
}

int printScene(){
    int index = 0;
    while(scene.object[index].color != NULL){

    //printf("object: %d\n", index);
    //printf("type %s\n", scene.object[index].type);
   // printf("color: %f %f %f\n", scene.object[index].color[0],scene.object[index].color[1],scene.object[index].color[2]);
   // printf("position: %f %f %f\n", scene.object[index].position[0],scene.object[index].position[1],scene.object[index].position[2]);
    if(scene.object[index].normal != NULL){
    //printf("normal: %f %f %f\n", scene.object[index].normal[0],scene.object[index].normal[1],scene.object[index].normal[2]);
    }
    else{
    //printf("radius: %f\n", scene.object[index].radius);
    }
    index++;
    }
return index;
}

int main(int c, char** argv) {
  read_scene("objects.json");
  //printScene();
  lastIndex = printScene();
  return 0;
}

