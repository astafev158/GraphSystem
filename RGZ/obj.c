#include "obj.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/// ������ ��������� float � ������
/// ���������� ���������� ����������� �������� � ��������� ��������� � v
int read_float(const char*str, float*v) {
	const char*end;
	const char*orig = str; // �������� �������� �������

	// ���������� ��� �������, ���� �� ���������� ������� -, +,., Null, ����� ������ ��� �����
	for (; *str != '-' && *str != '+' && *str != '.' && *str != '\0' && *str != '\n' && (*str < '0' || *str > '9'); ++str);
	if (*str == '\0' || *str == '\n') // ���� ����������� ���� ��� ����� ������
		return -1; // �� ���� ����� �� ��� ��������
	*v = strtof(str, &end); // ������ float
	return end - orig; // ���������� ������� ����� ������� �������� � ����������
}
/// ������ ��������� int � ������
/// ���������� ���������� ����������� �������� � ��������� ��������� � v
int read_int(const char*str, int*v) {
	const char*end;
	const char*orig = str; // �������� �������� �������


		// ���������� ��� ������� �� -, +, ����, ����� ������ ��� �����
	for (; *str != '-' && *str != '+' && *str != '\0' && *str != '\n' && (*str < '0' || *str > '9'); ++str);
	if (*str == '\0' || *str == '\n') // ���� ����������� ���� ��� ����� ������
		return -1; // �� ���� ����� �� ��� ��������
	*v = strtoll(str, &end, 10); // ������ int
	return end - orig; // ���������� ������� ����� ������� �������� � ����������
}
/// ������ ��������� .obj ����� ������� � ������
/// ���������� ���������� ����������� �������� � ��������� ��������� � v
int read_face_vert(const char*str, int*v) {
	const char*orig = str; // �������� �������� �������
	int r = read_int(str, v); // ������ int - ��� �������������� ������ �������
	if (r == -1) // ����  int �� ��� �������� = ��� ������� �������
		return -1;
	str += r; // ���������� ������ ����� ������� int
	// ���������� ����������
	for (; *str == '/' || (*str >= '0' && *str <= '9'); ++str);
	return str - orig; // ���������� ������� ����� ������� �������� � ����������
}

/// ������ .obj ������ �� �����
/// ���������� ��� ������ � ��������� ��������� � pObj
int read_obj(const char*path, obj*pObj) {
	FILE*file;
	int err;
	obj o = { 0, 0, 0, 0 };// �������������� ������ ������
	size_t size;
	char*contents;
	int eol = 0;

	err = fopen_s(&file, path, "r"); // ������� ������� ���� ��� ������
	if (err)
		return err;
	fseek(file, 0, SEEK_END); // �������� ������ �����
	size = ftell(file);
	fseek(file, 0, SEEK_SET); // ����� ��������� �� ������
	contents = calloc(size + 1, 1); // ����������� ����� ���������� �������
	fread(contents, 1, size, file); // ������ ����������� ����� � ����
	fclose(file); // ������� ���� .obj

// ����������� ������ ������
	for (int i = 0; i < size; ++i) {
		if (eol) { // We're in 'skip line' mode
			if (contents[i] == '\n') // ���� ����������� ����� ������
				eol = 0; // Exit eol mode
			continue;// ���������� ��� ������� �� ��� ���
		}
		// ���� ���������� ������� ���������
		if (contents[i] == 'v' && contents[i + 1] == ' ') {
			o.vc++; // ��������� ���������� ������
			eol = 1;
		}
		// ���� ���������� ������� ���������
		if (contents[i] == 'f' && contents[i + 1] == ' ') {
			o.fc++; // ��������� ���������� �������
			eol = 1;
		}
	}
	// �������� �������
	o.f = malloc(sizeof(obj_face)*o.fc);
	o.v = malloc(sizeof(obj_vertex)*o.vc);
	o.fc = 0; // ����� �������� ��� ������������� � �������� ���������
	o.vc = 0;
	eol = 0;
	// ���������� �� ������ �����
	for (int i = 0; i < size; ++i) {
		if (eol) {
			if (contents[i] == '\n')
				eol = 0;
			continue;
		}
		// ���������� ������� ���������
		if (contents[i] == 'v' && contents[i + 1] == ' ') {
			i += 2; // ���������� ����������
			i += read_float(&contents[i], &o.v[o.vc].x); // ������ ��� floats
			i += read_float(&contents[i], &o.v[o.vc].y);
			i += read_float(&contents[i], &o.v[o.vc].z);
			--i; // �������� �� ���� ������, ����� �� ���������� ����� ������
			++o.vc; // ��������� ������� ������
			eol = 1; // ���������� ������� ������
		}
		// ���������� ������� ���������
		else if (contents[i] == 'f' && contents[i + 1] == ' ') {
			i += 2; // ���������� ����������
			int orig = i; // ��������� �������
			int off;
			int t;
			o.f[o.fc].count = 0;
			// ���������� ���������� ������ � �����
			do {
				// ������ ������� ������
				off = read_face_vert(&contents[i], &t);
				if (off != -1)// ���� ������� ����� �������
					o.f[o.fc].count++; // ��������� �������
				i += off;
			} while (off != -1); // ���� �� ������� ����� �������
			i = orig; // ����� ������� ������
			// �������� ������ ������
			o.f[o.fc].v = malloc(sizeof(obj_vertex)*o.f[o.fc].count);
			o.f[o.fc].count = 0;
			// ������ ������� ������
			do {
				// ������ ������� �������
				off = read_face_vert(&contents[i], &t);
				if (off != -1) // ���� ������� ����� �������
					// ��������� ������ �������
					o.f[o.fc].v[o.f[o.fc].count++] = t-1;
				i += off;
			} while (off != -1); // ���� �� ������� ����� �������
			++o.fc; // ��������� ���������� �������
			eol = 1; // ���������� ������� ������
		}
	}
	*pObj = o;
	return 0;
}
/// ����������� ������ �� .obj ������
void free_obj(obj o) {
	for (int i = 0; i < o.fc; ++i) // ��� ������ �������
		free(o.f[i].v); // ���������� ������ �������� ������
	free(o.f); // ������ ��������� �������
	free(o.v); // ��������� ������ ������
}
/// Clone .obj model
obj obj_clone(obj o) {
	obj r;
	r.fc = o.fc; // �������� �������
	r.vc = o.vc;
	r.f = malloc(sizeof(obj_face)*r.fc); // �������� ���������� ������
	r.v = malloc(sizeof(obj_vertex)*r.vc);
	for (int i = 0; i < r.fc; ++i) {
		r.f[i].count = o.f[i].count; // �������� ���������� �������� �������
		r.f[i].v = malloc(sizeof(int)*r.f[i].count); // ������� ������ ��� ��������
		memcpy(r.f[i].v, o.f[i].v, sizeof(int) * r.f[i].count); // �������� �������
	}
	memcpy(r.v, o.v, sizeof(obj_vertex) * r.vc); // �������� �������
	return r;
}
/// ����������, ��������� �� ������ � ������������
int obj_need_triangulation(obj o) {
	for (int i = 0; i < o.fc; ++i)
		if (o.f[i].count != 3)
			return 1;
	return 0;
}
/// ������������ ������ ������
obj obj_triangulate(obj o) {
	obj r;
	r.vc = o.vc;
	r.v = malloc(sizeof(obj_vertex)*r.vc); // ������������ � �������� �������
	memcpy(r.v, o.v, sizeof(obj_vertex)*r.vc);
	r.fc = o.fc;
	for (int i = 0; i < o.fc; ++i)
		r.fc += o.f[i].count - 3; // ������ �������� ���������� ��� ���� ����������� ������
	r.f = malloc(sizeof(obj_face)*r.fc); // �������� ������ ������
	r.fc = 0; // �������� ����� ��� ������������� � �������� ��������
	for (int i = 0; i < o.fc; ++i) {// ��������� ������ ������
		int c = o.f[i].count; // �������� ���������� ������ ����
		if (c == 3) { // ���� ��� ��� �����������
			r.f[r.fc++] = o.f[i]; // �� ��������� ������� ��������. ������ �������� �������
			r.f[r.fc].count = c;
			r.f[r.fc].v = malloc(sizeof(obj_vertex)*c);
			memcpy(r.f[r.fc].v, o.f[i].v, sizeof(obj_vertex)*c);
		}
		else { // ����������������� �������
			int v = 1; // ������� �������� ������������
			while (c >= 3) { // ���� ������� �� �����������
				obj_face f; // ������� ����� ����� ������������
				f.v = malloc(sizeof(int) * 3);
				f.count = 3;
				f.v[0] = o.f[i].v[0]; // ��������� ��� ������� � ������� ��������
				f.v[1] = o.f[i].v[v];
				f.v[2] = o.f[i].v[v + 1];
				r.f[r.fc++] = f; // ��������� ����� �������
				v++; //��������� ��������
				--c; // ��������� ���������� ������
			}
		}
	}
	return r;
}