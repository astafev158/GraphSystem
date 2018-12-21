#include "obj.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/// Читает следующий float в строке
/// Возвращает количество прочитанных символов и сохраняет результат в v
int read_float(const char*str, float*v) {
	const char*end;
	const char*orig = str; // Запомним исходную позицию

	// Пропустить все символы, пока не встретятся символы -, +,., Null, новая строка или цифра
	for (; *str != '-' && *str != '+' && *str != '.' && *str != '\0' && *str != '\n' && (*str < '0' || *str > '9'); ++str);
	if (*str == '\0' || *str == '\n') // если встречается ноль или новая строка
		return -1; // Ни один номер не был прочитан
	*v = strtof(str, &end); // Читаем float
	return end - orig; // Возвращаем разницу между текущей позицией и оригиналом
}
/// Читает следующий int в строке
/// Возвращает количество прочитанных символов и сохраняет результат в v
int read_int(const char*str, int*v) {
	const char*end;
	const char*orig = str; // Запомним исходную позицию


		// Пропускаем все символы до -, +, ноль, новая строка или цифра
	for (; *str != '-' && *str != '+' && *str != '\0' && *str != '\n' && (*str < '0' || *str > '9'); ++str);
	if (*str == '\0' || *str == '\n') // если встречается ноль или новая строка
		return -1; // Ни один номер не был прочитан
	*v = strtoll(str, &end, 10); // читаем int
	return end - orig; // Возвращаем разницу между текущей позицией и оригиналом
}
/// Читает следующую .obj грань вершины в строке
/// Возвращает количество прочитанных символов и сохраняет результат в v
int read_face_vert(const char*str, int*v) {
	const char*orig = str; // Запомним исходную позицию
	int r = read_int(str, v); // Первый int - это действительный индекс вершины
	if (r == -1) // если  int не был прочитан = нет лицевой вершины
		return -1;
	str += r; // Перемещаем строку после первого int
	// пропустить перезапуск
	for (; *str == '/' || (*str >= '0' && *str <= '9'); ++str);
	return str - orig; // Возвращаем разницу между текущей позицией и оригиналом
}

/// Читает .obj модель из файла
/// Возвращает код ошибки и сохраняет результат в pObj
int read_obj(const char*path, obj*pObj) {
	FILE*file;
	int err;
	obj o = { 0, 0, 0, 0 };// Инициализируем пустую модель
	size_t size;
	char*contents;
	int eol = 0;

	err = fopen_s(&file, path, "r"); // Пробуем открыть файл для чтения
	if (err)
		return err;
	fseek(file, 0, SEEK_END); // Получить размер файла
	size = ftell(file);
	fseek(file, 0, SEEK_SET); // Сброс указателя на начало
	contents = calloc(size + 1, 1); // резервируем буфер достаточно большой
	fread(contents, 1, size, file); // Чтение содержимого файла в него
	fclose(file); // Закрыть файл .obj

// Анализируем каждый символ
	for (int i = 0; i < size; ++i) {
		if (eol) { // We're in 'skip line' mode
			if (contents[i] == '\n') // Если встречается новая строка
				eol = 0; // Exit eol mode
			continue;// Пропустить все символы до тех пор
		}
		// Если объявление вершины выполнено
		if (contents[i] == 'v' && contents[i + 1] == ' ') {
			o.vc++; // Увеличить количество вершин
			eol = 1;
		}
		// Если объявление лицевой выполнено
		if (contents[i] == 'f' && contents[i + 1] == ' ') {
			o.fc++; // Увеличить количество лицевых
			eol = 1;
		}
	}
	// Выделить массивы
	o.f = malloc(sizeof(obj_face)*o.fc);
	o.v = malloc(sizeof(obj_vertex)*o.vc);
	o.fc = 0; // Сброс размеров для использования в качестве счетчиков
	o.vc = 0;
	eol = 0;
	// Итерациуем по строке снова
	for (int i = 0; i < size; ++i) {
		if (eol) {
			if (contents[i] == '\n')
				eol = 0;
			continue;
		}
		// Объявление вершины выполнено
		if (contents[i] == 'v' && contents[i + 1] == ' ') {
			i += 2; // Пропустить объявление
			i += read_float(&contents[i], &o.v[o.vc].x); // Читаем три floats
			i += read_float(&contents[i], &o.v[o.vc].y);
			i += read_float(&contents[i], &o.v[o.vc].z);
			--i; // Вернемся на один символ, чтобы не пропустить новую строку
			++o.vc; // Увеличить счетчик вершин
			eol = 1; // Пропустить остаток строки
		}
		// Объявление лицевой выполнено
		else if (contents[i] == 'f' && contents[i + 1] == ' ') {
			i += 2; // Пропустить объявление
			int orig = i; // Запомнить позицию
			int off;
			int t;
			o.f[o.fc].count = 0;
			// Подсчитать количество вершин в грани
			do {
				// Чтение лицевых вершин
				off = read_face_vert(&contents[i], &t);
				if (off != -1)// Если найдена грань вершины
					o.f[o.fc].count++; // Увеличить счетчик
				i += off;
			} while (off != -1); // пока не найдена грань вершины
			i = orig; // Сброс позиции строки
			// Выделить массив вершин
			o.f[o.fc].v = malloc(sizeof(obj_vertex)*o.f[o.fc].count);
			o.f[o.fc].count = 0;
			// Чтение лицевых вершин
			do {
				// Чтение лицевой вершины
				off = read_face_vert(&contents[i], &t);
				if (off != -1) // Если найдена грань вершины
					// Сохраняем индекс вершины
					o.f[o.fc].v[o.f[o.fc].count++] = t-1;
				i += off;
			} while (off != -1); // пока не найдена грань вершины
			++o.fc; // Увеличить количество лицевых
			eol = 1; // Пропустить остаток строки
		}
	}
	*pObj = o;
	return 0;
}
/// освобождает память от .obj модели
void free_obj(obj o) {
	for (int i = 0; i < o.fc; ++i) // Для каждой лицевой
		free(o.f[i].v); // Освободить массив индексов вершин
	free(o.f); // Массив свободных лицевых
	free(o.v); // Свободный массив вершин
}
/// Clone .obj model
obj obj_clone(obj o) {
	obj r;
	r.fc = o.fc; // копируем размеры
	r.vc = o.vc;
	r.f = malloc(sizeof(obj_face)*r.fc); // Выделяем достаточно памяти
	r.v = malloc(sizeof(obj_vertex)*r.vc);
	for (int i = 0; i < r.fc; ++i) {
		r.f[i].count = o.f[i].count; // Копируем количество индексов лицевых
		r.f[i].v = malloc(sizeof(int)*r.f[i].count); // Выделим память для индексов
		memcpy(r.f[i].v, o.f[i].v, sizeof(int) * r.f[i].count); // Копируем индексы
	}
	memcpy(r.v, o.v, sizeof(obj_vertex) * r.vc); // Копируем вершины
	return r;
}
/// Определяем, нуждается ли модель в триангуляции
int obj_need_triangulation(obj o) {
	for (int i = 0; i < o.fc; ++i)
		if (o.f[i].count != 3)
			return 1;
	return 0;
}
/// Триангуляция граней модели
obj obj_triangulate(obj o) {
	obj r;
	r.vc = o.vc;
	r.v = malloc(sizeof(obj_vertex)*r.vc); // Распределяем и копируем вершины
	memcpy(r.v, o.v, sizeof(obj_vertex)*r.vc);
	r.fc = o.fc;
	for (int i = 0; i < o.fc; ++i)
		r.fc += o.f[i].count - 3; // должно добавить достаточно для всех треугольных граней
	r.f = malloc(sizeof(obj_face)*r.fc); // Выделить массив граней
	r.fc = 0; // Сбросить сумму для использования в качестве счетчика
	for (int i = 0; i < o.fc; ++i) {// Заполнить массив граней
		int c = o.f[i].count; // Получить количество вершин лица
		if (c == 3) { // Если это уже треугольник
			r.f[r.fc++] = o.f[i]; // Не требуется никаких действий. Просто копируем лицевые
			r.f[r.fc].count = c;
			r.f[r.fc].v = malloc(sizeof(obj_vertex)*c);
			memcpy(r.f[r.fc].v, o.f[i].v, sizeof(obj_vertex)*c);
		}
		else { // Триангуляционнуем лицевую
			int v = 1; // вершина смещения триангуляции
			while (c >= 3) { // Пока лицевая не треугольник
				obj_face f; // Создать новую грань треугольника
				f.v = malloc(sizeof(int) * 3);
				f.count = 3;
				f.v[0] = o.f[i].v[0]; // Соединяем все вершины с нулевым индексом
				f.v[1] = o.f[i].v[v];
				f.v[2] = o.f[i].v[v + 1];
				r.f[r.fc++] = f; // запомнить новую лицевую
				v++; //Увеличить смещение
				--c; // Уменьшить количество вершин
			}
		}
	}
	return r;
}