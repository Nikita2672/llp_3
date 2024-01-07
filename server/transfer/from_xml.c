#include <string.h>
#include "from_xml.h"
#include "../database/include/data/data.h"
#include "../database/include/file/tableBlocks.h"
#include "../database/include/file/fileApi.h"
#include "../database/include/util/util.h"
#include "../net/socket_wrapper.h"

int validateXmlAgainstSchemaFile(const char *xmlString, const char *schemaFilePath) {
    LIBXML_TEST_VERSION

    // Создание XML-документа из строки
    xmlDocPtr doc = xmlReadMemory(xmlString, strlen(xmlString), "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse the input XML.\n");
        return -1; // Ошибка при парсинге XML
    }

    // Создание XML-схемы из файла
    xmlSchemaParserCtxtPtr parserCtxt = xmlSchemaNewParserCtxt(schemaFilePath);
    if (parserCtxt == NULL) {
        fprintf(stderr, "Failed to parse the input XML schema.\n");
        xmlFreeDoc(doc);
        return -2; // Ошибка при парсинге XML схемы
    }

    xmlSchemaPtr schema = xmlSchemaParse(parserCtxt);
    if (schema == NULL) {
        fprintf(stderr, "Failed to parse the input XML schema.\n");
        xmlSchemaFreeParserCtxt(parserCtxt);
        xmlFreeDoc(doc);
        return -2; // Ошибка при парсинге XML схемы
    }

    // Создание контекста валидации
    xmlSchemaValidCtxtPtr validCtxt = xmlSchemaNewValidCtxt(schema);
    if (validCtxt == NULL) {
        fprintf(stderr, "Failed to create a validation context.\n");
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parserCtxt);
        xmlFreeDoc(doc);
        return -3; // Ошибка при создании контекста валидации
    }

    // Валидация XML по схеме
    int isValid = xmlSchemaValidateDoc(validCtxt, doc);

    // Освобождение ресурсов
    xmlSchemaFreeValidCtxt(validCtxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parserCtxt);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return isValid; // Возвращаем 0, если XML соответствует схеме, и отрицательное число в противном случае
}

char *create_xml_document(const char *message_content) {
    xmlDocPtr doc = NULL;           // XML-документ
    xmlNodePtr root = NULL;         // Корневой элемент
    xmlNodePtr messageNode = NULL;  // Элемент Message

    LIBXML_TEST_VERSION


    doc = xmlNewDoc(BAD_CAST "1.0");
    if (doc == NULL) {
        fprintf(stderr, "Failed to create XML document.\n");
        return NULL;
    }


    root = xmlNewNode(NULL, BAD_CAST "Root");
    xmlDocSetRootElement(doc, root);


    messageNode = xmlNewNode(NULL, BAD_CAST "Message");
    xmlNodeSetContent(messageNode, BAD_CAST message_content);
    xmlAddChild(root, messageNode);

    xmlChar *xmlString = NULL;
    int xmlStringSize = 0;
    xmlDocDumpFormatMemory(doc, &xmlString, &xmlStringSize, 1);


    xmlFreeDoc(doc);
    xmlCleanupParser();

    printf("\nSchema validation: %d\n", validateXmlAgainstSchemaFile(xmlString, SCHEMA_RESPONSE_FILE));
    return xmlString;
}

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

void *handleInsert(xmlNodePtr root, FILE *file, int client_socket) {
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
    insertRecordIntoTable(file, &entityRecord, tableName);
    send_data(client_socket, create_xml_document("Your insert was successfully completed\n"));
    send_data(client_socket, create_xml_document("End"));
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

void *handleDelete(xmlNodePtr root, FILE *file, int client_socket) {
    char *tableName;
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "tableName")) {
            tableName = (char *) xmlNodeGetContent(node);
        } else if (xmlStrEqual(node->name, BAD_CAST "filter")) {
            PredMass *predMass = goDepth(node->children[0].next);
            deleteRecordFromTable(file, tableName, predMass->predicate, predMass->predicateNumber);
        }
    }
    send_data(client_socket, create_xml_document("Your entity successfully deleted"));
    send_data(client_socket, create_xml_document("End"));
}

void doUpdateInDataBase(FILE *file, PredMass *predMass, char *updateField, char *tableName, FieldValue *newFieldValue) {
    printf("updateField: %s\n", updateField);
    printf("tableName %s\n", tableName);
    printf("dataSize %lu\n", newFieldValue->dataSize);
//    Iterator *iterator = readEntityRecordWithCondition(file, tableName, predMass->predicate, predMass->predicateNumber);
//    int fieldNumber;
//    TableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
//    for (int i = 0; i < tableOffsetBlock->fieldsNumber; i++) {
//        if (strcmp(tableOffsetBlock->nameTypeBlock[i].fieldName, updateField) == 0) {
//            fieldNumber = i;
//        }
//    }
//    int recordsNumberUpdate = 0;
//    while (hasNext(iterator, file)) {
//        recordsNumberUpdate++;
//    }
//    EntityRecord *entityRecord = malloc(sizeof(EntityRecord) * recordsNumberUpdate);
//
//    Iterator *iterator1 = readEntityRecordWithCondition(file, tableName, predMass->predicate,
//                                                        predMass->predicateNumber);
//    while (hasNext(iterator1, file)) {
//        entityRecord
//    }
}

void *handleUpdate(xmlNodePtr root, FILE *file, int client_socket) {
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
    send_data(client_socket, create_xml_document("Your entity successfully updated"));
    send_data(client_socket, create_xml_document("End"));
}

JoinWrapper *formJoin(xmlNodePtr joinNode) {
    JoinWrapper *joinWrapper = malloc(sizeof(JoinWrapper));
    for (xmlNodePtr node = joinNode->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "leftOperand")) {
            for (xmlNodePtr nodeLeft = node->children; nodeLeft; nodeLeft = nodeLeft->next) {
                if (xmlStrEqual(nodeLeft->name, BAD_CAST "leftTable")) {
                    joinWrapper->leftTableName = (char *) xmlNodeGetContent(nodeLeft);
                }
                if (xmlStrEqual(nodeLeft->name, BAD_CAST "leftField")) {
                    joinWrapper->leftFieldName = (char *) xmlNodeGetContent(nodeLeft);
                }
            }
        }
        if (xmlStrEqual(node->name, BAD_CAST "rightOperand")) {
            for (xmlNodePtr nodeRight = node->children; nodeRight; nodeRight = nodeRight->next) {
                if (xmlStrEqual(nodeRight->name, BAD_CAST "rightTable")) {
                    joinWrapper->rightTableName = (char *) xmlNodeGetContent(nodeRight);
                }
                if (xmlStrEqual(nodeRight->name, BAD_CAST "rightField")) {
                    joinWrapper->rightFieldName = (char *) xmlNodeGetContent(nodeRight);
                }
            }
        }
    }
    return joinWrapper;
}

void doSelectInDataBase(char *tableName, FILE *file, int client_socket) {
    Iterator *iterator = readEntityRecordWithCondition(file, tableName, NULL, 0);
    TableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    while (hasNext(iterator, file)) {
        EntityRecord *entityRecord = next(iterator, file);
        send_data(client_socket,
                  create_xml_document(printEntityRecord(entityRecord, tableOffsetBlock->fieldsNumber,
                                                        tableOffsetBlock->nameTypeBlock)));
    }
    send_data(client_socket, create_xml_document("End"));
}

void doSelectInDataBaseWithCond(char *tableName, PredMass *predMass, FILE *file, int client_socket) {
    Iterator *iterator = readEntityRecordWithCondition(file, tableName, predMass->predicate, predMass->predicateNumber);
    TableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    while (hasNext(iterator, file)) {
        EntityRecord *entityRecord = next(iterator, file);
        send_data(client_socket,
                  create_xml_document(printEntityRecord(entityRecord, tableOffsetBlock->fieldsNumber,
                                                        tableOffsetBlock->nameTypeBlock)));
    }
    send_data(client_socket, create_xml_document("End"));
}

NameTypeBlock *
concatenateNameType(uint16_t size_1, NameTypeBlock *nameTypeBlock1, uint16_t size_2, NameTypeBlock *nameTypeBlock2) {
    uint16_t newSize = size_1 + size_2;

    NameTypeBlock *concatenatedArray = (NameTypeBlock *) malloc(newSize * sizeof(NameTypeBlock));
    if (concatenatedArray == NULL) {
        return NULL;
    }
    for (uint16_t i = 0; i < size_1; ++i) {
        concatenatedArray[i] = nameTypeBlock1[i];
    }

    for (uint16_t i = 0; i < size_2; ++i) {
        concatenatedArray[size_1 + i] = nameTypeBlock2[i];
    }

    return concatenatedArray;
}

void doSelectInDataBaseWithJoin(char *tableName, JoinWrapper *joinWrapper, FILE *file, int client_socket) {
    Iterator *iterator = readEntityRecordWithCondition(file, tableName, NULL, 0);
    int recordsNumber = 0;
    while (hasNext(iterator, file)) recordsNumber++;

    Iterator *joinIterator = readEntityRecordWithCondition(file, tableName, NULL, 0);
    TableOffsetBlock *leftTableOffsetBlock = findTableOffsetBlock(file, tableName);
    TableOffsetBlock *rightTableOffsetBlock = findTableOffsetBlock(file, joinWrapper->rightTableName);

    int fieldNumber = 0;
    for (int i = 0; i < leftTableOffsetBlock->fieldsNumber; i++) {
        if (strcmp(leftTableOffsetBlock->nameTypeBlock[i].fieldName, joinWrapper->leftFieldName) == 0) {
            fieldNumber = i;
            break;
        }
    }

    uint16_t fieldsNumber = leftTableOffsetBlock->fieldsNumber + rightTableOffsetBlock->fieldsNumber;
    NameTypeBlock *newNameTypeBlock = concatenateNameType(leftTableOffsetBlock->fieldsNumber,
                                                          leftTableOffsetBlock->nameTypeBlock,
                                                          rightTableOffsetBlock->fieldsNumber,
                                                          rightTableOffsetBlock->nameTypeBlock);
    for (int i = 0; i < recordsNumber; i++) {
        EntityRecord *entityRecord = nextWithJoin(
                joinIterator,
                joinWrapper->rightTableName,
                file,
                fieldNumber,
                joinWrapper->rightFieldName);
        send_data(client_socket, create_xml_document(printEntityRecord(entityRecord, fieldsNumber, newNameTypeBlock)));
    }
    send_data(client_socket, create_xml_document("End"));
}

void doSelectInDataBaseWithJoinAndCond(char *tableName, JoinWrapper *joinWrapper, PredMass *predMass, FILE *file,
                                       int client_socket) {
    Iterator *iterator = readEntityRecordWithCondition(file, tableName, predMass->predicate, predMass->predicateNumber);
    int recordsNumber = 0;
    while (hasNext(iterator, file)) recordsNumber++;

    Iterator *joinIterator = readEntityRecordWithCondition(file, tableName, NULL, 0);
    TableOffsetBlock *leftTableOffsetBlock = findTableOffsetBlock(file, tableName);
    TableOffsetBlock *rightTableOffsetBlock = findTableOffsetBlock(file, joinWrapper->rightTableName);

    int fieldNumber = 0;
    for (int i = 0; i < leftTableOffsetBlock->fieldsNumber; i++) {
        if (strcmp(leftTableOffsetBlock->nameTypeBlock[i].fieldName, joinWrapper->leftFieldName) == 0) {
            fieldNumber = i;
            break;
        }
    }

    uint16_t fieldsNumber = leftTableOffsetBlock->fieldsNumber + rightTableOffsetBlock->fieldsNumber;
    NameTypeBlock *newNameTypeBlock = concatenateNameType(leftTableOffsetBlock->fieldsNumber,
                                                          leftTableOffsetBlock->nameTypeBlock,
                                                          rightTableOffsetBlock->fieldsNumber,
                                                          rightTableOffsetBlock->nameTypeBlock);
    for (int i = 0; i < recordsNumber; i++) {
        EntityRecord *entityRecord = nextWithJoin(
                joinIterator,
                joinWrapper->rightTableName,
                file,
                fieldNumber,
                joinWrapper->rightFieldName);
        send_data(client_socket, create_xml_document(printEntityRecord(entityRecord, fieldsNumber, newNameTypeBlock)));
    }
    send_data(client_socket, create_xml_document("End"));
}

void *handleSelect(xmlNodePtr root, FILE *file, int client_socket) {
    char *tableName;
    bool isCond = false;
    bool isJoin = false;
    PredMass *predMass = malloc(sizeof(PredMass));
    JoinWrapper *joinWrapper = malloc(sizeof(JoinWrapper));
    for (xmlNodePtr node = root->children; node; node = node->next) {
        if (xmlStrEqual(node->name, BAD_CAST "tableName")) {
            tableName = (char *) xmlNodeGetContent(node);
        }
        if (xmlStrEqual(node->name, BAD_CAST "filter")) {
            isCond = true;
            predMass = goDepth(node->children[0].next);
            printPredMass(predMass);
        }
        if (xmlStrEqual(node->name, BAD_CAST "join")) {
            isJoin = true;
            joinWrapper = formJoin(node);
        }
    }
    if (isCond && isJoin) {
        doSelectInDataBaseWithJoinAndCond(tableName, joinWrapper, predMass, file, client_socket);
    } else if (isCond) {
        doSelectInDataBaseWithCond(tableName, predMass, file, client_socket);
    } else if (isJoin) {
        doSelectInDataBaseWithJoin(tableName, joinWrapper, file, client_socket);
    } else {
        doSelectInDataBase(tableName, file, client_socket);
    }
}

void *from_xml(char *xml, FILE *file, int client_socket) {
    xmlDocPtr doc;
    xmlNodePtr root, node;

    LIBXML_TEST_VERSION

    doc = xmlReadMemory(xml, strlen(xml), "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse the input XML.\n");
        return "Failed to parse the input XML.\n";
    }

    root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        fprintf(stderr, "Empty document.\n");
        xmlFreeDoc(doc);
        return "Empty document.\n";
    }

    for (node = root->children; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            if (xmlStrEqual(node->name, BAD_CAST "requestType")) {
                if (strcmp(xmlNodeGetContent(node), "INSERT_QUERY") == 0)
                    return handleInsert(root, file, client_socket);
                else if (strcmp(xmlNodeGetContent(node), "UPDATE_QUERY") == 0)
                    return handleUpdate(root, file, client_socket);
                else if (strcmp(xmlNodeGetContent(node), "DELETE_QUERY") == 0)
                    return handleDelete(root, file, client_socket);
                else if (strcmp(xmlNodeGetContent(node), "SELECT_QUERY") == 0)
                    return handleSelect(root, file, client_socket);
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
}
