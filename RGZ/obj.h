#pragma once
#ifndef OBJ_H
#define OBJ_H
#include <GL/glew.h>

/// .obj ������ ������
typedef struct {
	float x, y, z; // Cooridnates
} obj_vertex;
/// .obj model face
typedef struct {
	int count; // ���-�� ������
	int*v;     // ������ �������� ������
} obj_face;
/// .obj ������
typedef struct {
	int vc;       // ���������� ������
	obj_vertex*v; // ������ ������
	int fc;       // ���������� ������� ������
	obj_face*f;   // ������ ���
} obj;

/// ������ ������ �� obj  �����
/// ������� ������.������ ��������� � pObj
int read_obj(const char*path, obj*pObj);
/// ����������� .obj ������ ������
void free_obj(obj o);
/// ��������� .obj ������
obj obj_clone(obj o);
/// ����������, �� �������� �� .model �����������������
int obj_need_triangulation(obj o);
/// ��������� ��� ����� ������ .obj �� �������������
obj obj_triangulate(obj o);

#endif