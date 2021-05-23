#ifndef _PJ_H
#define _PJ_H

	/*!
		\file

		\brief Заголовочный файл с описанием функций для работы с JSON-строками и JSON-объектами

		\date 2021/05/21

		\authors dima201246

		\version 1.0

		\example example.c
	*/

	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <stdio.h>


	/// Набор результатов выполнения
	typedef enum {
		PJ_OK = 0	///< Успешно
		, PJ_ERROR_BAD_PARAMETERS = 1	///< Неверные параметры функции
		, PJ_ERROR_BAD_JSON_STRING = 2	///< Неверный формат JSON-строки
		, PJ_ERROR_BAD_JSON_STRING_NO_CLOSE_SYM = 3	///< В JSON-строке нет закрывающего символа
		, PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_KEY = 4	///< Объект без ключа
		, PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE = 5	///< Объект без значения
		, PJ_ERROR_MEMORY_FAIL = 6	///< Ошибка выделения памяти
		, PJ_ERROR_WRONG_NUMBER = 7	///< Неправильный формат числа
		, PJ_ERROR_MEMORY_WRONG_NUMBER_EXP = 8	///< Неправильный формат экспоненты
		, PJ_ERROR_UNCKNOWN_VALUE_TYPE = 9	///< Неизвестный тип значения
		, PJ_ERROR_BAD_STRING_VALUE = 10	///< Неправильный формат строки
		, PJ_ERROR_NOT_CLOSED_OBJECT_OR_ARRAY = 11	///< Незакрытый объект или массив
		, PJ_ERROR_OBJECT_NOT_FOUND = 12	///< Объект не найден
		, PJ_ERROR_PATH_NOT_CORRECT = 13	///< Неверный путь
		, PJ_ERROR_BAD_INDEX_IN_PATH = 14	///< Неверный индекс в пути
		, PJ_ERROR_WRONG_VALUE_TYPE = 15	///< Неверный тип значения
		, PJ_ERROR_INDEX_OUT_OF_RANGE = 16	///< Индекс вне размера массива
		, PJ_ERROR_CANT_ADD_OBJECT_WITH_KEY = 17	///< Невозможно добавить объект с ключём
		, PJ_ERROR_CANT_ADD_OBJECT_WITHOUT_KEY = 18	///< Невозможно добавить объект без ключа
		, PJ_ERROR_CANT_ADD_OBJECT_INTO_VALUE = 19	///< Невозможно добавить объект в значение
		, PJ_ERROR_WRONG_VALUE_SIZE = 21	///< Неверный размер значения
		, PJ_ERROR_UNCKNOWN = 22	///< Неизвестная ошибка
	} pJErr_t;


	/// Набор типов значений объектов
	typedef enum {
		PJ_VALUE_TYPE_UNCKNOWN = 0	///< Неизвестный
		, PJ_VALUE_TYPE_OBJECT = 1	///< Объект
		, PJ_VALUE_TYPE_ARRAY = 2	///< Массив
		, PJ_VALUE_TYPE_STRING = 3	///< Строка
		, PJ_VALUE_TYPE_NUMBER = 4	///< Число
		, PJ_VALUE_TYPE_NULL = 5	///< NULL
		, PJ_VALUE_TYPE_BOOL = 6	///< Булево значение
	} pJValueType_t;


	/// Структура JSON-объекта
	typedef struct jsonObj_s {
		char *str_key;	///< Ключ
		void *ptr_value;	///< Значение
		pJValueType_t value_type;	///< Тип объекта
		struct jsonObj_s *ptr_next;	///< Указатель на следующий объект
	} pJObj_t;


	/*!
		\defgroup library Функции библиотеки

		\brief Функции обработки ошибок, перегрузка операторов, отладка

		@{
	*/

	/*!
		\brief Получение версии библиотеки PicoJson

		Возвращает указатель на строку вида "PicoJson_VM.m", где 'M' - мажорная версия 'm' - минорная версия

		\return Указатель на строчку с версией
	*/
	const char *pJGetVer();

	/*!
		\brief Вывод дерева JSON-объекта

		Вывод дерева JSON-объекта на стандартный вывод (stdout). Не является функцией сериализации, подходит для отладки.

		\param _ptr_obj_json [in] Указатель на экземпляр объекта
	*/
	void pJShowTree(const pJObj_t *_ptr_obj_json);

	/*!
		\brief Получение строкового описания ошибки

		\param _error [in] Ошибка

		\return Указатель на строку описания
	*/
	const char *pJErrStr(pJErr_t _error);

	/*!
		\brief Перегрузка функций выделения памяти

		Перегрузка необходима для многопоточных приложений.

		\param _func_calloc [in] Указатель на функцию
		\param _func_strndup [in] Указатель на функцию
	*/
	void pJSetFuncRealloc(void *(*_func_calloc)(size_t, size_t), char *(*_func_strndup)(const char *, size_t));

	/*!
		@}
	*/


	/*!
		\defgroup control Управление JSON-объектом

		\brief Инициализация, копирование, очистка JSON-объекта

		@{
	*/

	/*!
		\brief Инициализация экземпляра JSON-объекта

		По сути эта функция просто очищает структуру, можно не использовать,
		если при объявлении переменной явно указать значение структуры.
		Пример:

		\code
			void foo(const char *_str_json)
			{
				pJObj_t obj_json = {0};
			}
		\endcode

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта

		\return Результат выполнения
	*/
	pJErr_t pJInit(pJObj_t *_ptr_obj_json);

	/*!
		\brief Парсинг JSON-строки
		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_json [in] Указатель на JSON-строку
		\return Результат выполнения
	*/
	pJErr_t pJParse(pJObj_t *_ptr_obj_json, const char *_str_json);

	/*!
		\brief Парсинг JSON-строки с явным указанием длины
		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_json [in] Указатель на JSON-строку
		\param _size_str_json [in] Размер JSON-строки
		\return Результат выполнения
	*/
	pJErr_t pJParsen(pJObj_t *_ptr_obj_json, const char *_str_json, size_t _size_str_json);

	/*!
		\brief Копирование объекта

		\param _ptr_obj_json_dest [in,out] Указатель на экземпляр JSON-объекта в который будет производиться копирование
		\param _ptr_obj_json_cpy [in] Указатель на экземпляр JSON-объекта из которого будет производиться копирование

		\warning _ptr_obj_json_dest должен быть проинициализирован функцией pJInit(), копирование ведётся в новою область памяти

		\return Результат выполнения
	*/
	pJErr_t pJDup(pJObj_t *_ptr_obj_json_dest, const pJObj_t *_ptr_obj_json_cpy);


	/*!
		\brief Очистка экземпляра JSON-объекта

		Освобождение памяти и очистка структуры, после выполнения _ptr_obj_json можно использовать без вызова pJInit().

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта

		\return Результат выполнения
	*/
	void pJFree(pJObj_t *_ptr_obj_json);

	/*!
		\brief Сериализация экземпляра JSON-объекта в строку

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта

		\warning Память выделенную под JSON-строку необходимо после использования освободить

		\return Указатель на JSON-строку, при ошибке возвращает NULL
	*/
	char *pJSerialize(pJObj_t *_ptr_obj_json);

	/*!
		@}
	*/


	/*!
		\defgroup getters Получение данных из JSON-объекта

		\brief Функции для получения данных из JSON-объекта, будь то значение или характеристики объекта

		@{
	*/

	/*!
		\brief Получить указатель на экземпляр объекта

		Получение указателя на экземпляр объекта находящимся по пути _str_path.

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к объекту
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на экземпляр объекта, при ошибке возвращает NULL
	*/
	const pJObj_t *pJGetObj(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error);

	/*!
		\brief Получить указатель на строковое значение объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к значению
		\param _str_if_not_found [in] Значение которое будет возвращён, если функция не выполнится успешно
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на строку, при ошибке возвращает _str_if_not_found
	*/
	const char *pJGetStr(const pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_if_not_found, pJErr_t *_ptr_error);

	/*!
		\brief Получить булево значение объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к значению
		\param _flag_if_not_found [in] Значение которое будет возвращён, если функция не выполнится успешно
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на строку, при ошибке возвращает _flag_if_not_found
	*/
	bool pJGetBool(const pJObj_t *_ptr_obj_json, const char *_str_path, bool _flag_if_not_found, pJErr_t *_ptr_error);

	/*!
		\brief Получить числовое значение объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к значению
		\param _num_if_not_found [in] Значение которое будет возвращён, если функция не выполнится успешно
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на строку, при ошибке возвращает _num_if_not_found
	*/
	long long int pJGetLLNum(const pJObj_t *_ptr_obj_json, const char *_str_path, long long int _num_if_not_found, pJErr_t *_ptr_error);

	/*!
		\brief Получить вещественное значение объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к значению
		\param _flt_if_not_found [in] Значение которое будет возвращён, если функция не выполнится успешно
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на строку, при ошибке возвращает _flt_if_not_found
	*/
	float pJGetFloat(const pJObj_t *_ptr_obj_json, const char *_str_path, float _flt_if_not_found, pJErr_t *_ptr_error);

	/*!
		\brief Получить NULL значение объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к значению

		\return Результат выполнения
	*/
	pJErr_t pJGetNull(const pJObj_t *_ptr_obj_json, const char *_str_path);

	/*!
		\brief Получить размер массива

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к массиву
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Размер массива
	*/
	size_t pJGetArrSize(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error);

	/*!
		\brief Получить тип объекта

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к объекту
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Тип объекта
	*/
	pJValueType_t pJGetObjType(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error);

	/*!
		\brief Получить указатель на экземпляр объекта в массиве

		\param _ptr_obj_json [in] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к массиву
		\param _index [in] Индекс объекта в массиве
		\param _ptr_error [out] Указатель на результат выполнения, может быть равен NULL

		\return Указатель на экземпляр объекта, при ошибке возвращает NULL
	*/
	const pJObj_t *pJGetObjInArr(const pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, pJErr_t *_ptr_error);

	/*!
		@}
	*/


	/*! 
		\defgroup setters Создание JSON-объекта

		\brief Функции для создания и добавления данных в JSON-объекта

		@{
	*/

	/*!
		\brief Добавить объект в объект

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению объекта
		\param _str_key [in] Ключ
		\param _ptr_obj_json_cpy [in] (Значение) Указатель на экземпляр вставляемого объекта

		\warning Внутри функции _ptr_obj_json_cpy копируется с помощью pJDup()

		\return Результат выполнения
	*/
	pJErr_t pJAddObjInToObj(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const pJObj_t *_ptr_obj_json_cpy);

	/*!
		\brief Добавить пустой объект

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению объекта
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddObj(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key);

	/*!
		\brief Добавить пустой массив

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddArr(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key);

	/*!
		\brief Добавить строковое значение

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ
		\param _str [in] Указатель на строковое значение

		\return Результат выполнения
	*/
	pJErr_t pJAddStr(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const char *_str);

	/*!
		\brief Добавить строковое значение с указанием её длины

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ
		\param _str [in] Указатель на строковое значение
		\param _size_str [in] Длина строкового значения

		\return Результат выполнения
	*/
	pJErr_t pJAddStrn(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const char *_str, size_t _size_str);

	/*!
		\brief Добавить булево значение

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ
		\param _flag [in] Булево значение

		\return Результат выполнения
	*/
	pJErr_t pJAddBool(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, bool _flag);

	/*!
		\brief Добавить числовое значение

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ
		\param _num [in] Числовое значение

		\return Результат выполнения
	*/
	pJErr_t pJAddLLNum(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, long long int _num);

	/*!
		\brief Добавить вещественное значение

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ
		\param _flt [in] Вещественное значение

		\return Результат выполнения
	*/
	pJErr_t pJAddFlt(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, float _flt);

	/*!
		\brief Добавить NULL значение

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddNull(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key);

	/*!
		\brief Добавить пустой объект в объект или массив в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению объекта
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddObjToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key);

	/*!
		\brief Добавить пустой массив в объект или массив в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddArrToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key);

	/*!
		\brief Добавить строковое значение в или массив объект в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ
		\param _str [in] Указатель на строковое значение

		\return Результат выполнения
	*/
	pJErr_t pJAddStrToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, const char *_str);

	/*!
		\brief Добавить строковое значение или массив с указанием её длины в объект в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ
		\param _str [in] Указатель на строковое значение
		\param _size_str [in] Длина строкового значения

		\return Результат выполнения
	*/
	pJErr_t pJAddStrnToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, const char *_str, size_t _size_str);

	/*!
		\brief Добавить булево значение в объект или массив в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ
		\param _flag [in] Булево значение

		\return Результат выполнения
	*/
	pJErr_t pJAddBoolToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, bool _flag);

	/*!
		\brief Добавить числовое значение в объект или массив в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ
		\param _num [in] Числовое значение

		\return Результат выполнения
	*/
	pJErr_t pJAddLLNumToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, long long int _num);

	/*!
		\brief Добавить вещественное значение или массив в объект в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ
		\param _flt [in] Вещественное значение

		\return Результат выполнения
	*/
	pJErr_t pJAddFltToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, float _flt);

	/*!
		\brief Добавить NULL значение в объект или массив в массиве

		\param _ptr_obj_json [in,out] Указатель на экземпляр JSON-объекта
		\param _str_path [in] Путь к будущему расположению массива
		\param _index [in] Индекс объекта или массива в массиве
		\param _str_key [in] Ключ

		\return Результат выполнения
	*/
	pJErr_t pJAddNullToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key);

	/*!
		@}
	*/

#endif	// _PJ_H
