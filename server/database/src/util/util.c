#include "../../include/util/util.h"
#include "string.h"
#include "../../include/file/iterator.h"
#include "../../include/file/fileApi.h"

char *printEntityRecord(EntityRecord *entityRecord, uint16_t fieldsNumber, NameTypeBlock *nameTypeBlock) {
    size_t bufferSize = 4096;
    char *result = (char *)malloc(bufferSize);
    if (result == NULL) {
        fprintf(stderr, "Failed to allocate memory for result buffer.\n");
        exit(EXIT_FAILURE);
    }

    size_t offset = 0;
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        int bytesWritten = snprintf(result + offset, bufferSize - offset, "%s: ", nameTypeBlock[i].fieldName);
        if (bytesWritten < 0) {
            fprintf(stderr, "Error writing to buffer.\n");
            exit(EXIT_FAILURE);
        }
        offset += bytesWritten;

        switch (nameTypeBlock[i].dataType) {
            case INT:
                bytesWritten = snprintf(result + offset, bufferSize - offset, "%d; ", *(int32_t *)entityRecord->fields[i].data);
                break;
            case DOUBLE:
                bytesWritten = snprintf(result + offset, bufferSize - offset, "%f; ", *(double *)entityRecord->fields[i].data);
                break;
            case BOOL:
                bytesWritten = snprintf(result + offset, bufferSize - offset, "%s; ", (*(bool *)entityRecord->fields[i].data) ? "true" : "false");
                break;
            default:
                bytesWritten = snprintf(result + offset, bufferSize - offset, "%s; ", cutString((char *)entityRecord->fields[i].data, 0, entityRecord->fields[i].dataSize));
                break;
        }

        if (bytesWritten < 0) {
            fprintf(stderr, "Error writing to buffer.\n");
            exit(EXIT_FAILURE);
        }
        offset += bytesWritten;
    }

    result[offset] = '\0';

    return result;
}

char *cutString(char *string, uint64_t start, uint64_t end) {
    if (string == NULL || start >= end) {
        return NULL;
    }
    size_t length = end - start;
    char *newString = (char *) malloc(length + 1);

    if (newString == NULL) {
        return NULL;
    }
    strncpy(newString, string + start, length);
    newString[length] = '\0';
    return newString;
}

uint64_t getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

void freeEntityRecord(EntityRecord *entityRecord, uint16_t fieldsNumber) {
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        free(entityRecord->fields[i].data);
    }
    free(entityRecord->fields);
    free(entityRecord->linkNext);
    free(entityRecord);
}

void printMetaTableRecords(FILE* file) {
    Iterator *iteratorMeta = readEntityRecordWithCondition(file, "Meta", NULL, 0);
    NameTypeBlock nameTypeBlocksMeta = {"Offset", STRING};
    while (hasNext(iteratorMeta, file)) {
        EntityRecord *entityRecordMeta = next(iteratorMeta, file);
        printEntityRecord(entityRecordMeta, 1, &nameTypeBlocksMeta);
        freeEntityRecord(entityRecordMeta, 1);
    }
    freeIterator(iteratorMeta);
}
