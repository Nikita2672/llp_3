#include "ast_to_xml.h"
#include <libxml/parser.h>
#include <libxml/tree.h>


char *doubleToStr(double doubleValue) {
    int bufferSize = snprintf(NULL, 0, "%lf", doubleValue);
    char *stringValue = (char *) malloc(bufferSize + 1);
    snprintf(stringValue, bufferSize + 1, "%lf", doubleValue);
    return stringValue;
}

char *intToStr(int intValue) {
    int bufferSize = snprintf(NULL, 0, "%d", intValue);
    char *stringValue = (char *) malloc(bufferSize + 1);
    snprintf(stringValue, bufferSize + 1, "%d", intValue);
    return stringValue;
}

// Function to create the base XML document
xmlDocPtr createBaseXMLDocument() {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr rootNode = xmlNewNode(NULL, BAD_CAST "Root");

    xmlDocSetRootElement(doc, rootNode);

    return doc;
}

int countInsertArgs(int counter, struct AstNode *astNode) {
    if (astNode->num_children != 0) {
        counter++;
        return countInsertArgs(counter, astNode->children[0]);
    }
    return counter;
}

AstNode *getAstNode(int depth, struct AstNode *astNode) {
    if (depth == 0) return astNode;
    return getAstNode(depth - 1, astNode->children[0]);
}

char *xmlToStr(xmlDocPtr doc) {
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

    // Return the string representation of the modified document
    char *result = (char *) malloc(buffersize + 1);
    snprintf(result, buffersize + 1, "%s", xmlbuff);
    xmlFree(xmlbuff);
    return result;
}

void setContent(AstNode *astNode, xmlNodePtr nodePtr) {
    switch (astNode->type) {
        case STRING_LITERAL:
            xmlNodeSetContent(nodePtr, BAD_CAST astNode->value.str_val);
            break;
        case DOUBLE_LITERAL:
            xmlNodeSetContent(nodePtr, BAD_CAST doubleToStr(astNode->value.double_val));
            break;
        case INTEGER_LITERAL:
            xmlNodeSetContent(nodePtr, BAD_CAST intToStr(astNode->value.int_val));
            break;
        case BOOLEAN_LITERAL:
            xmlNodeSetContent(nodePtr, BAD_CAST intToStr(astNode->value.bool_val));
            break;
        case FIELD_IDENTIFIER:
            xmlNodeSetContent(nodePtr, BAD_CAST astNode->value.str_val);
            break;
        default:
            printf("There is no any Literal");
    }
}

xmlNodePtr parseInsertValues() {
    xmlNodePtr insertValues = xmlNewNode(NULL, BAD_CAST "insertValues");
    int valuesNumber = countInsertArgs(0, getRoot()->children[1]);
    AstNode *values[valuesNumber];
    for (int i = 0; i < valuesNumber; i++) {
        AstNode *node = getAstNode(i, getRoot()->children[1]);
        values[valuesNumber - i] = node->children[1];
        if (i == valuesNumber - 1) {
            values[0] = node->children[0];
            break;
        }
    }

    for (int i = 0; i < valuesNumber; i++) {

        xmlNodePtr xmlField = xmlNewNode(NULL, BAD_CAST "field");

        xmlNodePtr xmlValueType = xmlNewNode(NULL, BAD_CAST "type");
        xmlNodeSetContent(xmlValueType, BAD_CAST getAstNodeTypeName(values[i]->type));

        xmlNodePtr xmlValue = xmlNewNode(NULL, BAD_CAST "value");

        setContent(values[i], xmlValue);
        xmlAddChild(xmlField, xmlValueType);
        xmlAddChild(xmlField, xmlValue);
        xmlAddChild(insertValues, xmlField);
    }
    return insertValues;
}


xmlNodePtr formCondition(AstNode *node) {
    xmlNodePtr condition = xmlNewNode(NULL, BAD_CAST "condition");

    xmlNodePtr leftOp = xmlNewNode(NULL, BAD_CAST "leftOp");
    setContent(node->children[0], leftOp);

    xmlNodePtr rightOp = xmlNewNode(NULL, BAD_CAST "rightOp");
    setContent(node->children[1], rightOp);

    xmlNodePtr operator = xmlNewNode(NULL, BAD_CAST "operator");

    switch (node->type) {
        case EQ:
            xmlNodeSetContent(operator, BAD_CAST "==");
            break;
        case LE:
            xmlNodeSetContent(operator, BAD_CAST "<=");
            break;
        case GE:
            xmlNodeSetContent(operator, BAD_CAST ">=");
            break;
        case LT:
            xmlNodeSetContent(operator, BAD_CAST "<");
            break;
        case GT:
            xmlNodeSetContent(operator, BAD_CAST ">");
            break;
        case NE:
            xmlNodeSetContent(operator, BAD_CAST "!=");
            break;
        default:
            printf("There is no such operator");
            break;
    }

    xmlAddChild(condition, leftOp);
    xmlAddChild(condition, operator);
    xmlAddChild(condition, rightOp);
    return condition;
}

xmlNodePtr mulCond(enum AstNodeType boolOperator, xmlNodePtr leftOperand, xmlNodePtr rightOperand) {
    xmlNodePtr mul = xmlNewNode(NULL, BAD_CAST getAstNodeTypeName(boolOperator));
    xmlAddChild(mul, leftOperand);
    xmlAddChild(mul, rightOperand);
    return mul;
}

xmlNodePtr goDepth(AstNode *astNode) {
    if (astNode->type == OR || astNode->type == AND) {
        return mulCond(astNode->type, goDepth(astNode->children[0]), goDepth(astNode->children[1]));
    }
    return formCondition(astNode);
}


xmlNodePtr parseConditions(int depth, int level) {

    xmlNodePtr conditionValues = xmlNewNode(NULL, BAD_CAST "filter");
    xmlNodePtr conditions;
    if (level == 0) conditions = goDepth(getRoot()->children[depth]->children[0]);
    else if (level == 1) conditions = goDepth(getRoot()->children[depth]->children[0]->children[0]);
    else if (level == 2) conditions = goDepth(getRoot()->children[depth]->children[0]->children[1]->children[0]);
    xmlAddChild(conditionValues, conditions);
    return conditionValues;
}

xmlNodePtr parseJoin(int depth, int level) {

    xmlNodePtr join = xmlNewNode(NULL, BAD_CAST "join");
    xmlNodePtr leftOperand = xmlNewNode(NULL, BAD_CAST "leftOperand");
    xmlNodePtr leftTable = xmlNewNode(NULL, BAD_CAST "leftTable");
    xmlNodePtr leftField = xmlNewNode(NULL, BAD_CAST "leftField");

    xmlNodePtr rightOperand = xmlNewNode(NULL, BAD_CAST "rightOperand");
    xmlNodePtr rightTable = xmlNewNode(NULL, BAD_CAST "rightTable");
    xmlNodePtr rightField = xmlNewNode(NULL, BAD_CAST "rightField");

    char *leftTableVal;
    char *leftFieldVal;
    char *rightTableVal;
    char *rightFieldVal;
    if (level == 1) {
        leftTableVal = getRoot()->children[1]->children[0]->children[1]->children[0]->children[0]->value.str_val;
        leftFieldVal = getRoot()->children[1]->children[0]->children[1]->children[0]->children[1]->value.str_val;
        rightTableVal = getRoot()->children[1]->children[0]->children[1]->children[1]->children[0]->value.str_val;
        rightFieldVal = getRoot()->children[1]->children[0]->children[1]->children[1]->children[1]->value.str_val;
    } else if (level == 2) {
        leftTableVal = getRoot()->children[1]->children[0]->children[0]->children[1]->children[0]->children[0]->value.str_val;
        leftFieldVal = getRoot()->children[1]->children[0]->children[0]->children[1]->children[0]->children[1]->value.str_val;
        rightTableVal = getRoot()->children[1]->children[0]->children[0]->children[1]->children[1]->children[0]->value.str_val;
        rightFieldVal = getRoot()->children[1]->children[0]->children[0]->children[1]->children[1]->children[1]->value.str_val;
    }
    xmlNodeSetContent(leftTable, BAD_CAST leftTableVal);
    xmlNodeSetContent(leftField, BAD_CAST leftFieldVal);
    xmlNodeSetContent(rightTable, BAD_CAST rightTableVal);
    xmlNodeSetContent(rightField, BAD_CAST rightFieldVal);

    xmlAddChild(leftOperand, leftTable);
    xmlAddChild(leftOperand, leftField);

    xmlAddChild(rightOperand, rightTable);
    xmlAddChild(rightOperand, rightField);

    xmlAddChild(join, leftOperand);
    xmlAddChild(join, rightOperand);
    return join;
}


char *handleInsertQuery(xmlDocPtr doc) {

    xmlNodePtr insertNode = xmlNewNode(NULL, BAD_CAST "requestType");
    xmlNodeSetContent(insertNode, BAD_CAST "INSERT_QUERY");

    xmlNodePtr tableNode = xmlNewNode(NULL, BAD_CAST "tableName");
    xmlNodeSetContent(tableNode, BAD_CAST getRoot()->children[0]->children[0]->value.str_val);


    xmlNodePtr insertValues = parseInsertValues();

    xmlAddChild(xmlDocGetRootElement(doc), insertNode);
    xmlAddChild(xmlDocGetRootElement(doc), tableNode);
    xmlAddChild(xmlDocGetRootElement(doc), insertValues);

    return xmlToStr(doc);
}

char *handleDeleteQuery(xmlDocPtr doc) {
    xmlNodePtr deleteNode = xmlNewNode(NULL, BAD_CAST "requestType");
    xmlNodeSetContent(deleteNode, BAD_CAST "DELETE_QUERY");

    xmlNodePtr tableNode = xmlNewNode(NULL, BAD_CAST "tableName");
    xmlNodeSetContent(tableNode, BAD_CAST getRoot()->children[0]->children[0]->value.str_val);
    xmlNodeSetContent(tableNode, BAD_CAST getRoot()->children[0]->children[0]->value.str_val);

    xmlNodePtr conditions = parseConditions(1, 0);

    xmlAddChild(xmlDocGetRootElement(doc), deleteNode);
    xmlAddChild(xmlDocGetRootElement(doc), tableNode);
    xmlAddChild(xmlDocGetRootElement(doc), conditions);

    return xmlToStr(doc);
}


char *handleUpdateQuery(xmlDocPtr doc) {
    xmlNodePtr updateNode = xmlNewNode(NULL, BAD_CAST "requestType");
    xmlNodeSetContent(updateNode, BAD_CAST "UPDATE_QUERY");

    xmlNodePtr updateTable = xmlNewNode(NULL, BAD_CAST "tableName");
    xmlNodeSetContent(updateTable, BAD_CAST getRoot()->children[0]->children[0]->value.str_val);

    xmlNodePtr updateField = xmlNewNode(NULL, BAD_CAST "updateField");

    xmlNodePtr updateFieldName = xmlNewNode(NULL, BAD_CAST "updateFieldName");
    xmlNodeSetContent(updateFieldName, BAD_CAST getRoot()->children[1]->children[0]->children[1]->value.str_val);

    xmlNodePtr updateFieldValue = xmlNewNode(NULL, BAD_CAST "updateFieldValue");
    xmlNodeSetContent(updateFieldValue, BAD_CAST getRoot()->children[1]->children[1]->value.str_val);

    xmlAddChild(updateField, updateFieldName);
    xmlAddChild(updateField, updateFieldValue);

    xmlNodePtr conditions = parseConditions(2, 0);

    xmlAddChild(xmlDocGetRootElement(doc), updateNode);
    xmlAddChild(xmlDocGetRootElement(doc), updateTable);
    xmlAddChild(xmlDocGetRootElement(doc), updateField);
    xmlAddChild(xmlDocGetRootElement(doc), conditions);

    return xmlToStr(doc);
}

bool checkType(enum AstNodeType type) {
    for (int i = 0; i < getRoot()->children[1]->num_children; i++) {
        if (getRoot()->children[1]->children[i]->type == type) return true;
    }
    if (getRoot()->children[1]->children[0]->type == QUERY_BODY_CLAUSES) {
        for (int i = 0; i < getRoot()->children[1]->children[0]->num_children; i++) {
            if (getRoot()->children[1]->children[0]->children[i]->type == type) return true;
        }
    }
    return false;
}


char *handleSelectQuery(xmlDocPtr doc) {
    xmlNodePtr selectNode = xmlNewNode(NULL, BAD_CAST "requestType");
    xmlNodeSetContent(selectNode, BAD_CAST "SELECT_QUERY");

    xmlNodePtr selectTable = xmlNewNode(NULL, BAD_CAST "tableName");
    xmlNodeSetContent(selectTable, BAD_CAST getRoot()->children[0]->children[1]->children[0]->value.str_val);
    bool isJoin = checkType(JOIN);
    bool isCond = checkType(WHERE);
    bool isClause = checkType(QUERY_BODY_CLAUSES);

    xmlAddChild(xmlDocGetRootElement(doc), selectNode);
    xmlAddChild(xmlDocGetRootElement(doc), selectTable);

    xmlNodePtr conditions = NULL;
    if (isCond && isClause) conditions = parseConditions(1, 2);
    else if (isCond) conditions = parseConditions(1, 1);

    if (conditions != NULL) {
        xmlAddChild(xmlDocGetRootElement(doc), conditions);
    }

    xmlNodePtr joinClause = NULL;
    if (isJoin && isClause) joinClause = parseJoin(1, 2);
    else if (isJoin) joinClause = parseJoin(1, 1);

    if (joinClause != NULL) {
        xmlAddChild(xmlDocGetRootElement(doc), joinClause);
    }

    xmlNodePtr selectedVal = xmlNewNode(NULL, BAD_CAST "selectedVal");
    xmlNodePtr selectedEntity = xmlNewNode(NULL, BAD_CAST "entity");
    if (getRoot()->children[1]->children[1]->children[0]->num_children == 0) {
        xmlNodeSetContent(selectedEntity, BAD_CAST getRoot()->children[1]->children[1]->children[0]->value.str_val);
        xmlAddChild(selectedVal, selectedEntity);
    } else {
        xmlNodePtr selectedField = xmlNewNode(NULL, BAD_CAST "field");

        xmlNodeSetContent(selectedEntity, BAD_CAST getRoot()->children[1]->children[1]->children[0]->children[0]->value.str_val);
        xmlNodeSetContent(selectedField, BAD_CAST getRoot()->children[1]->children[1]->children[0]->children[1]->value.str_val);

        xmlAddChild(selectedVal, selectedEntity);
        xmlAddChild(selectedVal, selectedField);
    }

    xmlAddChild(xmlDocGetRootElement(doc), selectedVal);
    return xmlToStr(doc);
}


char *to_xml() {
    xmlDocPtr doc = createBaseXMLDocument();

    switch (getRoot()->type) {
        case UPDATE_QUERY:
            return handleUpdateQuery(doc);
        case INSERT_QUERY:
            return handleInsertQuery(doc);
        case SELECT_QUERY:
            return handleSelectQuery(doc);
        case DELETE_QUERY:
            return handleDeleteQuery(doc);
        default:
            printf("Thre is no such query type, please try again\n");
            return NULL;
    }
}