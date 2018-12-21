#pragma once
#ifndef OBJ_H
#define OBJ_H
#include <GL/glew.h>

/// .obj модель вершин
typedef struct {
	float x, y, z; // Cooridnates
} obj_vertex;
/// .obj model face
typedef struct {
	int count; // Кол-во вершин
	int*v;     // массив индексов вершин
} obj_face;
/// .obj модель
typedef struct {
	int vc;       // Количество вершин
	obj_vertex*v; // Массив вершин
	int fc;       // Количество лицевых вершин
	obj_face*f;   // массив лиц
} obj;

/// чтение модели из obj  файла
/// Вернуть ошибку.Модель храниться в pObj
int read_obj(const char*path, obj*pObj);
/// Освобождает .obj модель памяти
void free_obj(obj o);
/// клонирует .obj модель
obj obj_clone(obj o);
/// Определяет, не является ли .model триангулированной
int obj_need_triangulation(obj o);
/// Уменьшает все грани модели .obj до треугольников
obj obj_triangulate(obj o);

#endif