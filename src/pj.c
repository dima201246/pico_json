#include "include/pj.h"


#define __PJ_VER	"PicoJson_V1.0"


typedef enum {
	PJ_READER_IDLE
	, PJ_READER_READ_OBJECT
	, PJ_READER_READ_ARRAY
} pJReaderState_t;


void *(*__pJCalloc)(size_t _num, size_t _size) = calloc;
char *(*__pJStrndup)(const char *_str, size_t _len) = strndup;


size_t __pJObjDepth(pJObj_t *_ptr_obj_json);
void __pJGenTabs(size_t _num);
void __pJShowTree(const pJObj_t *_ptr_obj_json, size_t _depth);
pJErr_t __pJAddObjValue(pJObj_t *_ptr_obj_json, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value);
size_t __pJSearchCloseSignInJsonStr(const char *_str_json, size_t _size_str_json, char _chr_sign_open, char _chr_sign_close);
const char *__pJSearchSign(const char *_str, size_t _size_str, char _chr_sign);
void __pJFree(pJObj_t *_ptr_obj_json, bool _flag_free);
size_t __pJStrSize(pJObj_t *_ptr_obj_json);
char *__pJSerialize(pJObj_t *_ptr_obj_json, pJValueType_t _type_value, char *_ptr_to_str, bool _flag_first);
bool __pJCorrectIndex(const char *_str);
void __pJCleanEscapeChar(char *_str);
pJErr_t __pJCheckStr(const char *_str, size_t _size_str);
pJErr_t __pJAddValue(pJObj_t *_ptr_obj_json, const char *_str_key, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value);
pJErr_t __pJAddObj(pJObj_t *_ptr_obj_json, const char *_str_key, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value);
pJObj_t *__pJGetObjInArrByIndex(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, pJErr_t *_ptr_error);


void pJSetFuncRealloc(void *(*_func_calloc)(size_t, size_t), char *(*_func_strndup)(const char *, size_t)) {
	__pJCalloc = _func_calloc;
	__pJStrndup = _func_strndup;
}


const char *pJGetVer() {
	return __PJ_VER;
}


pJErr_t pJInit(pJObj_t *_ptr_obj_json) {
	if (_ptr_obj_json == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	memset((void *)_ptr_obj_json, 0, sizeof(pJObj_t));

	return PJ_OK;
}


void pJShowTree(const pJObj_t *_ptr_obj_json) {
	__pJShowTree(_ptr_obj_json, 0);
}


void pJFree(pJObj_t *_ptr_obj_json) {
	__pJFree(_ptr_obj_json, false);
	pJInit(_ptr_obj_json);
}


pJErr_t pJParsen(pJObj_t *_ptr_obj_json, const char *_str_json, size_t _size_str_json) {
	pJReaderState_t state_reader = PJ_READER_IDLE;
	pJErr_t error_json = PJ_OK;
	pJValueType_t type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
	pJObj_t *ptr_obj_json_now = NULL;

	const char *str_start = NULL;
	size_t size_str = 0;

	bool flag_bool_value = false;

	bool flag_found_exp = false;
	bool flag_found_sign = false;
	bool flag_found_comma = false;

	bool flag_str_compleat = false;

	bool flag_want_next_object = false;

	size_t index_char = 0;
	size_t size_str_json_new = 0;
	size_t inde_str_hex_num = 0;

	if ((_ptr_obj_json == NULL) || (_str_json == NULL) || (_size_str_json == 0))
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	while (index_char < _size_str_json)
	{
		// Если это печатный символ
		if ((_str_json[index_char] > 32) && (_str_json[index_char] < 127))
		{
			switch (_str_json[index_char]) {
				case '{': {
					if (state_reader == PJ_READER_READ_OBJECT)
					{
						if (ptr_obj_json_now->str_key == NULL)
						{
							return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_KEY;
						}
					}

					if ((state_reader == PJ_READER_READ_OBJECT) || (state_reader == PJ_READER_READ_ARRAY))
					{
						ptr_obj_json_now->value_type = type_value_found = PJ_VALUE_TYPE_OBJECT;

						size_str_json_new = (__pJSearchCloseSignInJsonStr(&_str_json[index_char], (_size_str_json - index_char), '{', '}') + 1);

						if (size_str_json_new == 0)
						{
							pJFree(_ptr_obj_json);
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 2\n");
							#endif
							return PJ_ERROR_BAD_JSON_STRING;
						}

						error_json = pJParsen(ptr_obj_json_now, &_str_json[index_char], size_str_json_new);

						index_char += size_str_json_new;
						if (error_json != PJ_OK)
						{
							pJFree(_ptr_obj_json);
							return error_json;
						}

						continue;
					}

					if (state_reader == PJ_READER_IDLE)
					{
						if (_ptr_obj_json->ptr_value != NULL)
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 18\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						state_reader = PJ_READER_READ_OBJECT;

						_ptr_obj_json->value_type = PJ_VALUE_TYPE_OBJECT;

						_ptr_obj_json->ptr_value = __pJCalloc(1, sizeof(pJObj_t));

						if (_ptr_obj_json->ptr_value == NULL)
						{
							return PJ_ERROR_MEMORY_FAIL;
						}

						ptr_obj_json_now = (pJObj_t *)_ptr_obj_json->ptr_value;

						#ifdef PJ_DEBUG
						printf("{\n");
						#endif
					}
				} break;

				case '[': {
					if (state_reader == PJ_READER_IDLE)
					{
						if (_ptr_obj_json->ptr_value != NULL)
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 19\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						state_reader = PJ_READER_READ_ARRAY;

						_ptr_obj_json->value_type = PJ_VALUE_TYPE_ARRAY;

						_ptr_obj_json->ptr_value = __pJCalloc(1, sizeof(pJObj_t));

						if (_ptr_obj_json->ptr_value == NULL)
						{
							return PJ_ERROR_MEMORY_FAIL;
						}

						ptr_obj_json_now = (pJObj_t *)_ptr_obj_json->ptr_value;

						#ifdef PJ_DEBUG
						printf("[\n");
						#endif
					}
					else
					{
						if ((state_reader == PJ_READER_READ_OBJECT) || (state_reader == PJ_READER_READ_ARRAY))
						{
							if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
							{
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 20\n");
								#endif
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_JSON_STRING;
							}

							if ((state_reader == PJ_READER_READ_OBJECT) && (ptr_obj_json_now->str_key == NULL))
							{
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 21\n");
								#endif
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_JSON_STRING;
							}

							type_value_found = PJ_VALUE_TYPE_ARRAY;

							size_str_json_new = (__pJSearchCloseSignInJsonStr(&_str_json[index_char], (_size_str_json - index_char), '[', ']') + 1);

							if (size_str_json_new == 0)
							{
								pJFree(_ptr_obj_json);
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 3\n");
								#endif
								return PJ_ERROR_BAD_JSON_STRING;
							}

							error_json = pJParsen(ptr_obj_json_now, &_str_json[index_char], size_str_json_new);

							index_char += size_str_json_new;

							if (error_json != PJ_OK)
							{
								pJFree(_ptr_obj_json);
								return error_json;
							}

							continue;
						}
						else
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 4\n");
							#endif
							return PJ_ERROR_BAD_JSON_STRING;
						}
					}
				} break;

				// Парсер строк
				case '"': {
					if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
					{
						#ifdef PJ_DEBUG
						printf("PJ_ERROR_BAD_JSON_STRING: 5\n");
						#endif
						return PJ_ERROR_BAD_JSON_STRING;
					}

					// Найдено строковое значение
					type_value_found = PJ_VALUE_TYPE_STRING;

					if ((index_char + 1) >= _size_str_json)
					{
						#ifdef PJ_DEBUG
						printf("PJ_ERROR_BAD_JSON_STRING: 6\n");
						#endif
						return PJ_ERROR_BAD_JSON_STRING;
					}

					// Пропускаем открывающуюся кавычку
					index_char++;

					// Получение указателя на начало строки
					str_start = &_str_json[index_char];

					flag_str_compleat = false;

					for (; index_char < _size_str_json; ++index_char)
					{
						if (_str_json[index_char] == '\\')
						{
							if ((index_char + 1) >= _size_str_json)
							{
								pJFree(_ptr_obj_json);
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 7\n");
								#endif
								return PJ_ERROR_BAD_JSON_STRING;
							}

							if ((_str_json[index_char + 1] != '"') && \
								(_str_json[index_char + 1] != '\\') && \
								(_str_json[index_char + 1] != '/') && \
								(_str_json[index_char + 1] != 'b') && \
								(_str_json[index_char + 1] != 'f') && \
								(_str_json[index_char + 1] != 'n') && \
								(_str_json[index_char + 1] != 'r') && \
								(_str_json[index_char + 1] != 't') && \
								(_str_json[index_char + 1] != 'u'))
							{
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_STRING_VALUE;
							}

							// Пропуск символа после знака '\'
							index_char++;

							if (_str_json[index_char] == 'u')
							{
								if ((index_char + 4) >= _size_str_json)
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_BAD_STRING_VALUE;
								}

								// Пропуск символа 'u'
								index_char++;

								for (inde_str_hex_num = 0; inde_str_hex_num < 4; ++inde_str_hex_num)
								{
									if ((_str_json[index_char + inde_str_hex_num] < '0') || (_str_json[index_char + inde_str_hex_num] > '9'))
									{
										if (((_str_json[index_char + inde_str_hex_num] >= 'a') && (_str_json[index_char + inde_str_hex_num] <= 'f')) || \
											((_str_json[index_char + inde_str_hex_num] >= 'A') && (_str_json[index_char + inde_str_hex_num] <= 'F')))
										{
											continue;
										}

										pJFree(_ptr_obj_json);
										return PJ_ERROR_BAD_STRING_VALUE;
									}
								}

								// Пропуск символов после знака 'u'
								index_char += 3;
							}

							continue;
						}

						if (_str_json[index_char] == '"')
						{
							flag_str_compleat = true;
							size_str = (&_str_json[index_char] - str_start);

							break;
						}
					}

					if (flag_str_compleat == false)
					{
						pJFree(_ptr_obj_json);
						return PJ_ERROR_BAD_JSON_STRING_NO_CLOSE_SYM;
					}

					// Пропуск закрывающей кавычки
					index_char++;

					continue;
				} break;

				case ':': {
					if (state_reader == PJ_READER_READ_OBJECT)
					{
						if (type_value_found != PJ_VALUE_TYPE_STRING)
						{
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_KEY;
						}

						if (ptr_obj_json_now->str_key != NULL)
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 8\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						ptr_obj_json_now->str_key = __pJStrndup(str_start, size_str);

						if (ptr_obj_json_now->str_key == NULL)
						{
							pJFree(_ptr_obj_json);
							return PJ_ERROR_MEMORY_FAIL;
						}

						// Очистка "буфера"
						type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
						str_start = NULL;
						size_str = 0;

						#ifdef PJ_DEBUG
						printf("Found key: \"%s\"\n", ptr_obj_json_now->str_key);
						#endif
					}
					else
					{
						#ifdef PJ_DEBUG
						printf("PJ_ERROR_BAD_JSON_STRING: 9\n");
						#endif
						pJFree(_ptr_obj_json);
						return PJ_ERROR_BAD_JSON_STRING;
					}
				} break;

				case ',': {
					if (state_reader == PJ_READER_READ_OBJECT)
					{
						if ((flag_want_next_object == true) && (ptr_obj_json_now->str_key == NULL))
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 22\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						if ((type_value_found == PJ_VALUE_TYPE_UNCKNOWN) && (ptr_obj_json_now->str_key))
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 10\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						if (ptr_obj_json_now->ptr_value == NULL)
						{
							if (type_value_found == PJ_VALUE_TYPE_BOOL)
							{
								error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_BOOL, (const void *)&flag_bool_value, sizeof(bool));
							}
							else
							{
								if (type_value_found == PJ_VALUE_TYPE_NULL)
								{
									error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_NULL, NULL, 0);
								}
								else
								{
									if ((type_value_found == PJ_VALUE_TYPE_NUMBER) || (type_value_found == PJ_VALUE_TYPE_STRING))
									{
										error_json = __pJAddObjValue(ptr_obj_json_now, type_value_found, str_start, size_str);
									}
								}
							}
						}

						// Очистка "буфера"
						type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
						str_start = NULL;
						size_str = 0;

						ptr_obj_json_now->ptr_next = __pJCalloc(1, sizeof(pJObj_t));

						if (ptr_obj_json_now->ptr_next == NULL)
						{
							pJFree(_ptr_obj_json);
							return PJ_ERROR_MEMORY_FAIL;
						}

						ptr_obj_json_now = ptr_obj_json_now->ptr_next;
						flag_want_next_object = true;
					}
					else
					{
						if (state_reader == PJ_READER_READ_ARRAY)
						{
							if (type_value_found == PJ_VALUE_TYPE_UNCKNOWN)
							{
								pJFree(_ptr_obj_json);
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE: 1\n");
								#endif
								return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE;
							}

							if (type_value_found == PJ_VALUE_TYPE_BOOL)
							{
								error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_BOOL, (const void *)&flag_bool_value, sizeof(bool));
							}
							else
							{
								if (type_value_found == PJ_VALUE_TYPE_NULL)
								{
									error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_NULL, NULL, 0);
								}
								else
								{
									if ((type_value_found == PJ_VALUE_TYPE_NUMBER) || (type_value_found == PJ_VALUE_TYPE_STRING))
									{
										error_json = __pJAddObjValue(ptr_obj_json_now, type_value_found, str_start, size_str);
									}
								}
							}

							if (error_json != PJ_OK)
							{
								pJFree(_ptr_obj_json);
								return error_json;
							}

							// Очистка "буфера"
							type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
							str_start = NULL;
							size_str = 0;

							ptr_obj_json_now->ptr_next = __pJCalloc(1, sizeof(pJObj_t));

							if (ptr_obj_json_now->ptr_next == NULL)
							{
								pJFree(_ptr_obj_json);
								return PJ_ERROR_MEMORY_FAIL;
							}

							ptr_obj_json_now = ptr_obj_json_now->ptr_next;
							flag_want_next_object = true;

							#ifdef PJ_DEBUG
							printf(",\n");
							#endif
						}
						else
						{
							pJFree(_ptr_obj_json);
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 11\n");
							#endif
							return PJ_ERROR_BAD_JSON_STRING;
						}
					}
				} break;

				case ']': {
					if (state_reader == PJ_READER_READ_ARRAY)
					{
						if ((flag_want_next_object == true) && (type_value_found == PJ_VALUE_TYPE_UNCKNOWN))
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 23\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						if (type_value_found == PJ_VALUE_TYPE_BOOL)
						{
							error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_BOOL, (const void *)&flag_bool_value, sizeof(bool));
						}
						else
						{
							if (type_value_found == PJ_VALUE_TYPE_NULL)
							{
								error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_NULL, NULL, 0);
							}
							else
							{
								if ((type_value_found == PJ_VALUE_TYPE_NUMBER) || (type_value_found == PJ_VALUE_TYPE_STRING))
								{
									error_json = __pJAddObjValue(ptr_obj_json_now, type_value_found, str_start, size_str);
								}
							}
						}

						if (error_json != PJ_OK)
						{
							pJFree(_ptr_obj_json);
							return error_json;
						}

						// Очистка "буфера"
						type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
						str_start = NULL;
						size_str = 0;

						#ifdef PJ_DEBUG
						printf("]\n");
						#endif

						state_reader = PJ_READER_IDLE;
					}
					else
					{
						pJFree(_ptr_obj_json);
						#ifdef PJ_DEBUG
						printf("PJ_ERROR_BAD_JSON_STRING: 12\n");
						#endif
						return PJ_ERROR_BAD_JSON_STRING;
					}
				} break;

				case '}': {
					if (state_reader == PJ_READER_READ_OBJECT)
					{
						if ((ptr_obj_json_now->str_key == NULL) && (type_value_found != PJ_VALUE_TYPE_UNCKNOWN))
						{
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_KEY;
						}

						if ((flag_want_next_object == true) && (type_value_found == PJ_VALUE_TYPE_UNCKNOWN))
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 24\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						if ((ptr_obj_json_now->str_key != NULL) && (ptr_obj_json_now->ptr_value == NULL) && (type_value_found == PJ_VALUE_TYPE_UNCKNOWN))
						{
							pJFree(_ptr_obj_json);
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE: 3\n");
							#endif
							return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE;
						}

						if (type_value_found == PJ_VALUE_TYPE_BOOL)
						{
							error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_BOOL, (const void *)&flag_bool_value, sizeof(bool));
						}
						else
						{
							if (type_value_found == PJ_VALUE_TYPE_NULL)
							{
								error_json = __pJAddObjValue(ptr_obj_json_now, PJ_VALUE_TYPE_NULL, NULL, 0);
							}
							else
							{
								if ((type_value_found == PJ_VALUE_TYPE_NUMBER) || (type_value_found == PJ_VALUE_TYPE_STRING))
								{
									error_json = __pJAddObjValue(ptr_obj_json_now, type_value_found, str_start, size_str);
								}
							}
						}

						if (error_json != PJ_OK)
						{
							pJFree(_ptr_obj_json);
							return error_json;
						}

						// Очистка "буфера"
						type_value_found = PJ_VALUE_TYPE_UNCKNOWN;
						flag_want_next_object = false;
						str_start = NULL;
						size_str = 0;

						#ifdef PJ_DEBUG
						printf("}\n");
						#endif

						state_reader = PJ_READER_IDLE;
					}
					else
					{
						#ifdef PJ_DEBUG
						printf("PJ_ERROR_BAD_JSON_STRING: 13\n");
						#endif
						pJFree(_ptr_obj_json);
						return PJ_ERROR_BAD_JSON_STRING;
					}
				} break;

				default: {
					// Проверка на начала числа
					if ((_str_json[index_char] == '-') || \
						((_str_json[index_char] >= '0') && (_str_json[index_char] <= '9')))
					{
						if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
						{
							#ifdef PJ_DEBUG
							printf("PJ_ERROR_BAD_JSON_STRING: 14\n");
							#endif
							pJFree(_ptr_obj_json);
							return PJ_ERROR_BAD_JSON_STRING;
						}

						// Найдено числовое значение
						type_value_found = PJ_VALUE_TYPE_NUMBER;

						flag_found_exp = false;
						flag_found_sign = false;
						flag_found_comma = false;

						str_start = &_str_json[index_char];

						for (; index_char < _size_str_json; ++index_char)
						{
							// Проверка нуля на первых позициях
							if (_str_json[index_char] == '0')
							{
								if ((&_str_json[index_char] - str_start) == 0)
								{
									if ((index_char + 1) < _size_str_json)
									{
										if ((_str_json[index_char + 1] >= '0') && (_str_json[index_char + 1] <= '9'))
										{
											pJFree(_ptr_obj_json);
											return PJ_ERROR_WRONG_NUMBER;
										}
									}
								}

								if ((&_str_json[index_char] - str_start) == 1)
								{
									if ((_str_json[index_char - 1] == '-') && ((index_char + 1) < _size_str_json))
									{
										if ((_str_json[index_char + 1] >= '0') && (_str_json[index_char + 1] <= '9'))
										{
											pJFree(_ptr_obj_json);
											return PJ_ERROR_WRONG_NUMBER;
										}
									}
								}
							}

							if (_str_json[index_char] == '-')
							{
								if ((index_char + 1) >= _size_str_json)
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								if ((_str_json[index_char + 1] < '0') || (_str_json[index_char + 1] > '9'))
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								if (flag_found_sign == false)
								{
									flag_found_sign = true;
								}
								else
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								continue;
							}

							if ((_str_json[index_char] == 'E') || _str_json[index_char] == 'e')
							{
								if ((index_char + 1) >= _size_str_json)
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								if (flag_found_exp == false)
								{
									flag_found_exp = true;

									if ((_str_json[index_char + 1] == '+') || _str_json[index_char + 1] == '-')
									{
										if ((index_char + 2) < _size_str_json)
										{
											if ((_str_json[index_char + 2] < '0') || (_str_json[index_char + 2] > '9'))
											{
												pJFree(_ptr_obj_json);
												return PJ_ERROR_MEMORY_WRONG_NUMBER_EXP;
											}
										}
										else
										{
											pJFree(_ptr_obj_json);
											return PJ_ERROR_MEMORY_WRONG_NUMBER_EXP;
										}

										index_char++;
									}
									else
									{
										if ((_str_json[index_char + 1] < '0') || (_str_json[index_char + 1] > '9'))
										{
											pJFree(_ptr_obj_json);
											return PJ_ERROR_MEMORY_WRONG_NUMBER_EXP;
										}
									}
								}

								continue;
							}

							if (_str_json[index_char] == '.')
							{
								if ((index_char + 1) >= _size_str_json)
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								if ((_str_json[index_char + 1] < '0') || (_str_json[index_char + 1] > '9'))
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								if (flag_found_comma == false)
								{
									flag_found_comma = true;
								}
								else
								{
									pJFree(_ptr_obj_json);
									return PJ_ERROR_WRONG_NUMBER;
								}

								continue;
							}

							if ((_str_json[index_char] < '0') || (_str_json[index_char] > '9'))
							{
								size_str = ((&_str_json[index_char - 1] - str_start) + 1);
								break;
							}
						}

						// Если цикл закончился сам, а не через стоп-символ
						if (index_char == _size_str_json)
						{
							size_str = ((&_str_json[index_char - 1] - str_start) + 1);

							if ((_str_json[index_char - 1] < '0') || (_str_json[index_char - 1] > '9'))
							{
								pJFree(_ptr_obj_json);
								return PJ_ERROR_WRONG_NUMBER;
							}
						}

						continue;
					}

					if ((_str_json[index_char] == 't') && ((index_char + 3) < _size_str_json))
					{
						if (memcmp((const void *)&_str_json[index_char], (const void *)"true", 4) == 0)
						{
							if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
							{
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 15\n");
								#endif
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_JSON_STRING;
							}

							// Найдено bool значение
							type_value_found = PJ_VALUE_TYPE_BOOL;

							flag_bool_value = true;

							index_char += 4;

							continue;
						}
					}

					if ((_str_json[index_char] == 'f') && ((index_char + 4) < _size_str_json))
					{
						if (memcmp((const void *)&_str_json[index_char], (const void *)"false", 5) == 0)
						{
							if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
							{
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 16\n");
								#endif
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_JSON_STRING;
							}

							// Найдено bool значение
							type_value_found = PJ_VALUE_TYPE_BOOL;

							flag_bool_value = false;

							index_char += 5;

							continue;
						}
					}

					if ((_str_json[index_char] == 'n') && ((index_char + 3) < _size_str_json))
					{
						if (memcmp((const void *)&_str_json[index_char], (const void *)"null", 4) == 0)
						{
							if (type_value_found != PJ_VALUE_TYPE_UNCKNOWN)
							{
								#ifdef PJ_DEBUG
								printf("PJ_ERROR_BAD_JSON_STRING: 17\n");
								#endif
								pJFree(_ptr_obj_json);
								return PJ_ERROR_BAD_JSON_STRING;
							}

							// Найдено bool значение
							type_value_found = PJ_VALUE_TYPE_NULL;

							index_char += 4;

							continue;
						}
					}

					pJFree(_ptr_obj_json);
					return PJ_ERROR_UNCKNOWN_VALUE_TYPE;
				}
			}
		}
		else
		{
			if ((_str_json[index_char] != ' ') && \
				(_str_json[index_char] != '\t') && \
				(_str_json[index_char] != '\n') && \
				(_str_json[index_char] != '\r'))
			{
				pJFree(_ptr_obj_json);
				return PJ_ERROR_BAD_JSON_STRING;
			}
		}

		++index_char;
	}

	if (_ptr_obj_json->value_type == PJ_VALUE_TYPE_OBJECT)
	{
		if ((_ptr_obj_json->str_key != NULL) && (_ptr_obj_json->ptr_value == NULL))
		{
			pJFree(_ptr_obj_json);
			#ifdef PJ_DEBUG
			printf("PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE: 4\n");
			#endif
			return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE;
		}
	}

	if ((ptr_obj_json_now != NULL) && (type_value_found != PJ_VALUE_TYPE_UNCKNOWN))
	{
		pJFree(_ptr_obj_json);
		return PJ_ERROR_NOT_CLOSED_OBJECT_OR_ARRAY;
	}

	if (state_reader != PJ_READER_IDLE)
	{
		#ifdef PJ_DEBUG
		printf("PJ_ERROR_BAD_JSON_STRING: 25\n");
		#endif
		pJFree(_ptr_obj_json);
		return PJ_ERROR_BAD_JSON_STRING;
	}

	if (_ptr_obj_json->value_type == PJ_VALUE_TYPE_UNCKNOWN)
	{
		if (type_value_found == PJ_VALUE_TYPE_UNCKNOWN)
		{
			pJFree(_ptr_obj_json);
			#ifdef PJ_DEBUG
			printf("PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE: 5\n");
			#endif
			return PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE;
		}

		if (type_value_found == PJ_VALUE_TYPE_BOOL)
		{
			error_json = __pJAddObjValue(_ptr_obj_json, PJ_VALUE_TYPE_BOOL, (const void *)&flag_bool_value, sizeof(bool));
		}
		else
		{
			if (type_value_found == PJ_VALUE_TYPE_NULL)
			{
				error_json = __pJAddObjValue(_ptr_obj_json, PJ_VALUE_TYPE_NULL, NULL, 0);
			}
			else
			{
				if ((type_value_found == PJ_VALUE_TYPE_NUMBER) || (type_value_found == PJ_VALUE_TYPE_STRING))
				{
					error_json = __pJAddObjValue(_ptr_obj_json, type_value_found, str_start, size_str);
				}
			}
		}

		if (error_json != PJ_OK)
		{
			pJFree(_ptr_obj_json);
			return error_json;
		}
	}

	// Удаление Empty-объекта
	if ((ptr_obj_json_now != NULL) && (ptr_obj_json_now->value_type == PJ_VALUE_TYPE_UNCKNOWN))
	{
		pJFree(ptr_obj_json_now);
		free(_ptr_obj_json->ptr_value);
		_ptr_obj_json->ptr_value = NULL;	
	}

	return PJ_OK;
}


pJErr_t pJParse(pJObj_t *_ptr_obj_json, const char *_str_json) {
	if (_str_json == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	return pJParsen(_ptr_obj_json, _str_json, strlen(_str_json));
}


const char *pJErrStr(pJErr_t _error) {
	switch (_error) {
		case PJ_OK: {
			return "Successful";
		} break;

		case PJ_ERROR_BAD_PARAMETERS: {
			return "Bad parameters for function";
		} break;

		case PJ_ERROR_BAD_JSON_STRING: {
			return "Bad JSON-string";
		} break;

		case PJ_ERROR_BAD_JSON_STRING_NO_CLOSE_SYM: {
			return "Not closed symbol in JSON-string";
		} break;

		case PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_KEY: {
			return "JSON-object without key";
		} break;

		case PJ_ERROR_BAD_JSON_OBJECT_WITHOUT_VALUE: {
			return "JSON-object without value";
		} break;

		case PJ_ERROR_MEMORY_FAIL: {
			return "Allocation memory fail";
		} break;

		case PJ_ERROR_WRONG_NUMBER: {
			return "Bad number in JSON-string";
		} break;

		case PJ_ERROR_MEMORY_WRONG_NUMBER_EXP: {
			return "Bad number exp in JSON-string";
		} break;

		case PJ_ERROR_UNCKNOWN_VALUE_TYPE: {
			return "Found unknown value type";
		} break;

		case PJ_ERROR_BAD_STRING_VALUE: {
			return "Bad string value";
		} break;

		case PJ_ERROR_NOT_CLOSED_OBJECT_OR_ARRAY: {
			return "Not found close symbol for object or array";
		} break;

		case PJ_ERROR_OBJECT_NOT_FOUND: {
			return "Obj not found";
		} break;

		case PJ_ERROR_PATH_NOT_CORRECT: {
			return "Path not correct";
		} break;

		case PJ_ERROR_BAD_INDEX_IN_PATH: {
			return "Bad index in path";
		} break;

		case PJ_ERROR_WRONG_VALUE_TYPE: {
			return "Wrong value type";
		} break;

		case PJ_ERROR_INDEX_OUT_OF_RANGE: {
			return "Index out of range";
		} break;

		case PJ_ERROR_CANT_ADD_OBJECT_WITH_KEY: {
			return "Can't add object with key";
		} break;

		case PJ_ERROR_CANT_ADD_OBJECT_WITHOUT_KEY: {
			return "Can't add object without key";
		} break;

		case PJ_ERROR_CANT_ADD_OBJECT_INTO_VALUE: {
			return "Can't add object into value";
		} break;

		case PJ_ERROR_WRONG_VALUE_SIZE: {
			return "Wrong value size";
		} break;

		default: {
			return "Unknown error";
		}
	}

	return NULL;
}


char *pJSerialize(pJObj_t *_ptr_obj_json) {
	return __pJSerialize(_ptr_obj_json, PJ_VALUE_TYPE_UNCKNOWN, NULL, true);
}


size_t pJGetArrSize(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;
	size_t count_element = 0;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return 0;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_ARRAY)
		{
			ptr_obj_json = (const pJObj_t *)ptr_obj_json->ptr_value;

			for (; ptr_obj_json != NULL; ptr_obj_json = ptr_obj_json->ptr_next, ++count_element);
		}
		else
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_WRONG_VALUE_TYPE;
			}
		}
	}

	return count_element;
}


const char *pJGetStr(const pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_if_not_found, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return _str_if_not_found;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_STRING)
		{
			return (const char *)ptr_obj_json->ptr_value;
		}
		else
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_WRONG_VALUE_TYPE;
			}
		}
	}

	return _str_if_not_found;
}


bool pJGetBool(const pJObj_t *_ptr_obj_json, const char *_str_path, bool _flag_if_not_found, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return _flag_if_not_found;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_BOOL)
		{
			return *((bool *)ptr_obj_json->ptr_value);
		}
		else
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_WRONG_VALUE_TYPE;
			}
		}
	}

	return _flag_if_not_found;
}


long long int pJGetLLNum(const pJObj_t *_ptr_obj_json, const char *_str_path, long long int _num_if_not_found, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return _num_if_not_found;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_NUMBER)
		{
			return atoll((const char *)ptr_obj_json->ptr_value);
		}
		else
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_WRONG_VALUE_TYPE;
			}
		}
	}

	return _num_if_not_found;
}


float pJGetFloat(const pJObj_t *_ptr_obj_json, const char *_str_path, float _flt_if_not_found, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return _flt_if_not_found;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_NUMBER)
		{
			return atof((const char *)ptr_obj_json->ptr_value);
		}
		else
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_WRONG_VALUE_TYPE;
			}
		}
	}

	return _flt_if_not_found;
}


pJErr_t pJGetNull(const pJObj_t *_ptr_obj_json, const char *_str_path) {
	pJErr_t error_json = PJ_OK;
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, &error_json);

	if (ptr_obj_json != NULL)
	{
		if (ptr_obj_json->value_type == PJ_VALUE_TYPE_NULL)
		{
			return PJ_OK;
		}
		else
		{
			return PJ_ERROR_WRONG_VALUE_TYPE;
		}
	}

	return error_json;
}


pJErr_t pJAddObj(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_OBJECT, NULL, 0);
}


pJErr_t pJAddArr(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_ARRAY, NULL, 0);
}


pJErr_t pJAddStrn(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const char *_str, size_t _size_str) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	if (_str == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_STRING, _str, _size_str);
}


pJErr_t pJAddStr(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const char *_str) {
	if (_str == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	return pJAddStrn(_ptr_obj_json, _str_path, _str_key, _str, strlen(_str));
}


pJErr_t pJAddNull(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_NULL, NULL, 0);
}


pJErr_t pJAddBool(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, bool _flag) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_BOOL, (const void *)&_flag, sizeof(bool));
}


pJErr_t pJAddLLNum(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, long long int _num) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;
	char str_buf[128] = {0};

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	snprintf(str_buf, 128, "%lld", _num);

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_NUMBER, (const void *)str_buf, strlen(str_buf));	
}


pJErr_t pJAddFlt(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, float _flt) {
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;
	char str_buf[128] = {0};

	ptr_obj_json = (pJObj_t *)pJGetObj((const pJObj_t *)_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	snprintf(str_buf, 128, "%f", _flt);

	return __pJAddObj(ptr_obj_json, _str_key, PJ_VALUE_TYPE_NUMBER, (const void *)str_buf, strlen(str_buf));	
}


pJErr_t pJDup(pJObj_t *_ptr_obj_json_dest, const pJObj_t *_ptr_obj_json_cpy) {
	pJObj_t *ptr_obj_json_next = NULL;
	pJErr_t error_json = PJ_OK;

	if ((_ptr_obj_json_dest == NULL) || (_ptr_obj_json_cpy == NULL))
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	for (;; _ptr_obj_json_cpy = ptr_obj_json_next)
	{
		ptr_obj_json_next = _ptr_obj_json_cpy->ptr_next;

		// Копируем ключ
		if (_ptr_obj_json_cpy->str_key != NULL)
		{
			_ptr_obj_json_dest->str_key = __pJStrndup(_ptr_obj_json_cpy->str_key, strlen(_ptr_obj_json_cpy->str_key));

			if (_ptr_obj_json_dest->str_key == NULL)
			{
				pJFree(_ptr_obj_json_dest);
				return PJ_ERROR_MEMORY_FAIL;
			}
		}

		// Копируем тип объекта
		_ptr_obj_json_dest->value_type = _ptr_obj_json_cpy->value_type;

		switch (_ptr_obj_json_cpy->value_type) {
			case PJ_VALUE_TYPE_OBJECT:
			case PJ_VALUE_TYPE_ARRAY: {
				if (_ptr_obj_json_cpy->ptr_value == NULL)
				{
					break;
				}

				_ptr_obj_json_dest->ptr_value = __pJCalloc(1, sizeof(pJObj_t));

				if (_ptr_obj_json_dest->ptr_value == NULL)
				{
					pJFree(_ptr_obj_json_dest);
					return PJ_ERROR_MEMORY_FAIL;
				}

				error_json = pJDup((pJObj_t *)_ptr_obj_json_dest->ptr_value, (const pJObj_t *)_ptr_obj_json_cpy->ptr_value);

				if (error_json != PJ_OK)
				{
					pJFree(_ptr_obj_json_dest);
					return error_json;
				}
			} break;

			case PJ_VALUE_TYPE_STRING:
			case PJ_VALUE_TYPE_NUMBER: {
				_ptr_obj_json_dest->ptr_value = (char *)__pJStrndup((const char *)_ptr_obj_json_cpy->ptr_value, strlen((const char *)_ptr_obj_json_cpy->ptr_value));

				if (_ptr_obj_json_dest->ptr_value == NULL)
				{
					pJFree(_ptr_obj_json_dest);
					return PJ_ERROR_MEMORY_FAIL;
				}
			} break;

			case PJ_VALUE_TYPE_BOOL: {
				_ptr_obj_json_dest->ptr_value = __pJCalloc(1, sizeof(bool));

				if (_ptr_obj_json_dest->ptr_value == NULL)
				{
					pJFree(_ptr_obj_json_dest);
					return PJ_ERROR_MEMORY_FAIL;
				}

				memcpy(_ptr_obj_json_dest->ptr_value, (const void *)_ptr_obj_json_cpy->ptr_value, sizeof(bool));
			} break;

			case PJ_VALUE_TYPE_UNCKNOWN:
			case PJ_VALUE_TYPE_NULL: {

			} break;
		}

		if (ptr_obj_json_next == NULL)
		{
			break;
		}
		else
		{
			_ptr_obj_json_dest->ptr_next = (pJObj_t *)__pJCalloc(1, sizeof(pJObj_t));

			if (_ptr_obj_json_dest->ptr_next == NULL)
			{
				pJFree(_ptr_obj_json_dest);
				return PJ_ERROR_MEMORY_FAIL;
			}

			_ptr_obj_json_dest = _ptr_obj_json_dest->ptr_next;
		}
	}

	return error_json;
}


const pJObj_t *pJGetObjInArr(const pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, pJErr_t *_ptr_error) {
	size_t index_obj_in_array = 0;
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return NULL;
	}

	if (_ptr_error != NULL)
	{
		*_ptr_error = PJ_OK;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json == NULL)
	{
		return NULL;
	}

	if (ptr_obj_json->value_type != PJ_VALUE_TYPE_ARRAY)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_OBJECT_NOT_FOUND;
		}

		return NULL;
	}

	if (pJGetArrSize(ptr_obj_json, NULL, NULL) <= _index)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_INDEX_OUT_OF_RANGE;
		}

		return NULL;
	}

	ptr_obj_json = ptr_obj_json->ptr_value;

	for (; ptr_obj_json != NULL; ptr_obj_json = ptr_obj_json->ptr_next, ++index_obj_in_array)
	{
		if (index_obj_in_array == _index)
		{
			return ptr_obj_json;
		}
	}

	return NULL;
}


const pJObj_t *pJGetObj(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error) {
	const char *str_start = _str_path;
	const char *str_end = _str_path;

	const char *str_index_start = NULL;
	const char *str_index_end = NULL;

	size_t size_str_path = 0;
	size_t index_obj_in_array_search = 0;
	size_t index_obj_in_array = 0;

	char str_buf_index[64] = {0};
	char str_buf[256] = {0};

	bool flag_search_array = false;
	bool flag_found_object = false;

	const pJObj_t *ptr_obj_json = _ptr_obj_json;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return NULL;
	}

	if (_ptr_error != NULL)
	{
		*_ptr_error = PJ_OK;
	}

	if (_str_path == NULL)
	{
		return _ptr_obj_json;
	}
	else
	{
		size_str_path = strlen(_str_path);
	}

	for (; (size_t)((str_end - _str_path) + 1) < size_str_path; str_start = str_end + 1)
	{
		flag_search_array = false;

		str_end = __pJSearchSign(str_start, (size_str_path - (str_end - _str_path)) + 1, ':');

		if (str_end == NULL)
		{
			if (_ptr_error != NULL)
			{
				*_ptr_error = PJ_ERROR_PATH_NOT_CORRECT;
			}

			return NULL;
		}

		if (*str_start == '[')
		{
			str_index_start = (str_start + 1);
			str_index_end = __pJSearchSign(str_index_start, (size_str_path - (str_index_start - _str_path)), ']');

			if (str_index_end == NULL)
			{
				if (_ptr_error != NULL)
				{
					*_ptr_error = PJ_ERROR_BAD_INDEX_IN_PATH;
				}

				return NULL;
			}

			// Копирование индекса объекта в буфер
			strncpy(str_buf_index, str_index_start, (str_index_end - str_index_start));
			str_buf_index[(str_index_end - str_index_start)] = '\0';

			if (__pJCorrectIndex(str_buf_index) == false)
			{
				if (_ptr_error != NULL)
				{
					*_ptr_error = PJ_ERROR_BAD_INDEX_IN_PATH;
				}

				return NULL;
			}

			index_obj_in_array_search = atoll(str_buf_index);

			flag_search_array = true;
		}
		else
		{
			strncpy(str_buf, str_start, (str_end - str_start));
			str_buf[(str_end - str_start)] = '\0';

			__pJCleanEscapeChar(str_buf);
		}

		if (flag_search_array == false)
		{
			flag_found_object = false;

			if (ptr_obj_json == NULL)
			{
				if (_ptr_error != NULL)
				{
					*_ptr_error = PJ_ERROR_OBJECT_NOT_FOUND;
				}

				return NULL;
			}

			// Если наткнулись на объект - сразу войти в него 
			if (ptr_obj_json->value_type == PJ_VALUE_TYPE_OBJECT)
			{
				ptr_obj_json = ptr_obj_json->ptr_value;
			}

			while (ptr_obj_json != NULL)
			{
				if ((ptr_obj_json->str_key != NULL) && (strcmp(ptr_obj_json->str_key, str_buf) == 0))
				{
					flag_found_object = true;

					break;
				}

				ptr_obj_json = ptr_obj_json->ptr_next;
			}
		}
		else
		{
			if ((ptr_obj_json == NULL) || (ptr_obj_json->value_type != PJ_VALUE_TYPE_ARRAY))
			{
				if (_ptr_error != NULL)
				{
					*_ptr_error = PJ_ERROR_OBJECT_NOT_FOUND;
				}

				return NULL;
			}

			flag_found_object = false;

			if (pJGetArrSize(ptr_obj_json, NULL, NULL) <= index_obj_in_array_search)
			{
				if (_ptr_error != NULL)
				{
					*_ptr_error = PJ_ERROR_INDEX_OUT_OF_RANGE;
				}

				return NULL;
			}

			// Вход в массив
			ptr_obj_json = ptr_obj_json->ptr_value;

			for (index_obj_in_array = 0; ptr_obj_json != NULL; ptr_obj_json = ptr_obj_json->ptr_next, ++index_obj_in_array)
			{
				if (index_obj_in_array == index_obj_in_array_search)
				{
					flag_found_object = true;

					break;
				}
			}
		}
	}

	if (flag_found_object == false)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_OBJECT_NOT_FOUND;
		}

		return NULL;
	}

	return ptr_obj_json;
}


pJValueType_t pJGetObjType(const pJObj_t *_ptr_obj_json, const char *_str_path, pJErr_t *_ptr_error) {
	const pJObj_t *ptr_obj_json = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return PJ_VALUE_TYPE_UNCKNOWN;
	}

	ptr_obj_json = pJGetObj(_ptr_obj_json, _str_path, _ptr_error);

	if (ptr_obj_json != NULL)
	{
		return ptr_obj_json->value_type;
	}

	return PJ_VALUE_TYPE_UNCKNOWN;
}



pJErr_t pJAddObjToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddObj(ptr_obj_json_obj_in_array, NULL, _str_key);
}


pJErr_t pJAddArrToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddArr(ptr_obj_json_obj_in_array, NULL, _str_key);
}


pJErr_t pJAddStrnToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, const char *_str, size_t _size_str) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	if (_str == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddStrn(ptr_obj_json_obj_in_array, NULL, _str_key, _str, _size_str);
}


pJErr_t pJAddStrToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, const char *_str) {
	if (_str == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	return pJAddStrnToArrObj(_ptr_obj_json, _str_path, _index, _str_key, _str, strlen(_str));
}


pJErr_t pJAddBoolToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, bool _flag) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddBool(ptr_obj_json_obj_in_array, NULL, _str_key, _flag);
}


pJErr_t pJAddLLNumToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, long long int _num) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddLLNum(ptr_obj_json_obj_in_array, NULL, _str_key, _num);
}


pJErr_t pJAddFltToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key, float _flt) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddFlt(ptr_obj_json_obj_in_array, NULL, _str_key, _flt);
}


pJErr_t pJAddNullToArrObj(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, const char *_str_key) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;
	pJErr_t error_json = PJ_OK;

	ptr_obj_json_obj_in_array = __pJGetObjInArrByIndex(_ptr_obj_json, _str_path, _index, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	return pJAddNull(ptr_obj_json_obj_in_array, NULL, _str_key);
}


size_t __pJObjDepth(pJObj_t *_ptr_obj_json) {
	size_t obj_depth = 0;

	if (_ptr_obj_json == NULL)
	{
		return 0;
	}

	for (;; _ptr_obj_json = _ptr_obj_json->ptr_next)
	{
		obj_depth++;

		if (_ptr_obj_json->ptr_next == NULL)
		{
			break;
		}
	}

	return obj_depth;
}


void __pJGenTabs(size_t _num) {
	for (;_num != 0; --_num)
	{
		printf("\t");
	}
}


void __pJShowTree(const pJObj_t *_ptr_obj_json, size_t _depth) {
	if (_ptr_obj_json == NULL)
	{
		return;
	}

	for (;; _ptr_obj_json = _ptr_obj_json->ptr_next)
	{
		if (_ptr_obj_json->str_key != NULL)
		{
			__pJGenTabs(_depth);
			printf("Key: \"%s\"\n", _ptr_obj_json->str_key);
		}

		switch (_ptr_obj_json->value_type) {
			case PJ_VALUE_TYPE_ARRAY: {
				__pJGenTabs(_depth);
				printf("Arr: [\n");
				__pJShowTree(_ptr_obj_json->ptr_value, (_depth + 1));
				__pJGenTabs(_depth);
				printf("]\n\n");
			}
			break;

			case PJ_VALUE_TYPE_OBJECT: {
				__pJGenTabs(_depth);
				printf("Obj: {\n");
				__pJShowTree(_ptr_obj_json->ptr_value, (_depth + 1));
				__pJGenTabs(_depth);
				printf("}\n\n");
			}
			break;

			case PJ_VALUE_TYPE_BOOL: {
				__pJGenTabs(_depth);
				printf("Bool: %s\n", (*(bool *)_ptr_obj_json->ptr_value == true ? "true" : "false"));

				if (_ptr_obj_json->ptr_next != NULL)
				{
					printf("\n");
				}
			}
			break;

			case PJ_VALUE_TYPE_NULL: {
				__pJGenTabs(_depth);
				printf("NULL (wow, it's is nothing!)\n");

				if (_ptr_obj_json->ptr_next != NULL)
				{
					printf("\n");
				}
			}
			break;

			case PJ_VALUE_TYPE_NUMBER: {
				__pJGenTabs(_depth);
				printf("Number: %s\n", (const char *)_ptr_obj_json->ptr_value);

				if (_ptr_obj_json->ptr_next != NULL)
				{
					printf("\n");
				}
			}
			break;

			case PJ_VALUE_TYPE_STRING: {
				__pJGenTabs(_depth);
				printf("String: \"%s\"\n", (const char *)_ptr_obj_json->ptr_value);

				if (_ptr_obj_json->ptr_next != NULL)
				{
					printf("\n");
				}
			}
			break;

			case PJ_VALUE_TYPE_UNCKNOWN: {
				__pJGenTabs(_depth);

				printf("Empty\n");

				if (_ptr_obj_json->ptr_next != NULL)
				{
					printf("\n");
				}
			} break;
		}

		if (_ptr_obj_json->ptr_next == NULL)
		{
			break;
		}
	}
}


pJErr_t __pJAddObjValue(pJObj_t *_ptr_obj_json, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value) {
	switch (_type_value) {
		case PJ_VALUE_TYPE_STRING: {
			_ptr_obj_json->value_type = PJ_VALUE_TYPE_STRING;
			_ptr_obj_json->ptr_value = (void *)__pJStrndup((const char *)_ptr_value, _size_value);

			#ifdef PJ_DEBUG
			printf("Found string value: \"%s\"\n", (char *)_ptr_obj_json->ptr_value);
			#endif
		} break;

		case PJ_VALUE_TYPE_NUMBER: {
			_ptr_obj_json->value_type = PJ_VALUE_TYPE_NUMBER;
			_ptr_obj_json->ptr_value = (void *)__pJStrndup((const char *)_ptr_value, _size_value);

			#ifdef PJ_DEBUG
			printf("Found number value: %s\n", (char *)_ptr_obj_json->ptr_value);
			#endif
		} break;

		case PJ_VALUE_TYPE_NULL: {
			_ptr_obj_json->value_type = PJ_VALUE_TYPE_NULL;

			#ifdef PJ_DEBUG
			printf("Found null value\n");
			#endif
		} break;

		case PJ_VALUE_TYPE_BOOL: {
			_ptr_obj_json->value_type = PJ_VALUE_TYPE_BOOL;
			_ptr_obj_json->ptr_value = (void *)__pJCalloc(1, sizeof(bool));
			*((bool *)_ptr_obj_json->ptr_value) = *((const bool *)_ptr_value);

			#ifdef PJ_DEBUG
			printf("Found bool value: %s\n", (*((bool *)_ptr_obj_json->ptr_value) == true ? "true" : "false"));
			#endif
		} break;

		case PJ_VALUE_TYPE_UNCKNOWN:
		case PJ_VALUE_TYPE_OBJECT:
		case PJ_VALUE_TYPE_ARRAY:
		default: {

		}
	}

	if ((_ptr_obj_json->ptr_value == NULL) && (_type_value != PJ_VALUE_TYPE_NULL))
	{
		return PJ_ERROR_MEMORY_FAIL;
	}

	return PJ_OK;
}


size_t __pJSearchCloseSignInJsonStr(const char *_str_json, size_t _size_str_json, char _chr_sign_open, char _chr_sign_close) {
	bool flag_inside_string = false;
	size_t index_char = 0;
	size_t count_open_char = 0;

	for (index_char = 0; index_char < _size_str_json; ++index_char)
	{
		if (_str_json[index_char] == '"')
		{
			if (_str_json[index_char - 1] != '\\')
			{
				flag_inside_string = !flag_inside_string;
			}
		}

		if (_str_json[index_char] == _chr_sign_open)
		{
			count_open_char++;
		}

		if (_str_json[index_char] == _chr_sign_close)
		{
			if (count_open_char == 1)
			{
				if (flag_inside_string == false)
				{
					return index_char;
				}
			}
			else
			{
				count_open_char--;
			}
		}
	}

	return 0;
}


const char *__pJSearchSign(const char *_str, size_t _size_str, char _chr_sign) {
	size_t index_char = 0;

	for (index_char = 0; index_char < _size_str; ++index_char)
	{
		if ((_str[index_char] == _chr_sign) || (_str[index_char] == '\0'))
		{
			if ((index_char != 0) && (_str[index_char - 1] == '\\'))
			{
				continue;
			}

			return &_str[index_char];
		}
	}

	return NULL;
}


void __pJFree(pJObj_t *_ptr_obj_json, bool _flag_free) {
	pJObj_t *ptr_obj_json_next = NULL;

	if (_ptr_obj_json == NULL)
	{
		return;
	}

	for (;; _ptr_obj_json = ptr_obj_json_next)
	{
		ptr_obj_json_next = _ptr_obj_json->ptr_next;

		free((void *)_ptr_obj_json->str_key);

		_ptr_obj_json->str_key = NULL;

		switch (_ptr_obj_json->value_type) {
			case PJ_VALUE_TYPE_OBJECT:
			case PJ_VALUE_TYPE_ARRAY: {
				__pJFree(_ptr_obj_json->ptr_value, true);
				_ptr_obj_json->ptr_value = NULL;
			} break;

			case PJ_VALUE_TYPE_UNCKNOWN:
			case PJ_VALUE_TYPE_STRING:
			case PJ_VALUE_TYPE_NUMBER:
			case PJ_VALUE_TYPE_NULL:
			case PJ_VALUE_TYPE_BOOL: {
				free(_ptr_obj_json->ptr_value);
				_ptr_obj_json->ptr_value = NULL;
			} break;

		}

		if (_flag_free == true)
		{
			free(_ptr_obj_json);
		}

		if (ptr_obj_json_next == NULL)
		{
			break;
		}
	}
}


size_t __pJStrSize(pJObj_t *_ptr_obj_json) {
	size_t size_str = 0;
	pJObj_t *ptr_obj_json_next = NULL;

	if (_ptr_obj_json == NULL)
	{
		return 0;
	}

	for (;; _ptr_obj_json = ptr_obj_json_next)
	{
		ptr_obj_json_next = _ptr_obj_json->ptr_next;

		if (_ptr_obj_json->str_key != NULL)
		{
			size_str += 5 + strlen(_ptr_obj_json->str_key);
		}

		switch (_ptr_obj_json->value_type) {
			case PJ_VALUE_TYPE_OBJECT:
			case PJ_VALUE_TYPE_ARRAY: {
				size_str += 4 + __pJStrSize(_ptr_obj_json->ptr_value);
			} break;

			case PJ_VALUE_TYPE_STRING: {
				size_str += 3 + strlen((const char *)_ptr_obj_json->ptr_value);
			} break;

			case PJ_VALUE_TYPE_NUMBER: {
				size_str += 1 + strlen((const char *)_ptr_obj_json->ptr_value);
			} break;

			case PJ_VALUE_TYPE_NULL: {
				size_str += 5;
			} break;

			case PJ_VALUE_TYPE_BOOL: {
				size_str += 1 + (*((bool *)_ptr_obj_json->ptr_value) == true ? 4 : 5);
			} break;

			case PJ_VALUE_TYPE_UNCKNOWN:
			{

			} break;
		}

		if (ptr_obj_json_next == NULL)
		{
			break;
		}

		size_str += 2;
	}

	return size_str;
}


char *__pJSerialize(pJObj_t *_ptr_obj_json, pJValueType_t _type_value, char *_ptr_to_str, bool _flag_first) {
	size_t size_json_str = 0;

	if (_ptr_obj_json == NULL)
	{
		return NULL;
	}

	if (_type_value == PJ_VALUE_TYPE_UNCKNOWN)
	{
		size_json_str = __pJStrSize(_ptr_obj_json);

		if (size_json_str == 0)
		{
			return NULL;
		}

		_ptr_to_str = (char *)__pJCalloc(size_json_str, sizeof(char));
	}

	for (;; _ptr_obj_json = _ptr_obj_json->ptr_next)
	{
		if (_ptr_obj_json->str_key != NULL)
		{
			strcat(_ptr_to_str, "\"");
			strcat(_ptr_to_str, _ptr_obj_json->str_key);
			strcat(_ptr_to_str, "\" : ");
		}

		switch (_ptr_obj_json->value_type) {
			case PJ_VALUE_TYPE_ARRAY: {
				strcat(_ptr_to_str, "[ ");
				__pJSerialize((pJObj_t *)_ptr_obj_json->ptr_value, PJ_VALUE_TYPE_ARRAY, _ptr_to_str, false);
				strcat(_ptr_to_str, "]");

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			}
			break;

			case PJ_VALUE_TYPE_OBJECT: {
				strcat(_ptr_to_str, "{ ");
				__pJSerialize((pJObj_t *)_ptr_obj_json->ptr_value, PJ_VALUE_TYPE_OBJECT, _ptr_to_str, false);
				strcat(_ptr_to_str, "}");

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			}
			break;

			case PJ_VALUE_TYPE_BOOL: {
				strcat(_ptr_to_str, (*(bool *)_ptr_obj_json->ptr_value == true ? "true" : "false"));

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			}
			break;

			case PJ_VALUE_TYPE_NULL: {
				strcat(_ptr_to_str, "null");

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			}
			break;

			case PJ_VALUE_TYPE_NUMBER: {
				strcat(_ptr_to_str, (const char *)_ptr_obj_json->ptr_value);

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			}
			break;

			case PJ_VALUE_TYPE_STRING: {
				strcat(_ptr_to_str, "\"");
				strcat(_ptr_to_str, (const char *)_ptr_obj_json->ptr_value);
				strcat(_ptr_to_str, "\"");

				if (_flag_first != true)
				{
					strcat(_ptr_to_str, " ");
				}
			} break;

			case PJ_VALUE_TYPE_UNCKNOWN: {

			} break;
		}

		if (((_type_value == PJ_VALUE_TYPE_UNCKNOWN) ||\
			(_type_value == PJ_VALUE_TYPE_ARRAY) ||\
			(_type_value == PJ_VALUE_TYPE_OBJECT)) &&\
			(_ptr_obj_json->ptr_next != NULL))
		{
			strcat(_ptr_to_str, ", ");
		}

		if (_ptr_obj_json->ptr_next == NULL)
		{
			break;
		}
	}

	return _ptr_to_str;
}


void __pJCleanEscapeChar(char *_str) {
	size_t size_str = strlen(_str);
	size_t index_char = 0;
	size_t index_char_shift = 0;

	for (;index_char < size_str;)
	{
		if (_str[index_char] == '\\')
		{
			if ((index_char + 1) < size_str)
			{
				if (_str[index_char + 1] == '\\')
				{
					index_char++;
				}
			}

			for (index_char_shift = index_char; index_char_shift < size_str; ++index_char_shift)
			{
				_str[index_char_shift] = _str[index_char_shift + 1]; 
			}

			size_str--;
			continue;
		}

		++index_char;
	}
}

bool __pJCorrectIndex(const char *_str) {
	size_t index_char = 0;
	size_t size_str = strlen(_str);

	for (index_char = 0; index_char < size_str; ++index_char)
	{
		if ((_str[index_char] < '0') || (_str[index_char] > '9'))
		{
			return false;
		}
	}

	return true;
}


pJErr_t __pJCheckStr(const char *_str, size_t _size_str) {
	size_t index_char = 0;
	size_t inde_str_hex_num = 0;

	if (_str == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	for (; index_char < _size_str; ++index_char)
	{
		if (_str[index_char] == '\\')
		{
			if ((index_char + 1) >= _size_str)
			{
				return PJ_ERROR_BAD_STRING_VALUE;
			}

			if ((_str[index_char + 1] != '"') && \
				(_str[index_char + 1] != '\\') && \
				(_str[index_char + 1] != '/') && \
				(_str[index_char + 1] != 'b') && \
				(_str[index_char + 1] != 'f') && \
				(_str[index_char + 1] != 'n') && \
				(_str[index_char + 1] != 'r') && \
				(_str[index_char + 1] != 't') && \
				(_str[index_char + 1] != 'u'))
			{
				return PJ_ERROR_BAD_STRING_VALUE;
			}

			// Пропуск символа после знака '\'
			index_char++;

			if (_str[index_char] == 'u')
			{
				if ((index_char + 4) >= _size_str)
				{
					return PJ_ERROR_BAD_STRING_VALUE;
				}

				// Пропуск символа 'u'
				index_char++;

				for (inde_str_hex_num = 0; inde_str_hex_num < 4; ++inde_str_hex_num)
				{
					if ((_str[index_char + inde_str_hex_num] < '0') || (_str[index_char + inde_str_hex_num] > '9'))
					{
						if (((_str[index_char + inde_str_hex_num] >= 'a') && (_str[index_char + inde_str_hex_num] <= 'f')) || \
							((_str[index_char + inde_str_hex_num] >= 'A') && (_str[index_char + inde_str_hex_num] <= 'F')))
						{
							continue;
						}

						return PJ_ERROR_BAD_STRING_VALUE;
					}
				}

				// Пропуск символов после знака 'u'
				index_char += 3;
			}

			continue;
		}

		if (_str[index_char] == '"')
		{
			return PJ_ERROR_BAD_STRING_VALUE;
		}
	}

	return PJ_OK;
}


pJErr_t __pJAddValue(pJObj_t *_ptr_obj_json, const char *_str_key, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value) {
	pJErr_t error_json = PJ_OK;

	if (_ptr_obj_json == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	switch (_type_value) {
		case PJ_VALUE_TYPE_UNCKNOWN: {
			return PJ_ERROR_UNCKNOWN_VALUE_TYPE;
		} break;

		case PJ_VALUE_TYPE_OBJECT:
		case PJ_VALUE_TYPE_ARRAY: {
			if (_str_key != NULL)
			{
				_ptr_obj_json->str_key = __pJStrndup(_str_key, strlen(_str_key));

				if (_ptr_obj_json->str_key == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}
			}

			_ptr_obj_json->value_type = _type_value;

			return PJ_OK;
		} break;

		case PJ_VALUE_TYPE_NUMBER:
		case PJ_VALUE_TYPE_STRING: {
			if (_str_key != NULL)
			{
				_ptr_obj_json->str_key = __pJStrndup(_str_key, strlen(_str_key));

				if (_ptr_obj_json->str_key == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}
			}

			error_json = __pJCheckStr((const char *)_ptr_value, _size_value);

			if (error_json == PJ_OK)
			{
				_ptr_obj_json->ptr_value = __pJStrndup((const char *)_ptr_value, _size_value);

				if (_ptr_obj_json->ptr_value == NULL)
				{
					pJFree(_ptr_obj_json);
					return PJ_ERROR_MEMORY_FAIL;
				}

				_ptr_obj_json->value_type = _type_value;

				return PJ_OK;
			}
			else
			{
				return error_json;
			}
		} break;

		case PJ_VALUE_TYPE_NULL: {
			if (_str_key != NULL)
			{
				_ptr_obj_json->str_key = __pJStrndup(_str_key, strlen(_str_key));

				if (_ptr_obj_json->str_key == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}
			}

			_ptr_obj_json->value_type = PJ_VALUE_TYPE_NULL;

			return PJ_OK;
		} break;

		case PJ_VALUE_TYPE_BOOL: {
			if (_str_key != NULL)
			{
				_ptr_obj_json->str_key = __pJStrndup(_str_key, strlen(_str_key));

				if (_ptr_obj_json->str_key == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}
			}

			if (_size_value == sizeof(bool))
			{
				_ptr_obj_json->ptr_value = __pJCalloc(1, sizeof(bool));

				if (_ptr_obj_json->ptr_value == NULL)
				{
					pJFree(_ptr_obj_json);
					return PJ_ERROR_MEMORY_FAIL;
				}

				memcpy(_ptr_obj_json->ptr_value, _ptr_value, sizeof(bool));

				_ptr_obj_json->value_type = PJ_VALUE_TYPE_BOOL;

				return PJ_OK;
			}
			else
			{
				pJFree(_ptr_obj_json);
				return PJ_ERROR_WRONG_VALUE_SIZE;
			}
		} break;

		default: {
			return PJ_ERROR_UNCKNOWN_VALUE_TYPE;
		}
	}

	return PJ_ERROR_UNCKNOWN;
}


pJErr_t __pJAddObj(pJObj_t *_ptr_obj_json, const char *_str_key, pJValueType_t _type_value, const void *_ptr_value, size_t _size_value) {
	if (_ptr_obj_json == NULL)
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	switch (_ptr_obj_json->value_type) {
		case PJ_VALUE_TYPE_UNCKNOWN: {
			if (_str_key != NULL)
			{
				return PJ_ERROR_CANT_ADD_OBJECT_WITH_KEY;
			}

			return __pJAddValue(_ptr_obj_json, _str_key, _type_value, _ptr_value, _size_value);
		} break;

		case PJ_VALUE_TYPE_OBJECT:
		case PJ_VALUE_TYPE_ARRAY: {
			if (((_ptr_obj_json->value_type == PJ_VALUE_TYPE_ARRAY) && (_str_key != NULL)) || \
				((_ptr_obj_json->value_type == PJ_VALUE_TYPE_OBJECT) && (_str_key == NULL)))
			{
				if (_ptr_obj_json->value_type == PJ_VALUE_TYPE_ARRAY)
				{
					return PJ_ERROR_CANT_ADD_OBJECT_WITH_KEY;
				}
				else
				{
					return PJ_ERROR_CANT_ADD_OBJECT_WITHOUT_KEY;
				}
			}

			if (_ptr_obj_json->ptr_value != NULL)
			{
				_ptr_obj_json = (pJObj_t *)_ptr_obj_json->ptr_value;

				for (; _ptr_obj_json->ptr_next != NULL; _ptr_obj_json = _ptr_obj_json->ptr_next);

				_ptr_obj_json->ptr_next = __pJCalloc(1, sizeof(pJObj_t));

				if (_ptr_obj_json->ptr_next == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}

				_ptr_obj_json = _ptr_obj_json->ptr_next;
			}
			else
			{
				_ptr_obj_json->ptr_value = __pJCalloc(1, sizeof(pJObj_t));

				if (_ptr_obj_json->ptr_value == NULL)
				{
					return PJ_ERROR_MEMORY_FAIL;
				}

				_ptr_obj_json = (pJObj_t *)_ptr_obj_json->ptr_value;
			}


			return __pJAddValue(_ptr_obj_json, _str_key, _type_value, _ptr_value, _size_value);
		} break;

		case PJ_VALUE_TYPE_BOOL:
		case PJ_VALUE_TYPE_NULL:
		case PJ_VALUE_TYPE_NUMBER:
		case PJ_VALUE_TYPE_STRING: {
			return PJ_ERROR_CANT_ADD_OBJECT_INTO_VALUE;
		} break;
	}

	return PJ_ERROR_UNCKNOWN;
}


// Получение объекта из массива по индексу
pJObj_t *__pJGetObjInArrByIndex(pJObj_t *_ptr_obj_json, const char *_str_path, size_t _index, pJErr_t *_ptr_error) {
	pJObj_t *ptr_obj_json_obj_in_array = NULL;

	if (_ptr_obj_json == NULL)
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_BAD_PARAMETERS;
		}

		return NULL;
	}

	if (_ptr_error != NULL)
	{
		*_ptr_error = PJ_OK;
	}

	ptr_obj_json_obj_in_array = (pJObj_t *)pJGetObjInArr((const pJObj_t *)_ptr_obj_json, _str_path, _index, _ptr_error);

	if (ptr_obj_json_obj_in_array == NULL)
	{
		return NULL;
	}

	if ((ptr_obj_json_obj_in_array->value_type != PJ_VALUE_TYPE_OBJECT) && (ptr_obj_json_obj_in_array->value_type != PJ_VALUE_TYPE_ARRAY))
	{
		if (_ptr_error != NULL)
		{
			*_ptr_error = PJ_ERROR_CANT_ADD_OBJECT_INTO_VALUE;
		}
	}

	return ptr_obj_json_obj_in_array;
}


pJErr_t pJAddObjInToObj(pJObj_t *_ptr_obj_json, const char *_str_path, const char *_str_key, const pJObj_t *_ptr_obj_json_cpy) {
	pJObj_t obj_json = {0};
	pJObj_t *ptr_obj_json = NULL;
	pJErr_t error_json = PJ_OK;

	if ((_ptr_obj_json == NULL) || (_ptr_obj_json == NULL))
	{
		return PJ_ERROR_BAD_PARAMETERS;
	}

	ptr_obj_json = (pJObj_t *)pJGetObj(_ptr_obj_json, _str_path, &error_json);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	if ((ptr_obj_json->value_type !=  PJ_VALUE_TYPE_OBJECT) && (ptr_obj_json->value_type !=  PJ_VALUE_TYPE_ARRAY))
	{
		return PJ_ERROR_CANT_ADD_OBJECT_INTO_VALUE;
	}

	if ((ptr_obj_json->value_type == PJ_VALUE_TYPE_OBJECT) && (_str_key == NULL))
	{
		return PJ_ERROR_CANT_ADD_OBJECT_WITHOUT_KEY;
	}

	if ((ptr_obj_json->value_type == PJ_VALUE_TYPE_ARRAY) && (_str_key != NULL))
	{
		return PJ_ERROR_CANT_ADD_OBJECT_WITH_KEY;
	}

	error_json = pJDup(&obj_json, _ptr_obj_json_cpy);

	if (error_json != PJ_OK)
	{
		return error_json;
	}

	if (ptr_obj_json->ptr_value == NULL)
	{
		ptr_obj_json->ptr_value = __pJCalloc(1, sizeof(pJObj_t));

		if (ptr_obj_json->ptr_value == NULL)
		{
			pJFree(&obj_json);
			return PJ_ERROR_MEMORY_FAIL;
		}

		ptr_obj_json = (pJObj_t *)ptr_obj_json->ptr_value;
	}
	else
	{
		ptr_obj_json = (pJObj_t *)ptr_obj_json->ptr_value;

		for (; ptr_obj_json->ptr_next != NULL; ptr_obj_json = ptr_obj_json->ptr_next);

		ptr_obj_json->ptr_next = (pJObj_t *)__pJCalloc(1, sizeof(pJObj_t));

		if (ptr_obj_json->ptr_next == NULL)
		{
			pJFree(&obj_json);
			return PJ_ERROR_MEMORY_FAIL;
		}

		ptr_obj_json = ptr_obj_json->ptr_next;
	}

	memcpy((void *)ptr_obj_json, (const void *)&obj_json, sizeof(pJObj_t));

	free((void *)ptr_obj_json->str_key);

	if (_str_key != NULL)
	{
		ptr_obj_json->str_key = __pJStrndup(_str_key, strlen(_str_key));

		if (ptr_obj_json->str_key == NULL)
		{
			pJFree(ptr_obj_json);
			return PJ_ERROR_MEMORY_FAIL;
		}
	}

	return PJ_OK;
}
