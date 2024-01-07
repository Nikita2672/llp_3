#ifndef SERVER_FROM_XML_H
#define SERVER_FROM_XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "stdio.h"
#include "../database/include/query/query.h"

char *from_xml(char *xml, FILE *file);

typedef struct {
    uint8_t predicateNumber;
    Predicate *predicate;
} PredMass;

PredMass *parseCondition(xmlNodePtr condition);

xmlNodePtr traverseChildren(xmlNodePtr node, int childNumber);

#endif //SERVER_FROM_XML_H
