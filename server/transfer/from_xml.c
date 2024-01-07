#include <string.h>
#include "from_xml.h"
#include "../database/include/data/data.h"
#include "../database/include/file/tableBlocks.h"
#include "../database/include/file/fileApi.h"
#include "../database/include/util/util.h"


bool stringToBool(const char *str) {
    if (strcmp(str, "\"true\"") == 0 || strcmp(str, "1") == 0) {
        return 1;
    } else if (strcmp(str, "\"false\"") == 0 || strcmp(str, "0") == 0) {
        return 0;
    } else {
        fprintf(stderr, "Error converting string to bool: %s\n", str);
        exit(EXIT_FAILURE);
    }
}

int stringToInt(const char *str) {
    char *endptr;
    long result = strtol(str, &endptr, 10);

    // Проверка на ошибку преобразования
    if (*endptr != '\0' || result > INT_MAX || result < INT_MIN) {
        fprintf(stderr, "Error converting string to int: %s\n", str);
        exit(EXIT_FAILURE);
    }

    int answer = (int) result;
    return answer;
}

double stringToDouble(const char *str) {
    char *endptr;
    double result = strtod(str, &endptr);

    // Проверка на ошибку преобразования
    if (*endptr != '\0') {
        fprintf(stderr, "Error converting string to double: %s\n", str);
        exit(EXIT_FAILURE);
    }

    return result;
}

char *handleInsert(xmlNodePtr root, FILE *file) {
    xmlNodePtr insertValues, field;
    char *tableName;
    FieldValue array[MAX_FIELDS];
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "tableName")) {
            tableName = (char *) xmlNodeGetContent(node);
            printf("tableName: %s\n", tableName);
        } else if (xmlStrEqual(node->name, BAD_CAST "insertValues")) {
            insertValues = node;

            // Перебор дочерних элементов insertValues
            int fieldNumber = -1;
            for (field = insertValues->children; field; field = field->next) {
                if (field->type == XML_ELEMENT_NODE && xmlStrEqual(field->name, BAD_CAST "field")) {
                    fieldNumber++;
                    xmlNodePtr typeNode = xmlFirstElementChild(field);
                    xmlNodePtr valueNode = xmlLastElementChild(field);

                    if (typeNode && valueNode && xmlStrEqual(typeNode->name, BAD_CAST "type")) {
                        if (strcmp(xmlNodeGetContent(typeNode), "INTEGER_LITERAL") == 0) {
                            int32_t *value = malloc(sizeof(int32_t));
                            *value = stringToInt((char *) xmlNodeGetContent(valueNode));
                            FieldValue fieldValue = {value, sizeof(int32_t)};
                            array[fieldNumber] = fieldValue;
                        } else if (strcmp(xmlNodeGetContent(typeNode), "DOUBLE_LITERAL") == 0) {
                            double *value = malloc(sizeof(double));
                            *value = stringToDouble((char *) xmlNodeGetContent(valueNode));
                            FieldValue fieldValue = {value, sizeof(double)};
                            array[fieldNumber] = fieldValue;
                        } else if (strcmp(xmlNodeGetContent(typeNode), "BOOL_LITERAL") == 0) {
                            bool *value = malloc(sizeof(bool));
                            *value = stringToBool((char *) xmlNodeGetContent(valueNode));
                            FieldValue fieldValue = {value, sizeof(bool)};
                            array[fieldNumber] = fieldValue;
                        } else {
                            char *value = malloc(sizeof(char) * strlen((char *) xmlNodeGetContent(valueNode)));
                            strcpy(value, (char *) xmlNodeGetContent(valueNode));
                            FieldValue fieldValue = {value, sizeof(char) * strlen(value)};
                            array[fieldNumber] = fieldValue;
                        }
                        printf("Field Type: %s, Field Value: %s\n", xmlNodeGetContent(typeNode),
                               xmlNodeGetContent(valueNode));
                    }
                }
            }
        }
    }

    EntityRecord entityRecord = {array, NULL};
    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    printEntityRecord(&entityRecord, 5, nameTypeBlocks1);
    insertRecordIntoTable(file, &entityRecord, tableName);
    return "Your insert was successfully completed\n";
}

xmlNodePtr traverseChildren(xmlNodePtr node, int childNumber) {
    xmlNodePtr child = node->children;
    int currentChild = 0;

    while (child != NULL) {
        if (child->type == XML_ELEMENT_NODE) {
            // Обработка дочернего элемента
            if (currentChild == childNumber) {
                // Возвращаем найденный дочерний элемент
                return child;
            }

            currentChild++;
        }

        // Переход к следующему дочернему элементу
        child = child->next;
    }

    // Если цикл завершился и нужный узел не был найден
    printf("Child number %d not found.\n", childNumber);
    return NULL; // Возвращаем NULL, если узел не найден
}

void makeFieldValueFromStr(char *value, FieldValue *fieldValue) {
    char *endptr;

    long longValue = strtol(value, &endptr, 10);
    if (*endptr == '\0') {
        int32_t *intValue = malloc(sizeof(int32_t));
        *intValue = (int) longValue;
        fieldValue->data = intValue;
        fieldValue->dataSize = sizeof(int32_t);
        return;
    }


    double *doubleValue = malloc(sizeof(double));
    *doubleValue = strtod(value, &endptr);
    if (*endptr == '\0') {
        fieldValue->data = doubleValue;
        fieldValue->dataSize = sizeof(double);
        return;
    }

    if (strcmp(value, "\"true\"") == 0 || strcmp(value, "\"false\"") == 0) {
        bool *boolValue = malloc(sizeof(bool));
        *boolValue = strcmp(value, "true") == 0;
        fieldValue->data = boolValue;
        fieldValue->dataSize = sizeof(bool);
        return;
    }
    fieldValue->dataSize = sizeof(char) * strlen(value);
    fieldValue->data = value;
}

PredMass *parseCondition(xmlNodePtr condition) {
    Predicate *predicate = malloc(sizeof(Predicate));
    FieldValue *fieldValue = malloc(sizeof(FieldValue));
    for (xmlNodePtr node = condition->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "leftOp")) {
            predicate->fieldName = (char *) xmlNodeGetContent(node);
        }
        if (xmlStrEqual(node->name, BAD_CAST "operator")) {
            if (strcmp((char *) xmlNodeGetContent(node), ">=") == 0) {
                predicate->comparator = MORE_OR_EQUALS;
            } else if (strcmp((char *) xmlNodeGetContent(node), "<=") == 0) {
                predicate->comparator = LESS_OR_EQUALS;
            } else if (strcmp((char *) xmlNodeGetContent(node), ">") == 0) {
                predicate->comparator = MORE;
            } else if (strcmp((char *) xmlNodeGetContent(node), "<") == 0) {
                predicate->comparator = LESS;
            } else if (strcmp((char *) xmlNodeGetContent(node), "==") == 0) {
                predicate->comparator = EQUALS;
            }
        }
        if (xmlStrEqual(node->name, BAD_CAST "rightOp")) {
            char *endptr;
            char *value = (char *) xmlNodeGetContent(node);
            makeFieldValueFromStr(value, fieldValue);
            predicate->comparableValue = fieldValue;
        }
    }
    PredMass *predMass = malloc(sizeof(PredMass));
    predMass->predicateNumber = 1;
    predMass->predicate = predicate;
    return predMass;
}

PredMass *mulCond(PredMass *predMass1, PredMass *predMass2) {
    int newSize = predMass1->predicateNumber + predMass2->predicateNumber;

    Predicate *mergedArray = (Predicate *) malloc(newSize * sizeof(Predicate));
    if (mergedArray == NULL) {
        return NULL;
    }

    for (int i = 0; i < predMass1->predicateNumber; ++i) {
        mergedArray[i] = predMass1->predicate[i];
    }

    for (int i = 0; i < predMass2->predicateNumber; ++i) {
        mergedArray[predMass1->predicateNumber + i] = predMass2->predicate[i];
    }
    PredMass *predMass = malloc(sizeof(predMass));
    predMass->predicate = mergedArray;
    predMass->predicateNumber = newSize;
    return predMass;
}

PredMass *goDepth(xmlNodePtr node) {
    if (xmlStrEqual(node->name, BAD_CAST "AND") || xmlStrEqual(node->name, BAD_CAST "OR")) {
        return mulCond(goDepth(traverseChildren(node, 0)), goDepth(traverseChildren(node, 1)));
    }
    return parseCondition(node);
}

void printPredMass(PredMass *predMass) {
    for (int i = 0; i < predMass->predicateNumber; i++) {
        printf("Comparator %i\n", predMass->predicate[i].comparator);
        printf("FieldName %s\n", predMass->predicate[i].fieldName);
        printf("DataSize %luu\n\n", predMass->predicate[i].comparableValue->dataSize);
    }
}

char *handleDelete(xmlNodePtr root, FILE *file) {
    char *tableName;
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "tableName")) {
            tableName = (char *) xmlNodeGetContent(node);
        } else if (xmlStrEqual(node->name, BAD_CAST "filter")) {
            PredMass *predMass = goDepth(node->children[0].next);
            printPredMass(predMass);
            deleteRecordFromTable(file, tableName, predMass->predicate, predMass->predicateNumber);
        }
    }
    return "Your Record was successfully deleted";
}

void doUpdateInDataBase(FILE *file, PredMass *predMass, char *updateField, char *tableName, FieldValue *newFieldValue) {
    printf("updateField: %s\n", updateField);
    printf("tableName %s\n", tableName);
    printf("dataSize %lu\n", newFieldValue->dataSize);
    Iterator *iterator = readEntityRecordWithCondition(file, tableName, predMass->predicate, predMass->predicateNumber);
    int fieldNumber;
    TableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    for (int i = 0; i < tableOffsetBlock->fieldsNumber; i++) {
        if (strcmp(tableOffsetBlock->nameTypeBlock[i].fieldName, updateField) == 0) {
            fieldNumber = i;
        }
    }
    while (hasNext(iterator, file)) {
        EntityRecord *entityRecord = next(iterator, file);
        entityRecord
//        EntityRecord *newEntityRecord = malloc();
    }
}

char *handleUpdate(xmlNodePtr root, FILE *file) {
    char *tableName;
    char *updateField;
    FieldValue *fieldValue = malloc(sizeof(FieldValue));
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "tableName")) {
            tableName = (char *) xmlNodeGetContent(node);
        } else if (xmlStrEqual(node->name, BAD_CAST "updateField")) {

            for (xmlNodePtr nodeUpdate = node->children; nodeUpdate; nodeUpdate = nodeUpdate->next) {
                if (xmlStrEqual(nodeUpdate->name, BAD_CAST "updateFieldName")) {
                    updateField = (char *) xmlNodeGetContent(nodeUpdate);
                } else if (xmlStrEqual(nodeUpdate->name, BAD_CAST "updateFieldValue")) {
                    makeFieldValueFromStr((char *) xmlNodeGetContent(nodeUpdate), fieldValue);
                }
            }
        } else if (xmlStrEqual(node->name, BAD_CAST "filter")) {
            PredMass *predMass = goDepth(node->children[0].next);
            printPredMass(predMass);
            doUpdateInDataBase(file, predMass, updateField, tableName, fieldValue);
//            deleteRecordFromTable(file, tableName, predMass->predicate, predMass->predicateNumber);
        }
    }
    return "Your Record was successfully updated";

}

void handleSelect(xmlNodePtr node, FILE *file) {

}

char *from_xml(char *xml, FILE *file) {
    xmlDocPtr doc;
    xmlNodePtr root, node;

    // Инициализация библиотеки libxml2
    LIBXML_TEST_VERSION

    // Парсинг строки в XML-документ
    doc = xmlReadMemory(xml, strlen(xml), "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse the input XML.\n");
        return "Failed to parse the input XML.\n";
    }

    // Получение корневого элемента
    root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        fprintf(stderr, "Empty document.\n");
        xmlFreeDoc(doc);
        return "Empty document.\n";
    }

    // Перебор элементов
    for (node = root->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            if (xmlStrEqual(node->name, BAD_CAST "requestType")) {
                if (strcmp(xmlNodeGetContent(node), "INSERT_QUERY") == 0) return handleInsert(root, file);
                else if (strcmp(xmlNodeGetContent(node), "UPDATE_QUERY") == 0) return handleUpdate(root, file);
                else if (strcmp(xmlNodeGetContent(node), "DELETE_QUERY") == 0) return handleDelete(root, file);
                else if (strcmp(xmlNodeGetContent(node), "SELECT_QUERY") == 0) handleSelect(root, file);
            }
        }
    }

    // Освобождение ресурсов
    xmlFreeDoc(doc);
    xmlCleanupParser();
}
