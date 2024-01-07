#ifndef SERVER_FROM_XML_H
#define SERVER_FROM_XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemas.h>
#include "stdio.h"
#include "../database/include/query/query.h"

#define SCHEMA_RESPONSE_FILE "/home/iwaa0303/CLionProjects/llp_3/server/schema/response_schema.xsd"

void *from_xml(char *xml, FILE *file, int client_socket);

typedef struct {
    uint8_t predicateNumber;
    Predicate *predicate;
} PredMass;

typedef struct {
    char *leftTableName;
    char *rightTableName;
    char *leftFieldName;
    char *rightFieldName;
} JoinWrapper;

PredMass *parseCondition(xmlNodePtr condition);

xmlNodePtr traverseChildren(xmlNodePtr node, int childNumber);

#endif //SERVER_FROM_XML_H
