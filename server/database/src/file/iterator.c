#include "../../include/file/iterator.h"
#include "../../include/data/data.h"
#include "../../include/file/fileApi.h"
#include "../../include/file/dataBlocks.h"
#include "../../include/query/query.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "../../include/util/util.h"

bool hasNext(Iterator *iterator, FILE *file) {
    HeaderSection headerSection;
    fseek(file, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof(HeaderSection), 1, file);
    SpecialDataSection specialDataSection;
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fread(&specialDataSection, sizeof(specialDataSection), 1, file);
    bool hasNextVariable = false;
    for (uint16_t i = iterator->currentPositionInBlock; i < headerSection.recordsNumber; i++) {
        EntityRecord *entityRecord = readRecord(file, i, iterator->blockOffset, iterator->fieldsNumber);
        bool valid = true;
        for (uint16_t j = 0; j < iterator->predicateNumber; j++) {
            bool result = checkPredicate(&iterator->predicate[j], entityRecord, iterator->fieldsNumber,
                                         iterator->nameTypeBlock);
            if (!result) {
                valid = false;
                break;
            }
        }
        freeEntityRecord(entityRecord, iterator->fieldsNumber);
        if (valid) {
            hasNextVariable = true;
            iterator->currentPositionInBlock = (i + 1);
            return hasNextVariable;
        }
    }
    if (specialDataSection.nextBlockOffset != 0) {
        iterator->currentPositionInBlock = 0;
        iterator->blockOffset = specialDataSection.nextBlockOffset;
        return hasNext(iterator, file);
    } else {
        return false;
    }
}


EntityRecord *next(Iterator *iterator, FILE *file) {
    return readRecord(file, iterator->currentPositionInBlock - 1, iterator->blockOffset, iterator->fieldsNumber);
}

static EntityRecord *concatenateEntityRecords(EntityRecord *entityRecord1,
                                              EntityRecord *entityRecord2, uint8_t fieldsNumber1,
                                              uint8_t fieldsNumber2) {
    if (entityRecord2 == NULL) return entityRecord1;
    FieldValue *newFields = malloc(sizeof(FieldValue) * (fieldsNumber1 + fieldsNumber2));
    memcpy(newFields, entityRecord1->fields, sizeof(FieldValue) * fieldsNumber1);
    memcpy(newFields + fieldsNumber1, entityRecord2->fields, sizeof(FieldValue) * fieldsNumber2);
    EntityRecord *entityRecord = malloc(sizeof(EntityRecord));
    entityRecord->fields = newFields;
    return entityRecord;
}


EntityRecord *nextWithJoin(Iterator *iterator1, const char *tableName,
                           FILE *file, uint8_t fieldNumber, char *fieldName) {
    EntityRecord *entityRecord1 = next(iterator1, file);;
//    if (hasNext(iterator1, file)) {
//        entityRecord1 =
//    } else {
//        return NULL;
//    }
    FieldValue fieldValue = entityRecord1->fields[fieldNumber];
    Predicate predicate = {&fieldValue, fieldName, EQUALS};
    Iterator *iterator2 = readEntityRecordWithCondition(file, tableName, &predicate, 1);
    EntityRecord *entityRecord2 = malloc(sizeof (EntityRecord));


    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Id", INT);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Name", STRING);

    NameTypeBlock *nameTypeBlock21 = initNameTypeBlock("AuthorId", INT);
    NameTypeBlock *nameTypeBlock22 = initNameTypeBlock("RecipeIdentity", INT);
    NameTypeBlock *nameTypeBlock23 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock24 = initNameTypeBlock("PreparationTime", INT);
    NameTypeBlock *nameTypeBlock25 = initNameTypeBlock("Description", STRING);
    NameTypeBlock *nameTypeBlock26 = initNameTypeBlock("RecipeId", INT);
    NameTypeBlock *nameTypeBlock27 = initNameTypeBlock("SkillLevel", STRING);
    NameTypeBlock *nameTypeBlock28 = initNameTypeBlock("CookingTime", INT);
    NameTypeBlock *nameTypeBlock29 = initNameTypeBlock("ElementId", INT);

    NameTypeBlock nameTypeBlocks1[2] = {
            *nameTypeBlock1,
            *nameTypeBlock2
    };

    NameTypeBlock nameTypeBlocks[11] = {
            *nameTypeBlock21,
            *nameTypeBlock22,
            *nameTypeBlock23,
            *nameTypeBlock24,
            *nameTypeBlock25,
            *nameTypeBlock26,
            *nameTypeBlock27,
            *nameTypeBlock28,
            *nameTypeBlock29,
            *nameTypeBlock1,
            *nameTypeBlock2,
    };

    if (hasNext(iterator2, file)) {
        entityRecord2 = next(iterator2, file);
//        printf("SEARCH: %s\n", printEntityRecord(entityRecord2, 2, nameTypeBlocks1));
        EntityRecord *entityRecord = concatenateEntityRecords(entityRecord1, entityRecord2,
                                                              iterator1->fieldsNumber,
                                                              iterator2->fieldsNumber);
        printf("FOUND: %s\n", printEntityRecord(entityRecord, 11, nameTypeBlocks));
        return entityRecord;
    }
    EntityRecord *entityRecord = concatenateEntityRecords(entityRecord1, entityRecord2,
                                                          iterator1->fieldsNumber,
                                                          iterator2->fieldsNumber);
    return entityRecord;
}

struct EntityMass *nextWithMulJoin(Iterator *iterator1, const char *tableName,
                                   FILE *file, uint8_t fieldNumber, char *fieldName,
                                   Predicate *predicate2, int32_t predicateNumber) {
    EntityRecord *entityRecord1 = NULL;
    if (hasNext(iterator1, file)) {
        entityRecord1 = next(iterator1, file);
    } else {
        return NULL;
    }
    FieldValue fieldValue = entityRecord1->fields[fieldNumber];
    Predicate predicate = {&fieldValue, fieldName, EQUALS};
    Iterator *iterator2 = readEntityRecordWithCondition(file, tableName, &predicate, 1);
    int32_t counter = 0;
    while (hasNext(iterator2, file)) {
        counter++;
    }
    EntityRecord **entityArray = malloc(sizeof(EntityRecord) * counter);
    Iterator *iterator3 = readEntityRecordWithCondition(file, tableName, &predicate, 1);
    int32_t position = 0;
    while (hasNext(iterator3, file)) {
        EntityRecord *entityRecord2 = next(iterator3, file);
        EntityRecord *entityRecord = concatenateEntityRecords(entityRecord1, entityRecord2,
                                                              iterator1->fieldsNumber,
                                                              iterator2->fieldsNumber);
        entityArray[position] = entityRecord;
        position++;
    }
    struct EntityMass *entityMass = malloc(sizeof(struct EntityMass));
    entityMass->massEntityRecords = entityArray;
    entityMass->numberRecords = counter;
    return entityMass;
}

void freeIterator(Iterator *iterator) {
    free(iterator->nameTypeBlock);
    free(iterator);
}
