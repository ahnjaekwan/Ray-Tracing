//
// raytrace.cpp
//

#define _CRT_SECURE_NO_WARNINGS
#include "matm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Ray
{
	vec4 origin;
	vec4 dir;
};

// TODO: add structs for spheres, lights and anything else you may need.

struct Sphere
{
	string name;
	vec4 origin;
	vec3 scale;
	vec3 rgb;
	float Ka, Kd, Ks, Kr, n;
	mat4 M;
	mat4 M_inverse;
	Sphere(string name1, vec4 origin1, vec3 scale1, vec3 rgb1, float Ka1, float Kb1, float Ks1, float Kr1, float n1)
		: name(name1), origin(origin1), scale(scale1), rgb(rgb1), Ka(Ka1), Kd(Kb1), Ks(Ks1), Kr(Kr1), n(n1)
	{
		M = Translate(origin)*Scale(scale);
		InvertMatrix(M, M_inverse);
	}
};

struct Light
{
	string name;
	vec4 origin;
	vec3 Irgb;
	Light(string name1, vec4 origin1, vec3 Irgb1)
		: name(name1), origin(origin1), Irgb(Irgb1)
	{}
};

struct Intersect
{
	vec4 position;		// Point position of intersect
	vec4 normal;		// Normal vector at intersect
	float distance;		// Distance of intersect point from camera
	int s_index;
	Intersect()
	{}
	Intersect(vec4 position1, vec4 normal1, float distance1, int index1)
		: position(position1), normal(normal1), distance(distance1), s_index(index1)
	{}
};

vector<vec4> g_colors;

float g_left;
float g_right;
float g_top;
float g_bottom;
float g_near;

//RES
int g_width;
int g_height;

//SPHERE
vector<Sphere> s;
int s_index = 0;

//LIGHT
vector<Light> l;
int l_index = 0;

//BACK
vec3 g_back;

//AMBIENT
vec3 g_ambient;

//OUTPUT
string g_outputName;
// -------------------------------------------------------------------
// Input file parsing

inline vec3 toVec3(vec4 in)
{
	return vec3(in[0], in[1], in[2]);
}

vec3 toVec3(const string& s1, const string& s2, const string& s3)
{
	stringstream ss(s1 + " " + s2 + " " + s3);
	vec3 result;
	ss >> result.x >> result.y >> result.z;
	return result;
}

vec4 toVec4(const string& s1, const string& s2, const string& s3)
{
	stringstream ss(s1 + " " + s2 + " " + s3);
	vec4 result;
	ss >> result.x >> result.y >> result.z;
	result.w = 1.0f;
	return result;
}

float toFloat(const string& s)
{
	stringstream ss(s);
	float f;
	ss >> f;
	return f;
}

void parseLine(const vector<string>& vs)
{
	//TODO: add parsing of NEAR, LEFT, RIGHT, BOTTOM, TOP, SPHERE, LIGHT, BACK, AMBIENT, OUTPUT.
	const int num_labels = 11;
	const string labels[] = { "NEAR", "LEFT", "RIGHT", "BOTTOM", "TOP", "RES", "SPHERE", "LIGHT", "BACK", "AMBIENT", "OUTPUT" };
	unsigned label_id = find(labels, labels + num_labels, vs[0]) - labels;
	switch (label_id)
	{
	case 0: g_near = toFloat(vs[1]); break; //NEAR
	case 1: g_left = toFloat(vs[1]); break; //LEFT
	case 2: g_right = toFloat(vs[1]); break; //RIGHT
	case 3: g_bottom = toFloat(vs[1]); break; //BOTTOM
	case 4: g_top = toFloat(vs[1]); break; //TOP
	case 5: //RES
		g_width = (int)toFloat(vs[1]);
		g_height = (int)toFloat(vs[2]);
		g_colors.resize(g_width * g_height);
		break;
	case 6: //SPHERE
		s.push_back(Sphere(vs[1], toVec4(vs[2], vs[3], vs[4]), toVec3(vs[5], vs[6], vs[7]), toVec3(vs[8], vs[9], vs[10]),
			toFloat(vs[11]), toFloat(vs[12]), toFloat(vs[13]), toFloat(vs[14]), toFloat(vs[15])));
		break;
	case 7: //LIGHT
		l.push_back(Light(vs[1], toVec4(vs[2], vs[3], vs[4]), toVec3(vs[5], vs[6], vs[7])));
		break;
	case 8: //BACK
		g_back = toVec3(vs[1], vs[2], vs[3]);
		break;
	case 9: //AMBIENT
		g_ambient.x = toFloat(vs[1]);
		g_ambient.y = toFloat(vs[2]);
		g_ambient.z = toFloat(vs[3]);
		break;
	case 10: //OUTPUT
		g_outputName = vs[1];
		break;
	}
}

void loadFile(const char* filename)
{
	ifstream is(filename);
	if (is.fail())
	{
		cout << "Could not open file " << filename << endl;
		exit(1);
	}
	string s;
	vector<string> vs;
	while (!is.eof())
	{
		vs.clear();
		getline(is, s);
		istringstream iss(s);
		while (!iss.eof())
		{
			string sub;
			iss >> sub;
			vs.push_back(sub);
		}
		parseLine(vs);
	}
}


// -------------------------------------------------------------------
// Utilities

void setColor(int ix, int iy, const vec4& color)
{
	int iy2 = g_height - iy - 1; // Invert iy coordinate.
	g_colors[iy2 * g_width + ix] = color;
}
// -------------------------------------------------------------------
// Intersection routine

// TODO: add your ray-sphere intersection routine here.

bool intersection(const Ray& ray, Intersect &intersect)
{
	// DONE: add your ray-sphere intersection routine here.
	bool exist = false;
	vector<Intersect> intersect_List;
		
	for (int i = 0; i < (int)s.size(); i++) //For each sphere in the scene
	{
		vec3 S_inverse = toVec3(s[i].M_inverse * ray.origin);
		vec3 c_inverse = toVec3(s[i].M_inverse * ray.dir);
		float A = dot(c_inverse, c_inverse);
		float B = dot(S_inverse, c_inverse);
		float C = dot(S_inverse, S_inverse) - 1;
		float discriminant = B * B - A * C;


		bool check = false;
		bool isHollow = false;
		float solution;

		if (discriminant < 0); // If discriminant < 0, no intersect, so do nothing
		else if (discriminant == 0) // If determinant = 0, one intersect
		{  
		    // Find intersection
			solution = -1 * B / A;
			// Check if intersection point is valid
			if ((ray.origin + solution * ray.dir).z <= -g_near && solution >= 0.0001f) // Intersection point should be behind the near plane. Use 0.0001f so it doesn't intersect itself due to rounding error
				check = true;
		}
		else // If discriminant > 0, two intersect points
		{  
			// Get two solutions
			float solutions[2];
			solutions[0] = -1 * (B + sqrt(discriminant)) / A;
			solutions[1] = -1 * (B - sqrt(discriminant)) / A;
			// Use the lesser of two
			solution = fminf(solutions[0], solutions[1]);
			if ((ray.origin + solution * ray.dir).z <= -g_near && solution >= 0.0001f) // Intersection point should be behind the near plane. Use 0.0001f so it doesn't intersect itself due to rounding error
				check = true;
			else // If not valid
			{
				solution = fmaxf(solutions[0], solutions[1]); // Use the larger one
				if ((ray.origin + solution * ray.dir).z <= -g_near && solution >= 0.0001f) // Check whether if it is valid
				{
					isHollow = true;
					check = true;
				}
			}			
		}

		if (check) // Push the intersection informations into intersect_List
		{
			vec4 intersection_point = ray.origin + ray.dir * solution;
			vec4 normal = intersection_point - s[i].origin;
			if (isHollow) 
				normal = -normal;
			mat4 trans = transpose(s[i].M_inverse);
			normal = trans * s[i].M_inverse * normal;
			normal.w = 0;
			normal = normalize(normal);
			intersect_List.push_back(Intersect(intersection_point, normal, length(intersection_point - toVec3(ray.origin)), i));
			exist = true;
		}
	}

	// If any intersection exists, compare intersection distance and pick the smallest distance and return true
	if (exist) 
	{
		// Find the intersection with the smallest distance
		intersect = intersect_List[0];
		for (int i = 1; i < (int)intersect_List.size(); i++) // Start from index 1 because we already set index 0 as a starter
		{	
			if (intersect_List[i].distance < intersect.distance) {
				intersect = intersect_List[i];
			}
		}
		return true;
	}
	return false; //Return false if no intersects detected
}


// -------------------------------------------------------------------
// Ray tracing

bool inShadow(const Ray &shadowRay, const Intersect &intersect, const Light &light)
{
	if (dot(shadowRay.dir, intersect.normal) < 0) // Return true right away when object is shadowing itself. Check dot product being negative value
		return true;

	float distance_light = length(light.origin - shadowRay.origin); // Light distance is determined length between light origin and shadow origin

	Intersect shadow;
	if (intersection(shadowRay, shadow))  // shadow has the shortest distance to shadowRay origin
	{		
		if (shadow.distance < distance_light) // If light distance is longer than shadow distance, then it is in shadow
			return true;
		else
			return false;
	}
	return false;   // Return false if no intersects detected
}

// -------------------------------------------------------------------
// Ray tracing

vec4 trace(const Ray &ray, bool &found, int num_recursion)
{
	// DONE: implement your ray tracing routine here.

	Intersect intersect;
	if (!intersection(ray, intersect)) // If there is no intersection, then assign background color to pixel
	{  
		found = false;
		return vec4(g_back);
	}
	else // If there is any intersection, intersect stores informations of intersected sphere
	{  
		found = true;
		Sphere *s_intersect = &s[intersect.s_index]; // sphere which intersects with ray

		// Find outgoing (reflected) ray based off incoming ray and intersect normal
		Ray reflected_light;    // Reflection of light ray. This will be used to find diffuse and specular
		reflected_light.origin = intersect.position;
		Ray reflected_viewer;   // Reflection of viewer ray. This will be used to find reflect trace
		reflected_viewer.origin = intersect.position;
		reflected_viewer.dir = normalize(ray.dir - 2 * dot(ray.dir, intersect.normal) * intersect.normal);
		
		vec3 ambient = vec3();
		vec3 diffuse = vec3();
		vec3 specular = vec3();

		for (int i = 0; i < (int)l.size(); i++)
		{
			vec4 light_dir;
			light_dir = normalize(l[i].origin - intersect.position); // light_dir is achieved from light and intersection position. Normalized should be applied at the end

			reflected_light.dir = normalize(2 * dot(light_dir, intersect.normal) * intersect.normal - light_dir); // Get the dir of reflection of light ray which will be used in computing specular

			Ray shadowRay; // Create shadowRay to figure out whether intersection is in shadow or not
			shadowRay.dir = light_dir;
			shadowRay.origin = intersect.position;

			if (!inShadow(shadowRay, intersect, l[i])) // Check whether intersection is in shadow. If not, sum diffuse and specualr for each light
			{
				if (dot(intersect.normal, light_dir) > 0) 
				{	
					diffuse += s_intersect->Kd * l[i].Irgb * dot(intersect.normal, light_dir) * s_intersect->rgb; // If diffuse dot product is positive value, then sum up diffuse
					if (dot(reflected_light.dir, -ray.dir) > 0)
						specular += s_intersect->Ks * l[i].Irgb * pow(dot(reflected_light.dir, -ray.dir), s_intersect->n);  // If specular dot product is positive value, then sum up specular
				}
			}
		}
		ambient = s_intersect->Ka * (s_intersect->rgb * g_ambient);

		// color_local is the sum of ambient, diffuse and specular
		vec3 color_local = ambient + diffuse + specular;

		// Recursive call trace to find color_reflected
		vec4 color_reflected = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Initialize in case of recuseDepth is 0
		if (num_recursion > 0)
		{
			bool found_next = false;  // Used to detect if reflect color is valid for tracing reflectRay
			color_reflected = trace(reflected_viewer, found_next, num_recursion - 1);
			if (!found_next)  // If object is not found, then set it zero (without this, color_reflected will be background color)
				color_reflected = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		// Return sum of color_local and color_reflected which is scaled by Kr
		return vec4(color_local) + s_intersect->Kr * color_reflected;
	}
}

vec4 getDir(int ix, int iy)
{
	// DONE: modify this. This should return the direction from the origin
	// to pixel (ix, iy), normalized.
	vec4 dir;
	dir = vec4(g_left + (float)ix * (g_right - g_left) / ((float)g_width - 1), g_bottom + iy * (g_top - g_bottom) / ((float)g_height - 1), -g_near, 0.0f);
	return normalize(dir);
}

void renderPixel(int ix, int iy)
{
	Ray ray;
	bool objFound = false;
	ray.origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	ray.dir = getDir(ix, iy);
	vec4 color = trace(ray, objFound, 3);
	setColor(ix, iy, color);
}

void render()
{
	for (int iy = 0; iy < g_height; iy++)
		for (int ix = 0; ix < g_width; ix++)
			renderPixel(ix, iy);
}


// -------------------------------------------------------------------
// PPM saving

void savePPM(int Width, int Height, char* fname, unsigned char* pixels)
{
	FILE *fp;
	const int maxVal = 255;

	printf("Saving image %s: %d x %d\n", fname, Width, Height);
	fp = fopen(fname, "wb");
	if (!fp) {
		printf("Unable to open file '%s'\n", fname);
		return;
	}
	fprintf(fp, "P6\n");
	fprintf(fp, "%d %d\n", Width, Height);
	fprintf(fp, "%d\n", maxVal);

	for (int j = 0; j < Height; j++) {
		fwrite(&pixels[j*Width * 3], 3, Width, fp);
	}

	fclose(fp);
}

void saveFile()
{
	float temp;
	// Convert color components from floats to unsigned chars.
	// DONE: clamp values if out of range.
	unsigned char* buf = new unsigned char[g_width * g_height * 3];
	for (int y = 0; y < g_height; y++)
		for (int x = 0; x < g_width; x++)
			for (int i = 0; i < 3; i++)
			{
				temp = ((float*)g_colors[y*g_width + x])[i];
				temp = (temp > 1 ? 1 : temp);   // Clamp color to 1 max
				//buf[y*g_width * 3 + x * 3 + i] = (unsigned char)(((float*)g_colors[y*g_width + x])[i] * 255.9f);		
				buf[y*g_width * 3 + x * 3 + i] = (unsigned char)(temp * 255.9f);
			}
	// DONE: change file name based on input file name.	
	char* fileName = new char[g_outputName.length() + 1];
	strcpy(fileName, g_outputName.c_str());
	savePPM(g_width, g_height, fileName, buf);
	delete[] buf;
	delete[] fileName;
}


// -------------------------------------------------------------------
// Main

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: template-rt <input_file.txt>" << endl;
		exit(1);
	}
	loadFile(argv[1]);
	render();
	saveFile();
	return 0;
}
