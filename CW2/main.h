#ifndef MAIN_H
#define MAIN_H
#define FREEGLUT_STATIC
#define _CRT_SECURE_NO_WARNINGS
// basic setting to link the static library


#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <GL/freeglut.h>
#include "lib/loadpng.h"
#include "lib/process_image.h"
#include "lib/gl_texture.h"
// import STL library and the library write by myself in the folder named lib

using namespace std;

#define WIDTH 800
#define HEIGHT 800
#define INTERVAL 15
#define MAX_SCROLL_VALUE 10
// set the global variables


class Model {
	/*
	* declare the Model class to store the .obj document with the texture document
	*/
private:
    static int count_char(string& str, char ch) {
        int c = 0;
        int length = str.length() - 1;
        for (int i = 0; i < length; i++) if (str[i] == ch) c++;
        return c;
    }

    static bool has_double_slash(string& str) {
        int length = str.length() - 2;
        for (int i = 0; i < length; i++) {
            if (str[i] == '/' && str[i + 1] == '/')
                return true;
        }
        return false;
    }

    class Material {
	/*
	* sub-class to store the material, such as: .mtl, .png documents
	*/	
    public:
        float* ambient;
        float* diffuse;
        float* specular;
        GLuint texture;

        Material(float* ambient, float* diffuse, float* specular) {
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->texture = 0;
        }
    };

    class Face {
	/*
	* sotre a face of the model
	* two kind of: triangle or quadrangle
	*/
    public:
        int edge;
        int* vertices;
        int* texcoords;
        int normal;

        Face(int edge, int* vertices, int* texcoords, int normal = -1) {
            this->edge = edge;
            this->vertices = vertices;
            this->texcoords = texcoords;
            this->normal = normal;
        }
    };

    string prefix;
    vector<Material> materials;
    unordered_map<string, int> map_material;

    vector<float*> vertices;
    vector<float*> texcoords;
    vector<float*> normals;
    vector<Face> faces;

    GLuint list;

    void load_material(const char* filename) {
		/*
		* sub-function to load the material document
		* input: file's name (with the path)
		* data-type: consrt char* (char array)
		*/
        string line;
        vector<string> lines;
        ifstream in(filename);
        if (!in.is_open()) {
            printf("Loading Error material %s is not exist.\n", filename);
            return;
        }

        while (!in.eof()) {
            getline(in, line);
            lines.push_back(line);
        }
        in.close();

        Material* m = NULL;
        int count_material = 0;
        char str[40];
        string material;
        float* a = NULL, * d = NULL, * s = NULL;

        for (string& line : lines) {
            if (line[0] == 'n' && line[1] == 'e') {
                sscanf(line.c_str(), "newmtl %s", str);
                material = str;
                map_material[material] = count_material;
                count_material++;
                a = new float[4]{ 0.2f, 0.2f, 0.2f, 1.0f };
                d = new float[4]{ 0.8f, 0.8f, 0.8f, 1.0f };
                s = new float[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
                materials.push_back(Material(a, d, s));
                m = &materials[materials.size() - 1];
            }
            else if (line[0] == 'K') {
                switch (line[1]) {
                case 'a':
                    sscanf(line.c_str(), "Ka %f %f %f", &a[0], &a[1], &a[2]);
                    break;
                case 'd':
                    sscanf(line.c_str(), "Kd %f %f %f", &d[0], &d[1], &d[2]);
                    break;
                case 's':
                    sscanf(line.c_str(), "Ks %f %f %f", &s[0], &s[1], &s[2]);
                    break;
                }
            }
            else if (line[0] == 'm' && line[1] == 'a') {
                sscanf(line.c_str(), "map_Kd %s", str);
                std::string file = prefix + str;
                Image img;
                Load_Texture_Swap(&img, file.c_str());
                glGenTextures(1, &(m->texture));
                glBindTexture(GL_TEXTURE_2D, m->texture);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.w, img.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.img);
                glBindTexture(GL_TEXTURE_2D, 0);
                Delete_Image(&img);
            }
        }
    }
	
	/*
	* detailed string reader from the document's data
	* handle the content with diffenet methods in different input
	*/
    void _3v(string& line) {
        int v0, v1, v2;
        sscanf(line.c_str(), "f %d %d %d", &v0, &v1, &v2);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        faces.push_back(Face(3, v, NULL));
    }

    void _3vt(string& line) {
        int v0, v1, v2, t0, t1, t2;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        int* t = new int[3]{ t0 - 1, t1 - 1, t2 - 1 };
        faces.push_back(Face(3, v, t));
    }

    void _3vn(string& line) {
        int v0, v1, v2, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        faces.push_back(Face(3, v, NULL, n - 1));
    }

    void _3vtn(string& line) {
        int v0, v1, v2, t0, t1, t2, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n);
        int* v = new int[3]{ v0 - 1, v1 - 1, v2 - 1 };
        int* t = new int[3]{ t0 - 1, t1 - 1, t2 - 1 };
        faces.push_back(Face(3, v, t, n - 1));
    }

    void _4v(string& line) {
        int v0, v1, v2, v3;
        sscanf(line.c_str(), "f %d %d %d %d", &v0, &v1, &v2, &v3);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        faces.push_back(Face(4, v, NULL));
    }

    void _4vt(string& line) {
        int v0, v1, v2, v3, t0, t1, t2, t3;
        sscanf(line.c_str(), "f %d/%d %d/%d %d/%d %d/%d", &v0, &t0, &v1, &t1, &v2, &t2, &v3, &t3);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        int* t = new int[4]{ t0 - 1, t1 - 1, t2 - 1, t3 - 1 };
        faces.push_back(Face(4, v, t));
    }

    void _4vn(string& line) {
        int v0, v1, v2, v3, n;
        sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d", &v0, &n, &v1, &n, &v2, &n, &v3, &n);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        faces.push_back(Face(4, v, NULL, n - 1));
    }

    void _4vtn(string& line) {
        int v0, v1, v2, v3, t0, t1, t2, t3, n;
        sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &t0, &n, &v1, &t1, &n, &v2, &t2, &n, &v3, &t3, &n);
        int* v = new int[4]{ v0 - 1, v1 - 1, v2 - 1, v3 - 1 };
        int* t = new int[4]{ t0 - 1, t1 - 1, t2 - 1, t3 - 1 };
        faces.push_back(Face(4, v, t, n - 1));
    }

public:
    float pos_x, pos_y, pos_z;
	
	/*
	* encapsulate a methods named "load" to be public and be called from the out
	*/
    void load(const char* filename) {
        string tmp = filename;
        prefix = "";
        int n = tmp.find_last_of('/') + 1;
        if (n > 0) prefix = tmp.substr(0, n);

        string line;
        vector<string> lines;
        ifstream fileStream(filename);
        if (!fileStream.is_open()) { printf("Cannot load model %s\n", filename); return; }
        while (!fileStream.eof()) getline(fileStream, line),lines.push_back(line);        
        fileStream.close();

        float a, b, c;
        char str[40];

        pos_x = pos_y = 0.0f;
        float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;

        for (string& line : lines) {
            if (line[0] == 'v') {
                if (line[1] == ' ') {
                    sscanf(line.c_str(), "v %f %f %f", &a, &b, &c);
                    (a > 0.0f) ? sum_x += a : sum_x -= a;
                    (b > 0.0f) ? sum_y += b : sum_y -= b;
                    (c > 0.0f) ? sum_z += c : sum_z -= c;
                    pos_x += a;
                    pos_y += b;
                    vertices.push_back(new float[3]{ a, b, c });
                }
                else if (line[1] == 't') 
                    sscanf(line.c_str(), "vt %f %f", &a, &b),
                    texcoords.push_back(new float[2]{ a, b });                
                else 
                    sscanf(line.c_str(), "vn %f %f %f", &a, &b, &c),
                    normals.push_back(new float[3]{ a, b, c });
                
            }
            else if (line[0] == 'f') {
                int edge = count_char(line, ' ');
                int count_slash = count_char(line, '/');
                if (count_slash == 0)
                    (edge == 3) ? _3v(line) : _4v(line);
                else if (count_slash == edge) 
                    (edge == 3) ? _3vt(line) : _4vt(line);
                else if (count_slash == edge * 2)
                    (has_double_slash(line))?
                        (edge == 3) ? _3vn(line) : _4vn(line):                    
                        (edge == 3) ? _3vtn(line) : _4vtn(line);
            }
            else if (line[0] == 'm' && line[1] == 't') {
                sscanf(line.c_str(), "mtllib %s", &str);
                string file = prefix + str;
                load_material(file.c_str());
            }
            else if (line[0] == 'u' && line[1] == 's') {
                sscanf(line.c_str(), "usemtl %s", &str);
                string material = str;
                if (map_material.find(material) != map_material.end())
                    faces.push_back(Face(-1, NULL, NULL, map_material[material]));
            }
        }
        bool has_texcoord = false;
		
		// staring to add operations into list
        list = glGenLists(1);		
        glNewList(list, GL_COMPILE);
        for (Face& face : faces) {
            if (face.edge == -1) {
                has_texcoord = false;
                glLightfv(GL_LIGHT0, GL_AMBIENT, materials[face.normal].ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, materials[face.normal].diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, materials[face.normal].specular);
                if (materials[face.normal].texture != 0)
                    has_texcoord = true,
                    glBindTexture(GL_TEXTURE_2D, materials[face.normal].texture);
                continue;
            }
            (face.normal != -1)?
                glNormal3fv(normals[face.normal]):
                glDisable(GL_LIGHTING);
            if (has_texcoord) {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++) {
                    glTexCoord2fv(texcoords[face.texcoords[i]]);
                    glVertex3fv(vertices[face.vertices[i]]);
                }
                glEnd();
            }
            else {
                glBegin(GL_POLYGON);
                for (int i = 0; i < face.edge; i++) glVertex3fv(vertices[face.vertices[i]]);
                glEnd();
            }
            if (face.normal == -1) glEnable(GL_LIGHTING);
        }
        glEndList();
		// end of the list
		
        printf("Model: %s\n", filename);
        printf("Vertices: %d\n", vertices.size());
        printf("Texcoords: %d\n", texcoords.size());
        printf("Normals: %d\n", normals.size());
        printf("Faces: %d\n", faces.size());
        printf("Materials: %d\n", materials.size());

        sum_x /= vertices.size();
        sum_y /= vertices.size();
        sum_z /= vertices.size();
        pos_x /= vertices.size();
        pos_x = -pos_x;
        pos_y /= vertices.size();
        pos_y = -pos_y;
        pos_z = -sqrt(sum_x * sum_x + sum_y * sum_y + sum_z * sum_z) * 15;

        printf("Pos_X: %f\n", pos_x);
        printf("Pos_Y: %f\n", pos_y);
        printf("Pos_Z: %f\n", pos_z);

        for (Material& material : materials) {
            delete material.ambient;
            delete material.diffuse;
            delete material.specular;
        }

        for (float* f : vertices)delete f;
        for (float* f : texcoords)delete f;
        for (float* f : normals) delete f;
        materials.clear();
        map_material.clear();
        vertices.clear();
        texcoords.clear();
        normals.clear();
        faces.clear();
    }
	
	/*
	* make a "draw" method to be called from the out and display the model
	* by call a list of OpenGL's methods
	* the list is built when load the documents as above
	*/
    void draw() { glCallList(list); }
};

#endif
