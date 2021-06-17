#include "include/pj.h"

/*!
	\file

	\brief Файл с примером использования всех видов функций

	\date 2021/05/21

	\authors dima201246

	\version 1.0
*/

// This example code unpack and generate this JSON-tring
const char *Str_json_test = \
"{ " \
	"\"string_value\" : \"string0\" , " \
	"\"number_value\" : 19 , " \
	"\"bool_value\" : true , " \
	"\"null_value\" : null , "\
	"\"array_value\" : [ \"1\" , 2 , false , null , [ \"3\" , 4 , true , null , [ ] , { } ] , { \"key\" : \"value0\" } ] , " \
	"\"object_value\": { " \
		"\"string_value\" : \"string1\" , " \
		"\"number_value\" : 19 , " \
		"\"bool_value\" : true , " \
		"\"null_value\" : null , " \
		"\"array:value[2]_\" : [ \"1\" , 2 , false , null , [ \"3\" , 4.2 , true , null , [ ] , { } ] , { \"key\" : \"value1\" } ] , " \
		"\"array_inc\" : [ 1 , 2 , 3 , 4 , 5 ] , " \
		"\"object_value\" : { \"key\" : \"value2\" } , " \
		"\"empty_object_value\" : { } , " \
		"\"empty_array_value\" : [ ] " \
	"} " \
"}";


/// [getting_example]
void unpack() {
	pJObj_t obj_json = {0};
	const pJObj_t *ptr_obj_json_array = NULL;
	const pJObj_t *ptr_obj_json_array_element = NULL;
	pJErr_t error_json = PJ_OK;
	pJValueType_t type_json_value = PJ_VALUE_TYPE_UNCKNOWN;
	const char *str = NULL;
	long long int num = 0;
	bool flag = false;
	size_t size_array = 0;
	size_t index_object = 0;
	float flt = 0.0;

	printf("\n\nUnpack:\n\n");

	pJInit(&obj_json);

	error_json = pJParse(&obj_json, Str_json_test);

	if (error_json != PJ_OK)
	{
		fprintf(stderr, "Error parse: %s\n", pJErrStr(error_json));
		return;
	}

	// Получение строки
	{
		str = pJGetStr(&obj_json, "string_value", NULL, &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("String value: %s\n", str);
		}
	}

	// Получение числа
	{
		num = pJGetLLNum(&obj_json, "number_value", 0, &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("Number value: %lld\n", num);
		}
	}

	// Получение bool
	{
		flag = pJGetBool(&obj_json, "bool_value", false, &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("Bool value: %s\n", (flag == true ? "true" : "false"));
		}
	}

	// Получение NULL
	{
		error_json = pJGetNull(&obj_json, "null_value");

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("Null value\n");
		}
	}

	// Получение размера массива
	{
		size_array = pJGetArrSize(&obj_json, "array_value", &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("Arr size: %ld\n", size_array);
		}
	}

	printf("\nIn array:\n");

	// Итерация по элементам массива
	{
		// Получение объекта массива
		ptr_obj_json_array = pJGetObj(&obj_json, "array_value", &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}

		for (index_object = 0; index_object < size_array; ++index_object)
		{
			ptr_obj_json_array_element = pJGetObjInArr(ptr_obj_json_array, NULL, index_object, &error_json);

			if (error_json != PJ_OK)
			{
				fprintf(stderr, "Error get element: %s\n", pJErrStr(error_json));
				continue;
			}

			type_json_value = pJGetObjType(ptr_obj_json_array_element, NULL, &error_json);

			if (error_json != PJ_OK)
			{
				fprintf(stderr, "Error get value type: %s\n", pJErrStr(error_json));
				continue;
			}

			switch (type_json_value)
			{
				case PJ_VALUE_TYPE_OBJECT: {
					printf("It's object\n");
				} break;

				case PJ_VALUE_TYPE_ARRAY: {
					printf("It's array\n");
				} break;

				case PJ_VALUE_TYPE_STRING: {
					str = pJGetStr(ptr_obj_json_array_element, NULL, NULL, &error_json);

					if (error_json != PJ_OK)
					{
						fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
					}
					else
					{
						printf("String value: %s\n", str);
					}
				} break;

				case PJ_VALUE_TYPE_NUMBER: {
					num = pJGetLLNum(ptr_obj_json_array_element, NULL, 0, &error_json);

					if (error_json != PJ_OK)
					{
						fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
					}
					else
					{
						printf("Number value: %lld\n", num);
					}
				} break;

				case PJ_VALUE_TYPE_NULL: {
					error_json = pJGetNull(ptr_obj_json_array_element, NULL);

					if (error_json != PJ_OK)
					{
						fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
					}
					else
					{
						printf("Null value\n");
					}
				} break;

				case PJ_VALUE_TYPE_BOOL: {
					flag = pJGetBool(ptr_obj_json_array_element, NULL, true, &error_json);

					if (error_json != PJ_OK)
					{
						fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
					}
					else
					{
						printf("Bool value: %s\n", (flag == true ? "true" : "false"));
					}
				} break;

				case PJ_VALUE_TYPE_UNCKNOWN:
				default: {

				}
			}
		}
	}

	printf("\nAccess by path:\n");

	// Доступ к объекту по пути
	{
		flt = pJGetFlt(&obj_json, "object_value:array\\:value[2]_:[4]:[1]", -0.1, &error_json);

		if (error_json != PJ_OK)
		{
			fprintf(stderr, "Error unpack: %s\n", pJErrStr(error_json));
		}
		else
		{
			printf("Float value: %f\n", flt);
		}
	}

	pJFree(&obj_json);
}
/// [getting_example]


void create() {
	pJObj_t obj_json = {0};
	pJErr_t error_json = PJ_OK;
	char *str_json = NULL;
	size_t index = 0;

	printf("\n\nCreate:\n\n");

	pJInit(&obj_json);

	// Создание объекта
	{
		error_json = pJAddObj(&obj_json, NULL, NULL);

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}
	}

	// Добавление строки
	{
		error_json = pJAddStr(&obj_json, NULL, "string_value", "string0");

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}
	}

	// Добавление числа
	{
		error_json = pJAddLLNum(&obj_json, NULL, "number_value", 19);

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}
	}

	// Добавление bool
	{
		error_json = pJAddBool(&obj_json, NULL, "bool_value", true);

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}
	}

	// Добавление null
	{
		error_json = pJAddNull(&obj_json, NULL, "null_value");

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}
	}

	// Добавление и заполнение массива
	{
		error_json = pJAddArr(&obj_json, NULL, "array_value");

		if (error_json != PJ_OK)
		{
			printf("Error: %s\n", pJErrStr(error_json));
		}

		pJAddStr(&obj_json, "array_value", NULL, "1");
		pJAddLLNum(&obj_json, "array_value", NULL, 2);
		pJAddBool(&obj_json, "array_value", NULL, false);
		pJAddNull(&obj_json, "array_value", NULL);

		pJAddArr(&obj_json, "array_value", NULL);

		pJAddStr(&obj_json, "array_value:[4]", NULL, "3");
		pJAddLLNumToArrObj(&obj_json, "array_value", 4, NULL, 4);
		pJAddBool(&obj_json, "array_value:[4]", NULL, true);
		pJAddNull(&obj_json, "array_value:[4]", NULL);
		pJAddArr(&obj_json, "array_value:[4]", NULL);
		pJAddObj(&obj_json, "array_value:[4]", NULL);

		pJAddObj(&obj_json, "array_value", NULL);
		pJAddStrToArrObj(&obj_json, "array_value", 5, "key", "value0");
	}

	// Добавление и заполнение объекта
	{
		pJAddObj(&obj_json, NULL, "object_value");

		pJAddStr(&obj_json, "object_value", "string_value", "1");
		pJAddLLNum(&obj_json, "object_value", "number_value", 19);
		pJAddBool(&obj_json, "object_value", "bool_value", true);
		pJAddNull(&obj_json, "object_value", "null_value");

		// Добавление объекта в объект
		{
			pJObj_t obj_json_tmp = {0};

			pJInit(&obj_json_tmp);

			pJAddArr(&obj_json_tmp, NULL, NULL);

			pJAddStr(&obj_json_tmp, NULL, NULL, "1");
			pJAddLLNum(&obj_json_tmp, NULL, NULL, 2);
			pJAddBool(&obj_json_tmp, NULL, NULL, false);
			pJAddNull(&obj_json_tmp, NULL, NULL);

			pJAddArr(&obj_json_tmp, NULL, NULL);

			pJAddStr(&obj_json_tmp, "[4]", NULL, "3");
			pJAddFltToArrObj(&obj_json_tmp, NULL, 4, NULL, 4.2);
			pJAddBool(&obj_json_tmp, "[4]", NULL, true);
			pJAddNull(&obj_json_tmp, "[4]", NULL);
			pJAddArr(&obj_json_tmp, "[4]", NULL);
			pJAddObj(&obj_json_tmp, "[4]", NULL);

			pJAddObj(&obj_json_tmp, NULL, NULL);
			pJAddStrToArrObj(&obj_json_tmp, NULL, 5, "key", "value1");

			pJAddObjInToObj(&obj_json, "object_value", "array:value[2]_\\", &obj_json_tmp);

			pJFree(&obj_json_tmp);
		}

		{
			pJAddArr(&obj_json, "object_value", "array_inc");

			for (index = 1; index < 6; ++index)
			{
				pJAddLLNum(&obj_json, "object_value:array_inc", NULL, index);
			}
		}

		{
			pJAddObj(&obj_json, "object_value", "object_value");
			pJAddStr(&obj_json, "object_value:object_value", "key", "value2");
		}

		pJAddObj(&obj_json, "object_value", "empty_object_value");
		pJAddArr(&obj_json, "object_value", "empty_array_value");
	}

	pJShowTree(&obj_json);

	str_json = pJSerialize(&obj_json);

	if (str_json == NULL)
	{
		fprintf(stderr, "Failed serialize JSON-object\n");
	}
	else
	{
		printf("\nSerialized JSON-object: %s\n", str_json);
	}

	free((void *)str_json);
	pJFree(&obj_json);
}


int main() {
	unpack();
	create();
	return 0;
}
