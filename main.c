#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define MAX_FIELD_SIZE 20
/*
typedef struct TreeNode {
    const char name[MAX_FIELD_SIZE];
    void * data;
    // data = malloc(), with capacity so it's a proper dynamic array
} TreeNode;

typedef struct FieldTreePointer {
    struct NodeAddrToOffset {
        void * nodeAddr;
        unsigned int offset;
    } NodeAddrToOffset;

    NodeAddrToOffset * nodeAddrToOffsets; 
    //  where valuesToAdd is like:
    //  void makeWith(FieldTreePointer ftPtr, Schema treeSchema, List newFields) {
    //      const unsigned int valuesToAdd = size(newFields);
    //      ? addrs = addDataForSpecificTree(treeSchema);
    //      ftPtr2 . nodeAddrToOffsets = malloc(sizeof(ftPtr1 . nodeAddrToOffsets) + valuesToAdd);
    //      ftPtr2 . nodeAddrToOffsets . push (eachof (addrs));
    //      ...
    //  }
} FieldTreePointer;
 */

typedef struct {
    const char * name;
    const char * type;
    void * value;
} Field;

typedef struct List {
    Field value;
    struct List * next;
} List;

void * findField(const char * name, List * list) {
    while (list) {
        if (strcmp(list -> value . name, name) == 0) {
            return list -> value . value;
        }
        list = list -> next;
    }
    return NULL;
}

typedef struct {
    List fields;
} Tree;

// To help print stuff
void printNamedInt(Field namedValue) {
    // printf("<%s> %s = %d\n", namedValue . type, namedValue . name, * ((int *) (namedValue . value)));
    printf("<%s> %s = %s\n", namedValue . type, namedValue . name, (char *) (namedValue . value));
}

// Different print options.
void printInt(const char * name, const char * data) {
    long long int number = strtol(data, NULL, 10);
    printf("%s = %lld\n", name, number);
    fflush(stdout);
}

void printString(const char * name, const char * data) {
    printf("%s = %s\n", name, (char *) data);
}

void printJob(const char * name, const char * data) {
    printf("%s = { ... }\n", name);
}

typedef struct PrintType {
    void (*printer) (const char *, const char *);
    const char * type;
} PrintType;

void printFields(List * fields, PrintType printers[], const unsigned int printerSize) {
    // Do a special thing for nested trees  
    while (fields) {
        bool printed = false;
        for (unsigned int i = 0; i < printerSize; i++) {
            PrintType p = printers[i];
            if (strcmp(fields -> value . type, p . type) == 0) {
                // printf("for the name %s on the type %s\n", fields -> value . name, fields -> value . type);
                p . printer(fields -> value . name, fields -> value . value);
                printed = true;
                break;
            }
        }
        if (!printed) {
            printf("Unknown type <%s> %s = %s\n", fields -> value . type, fields -> value . name, (char *) fields -> value . value);
        }
        fields = fields -> next; 
    }
}


void printList(List * list) {
    printf("Assuming list is full of ints\n");
    while (list) {
        printNamedInt(list -> value);
        list = list -> next;
    }
}

void testTree() {
    const int nineteen = 19;
    Field age = { .name = "age", .value = (void *) &nineteen };
    printNamedInt(age);

    List basicStudentFields = { .value = age };
    List job;
    job . value = (Field) { .name = "job", .value = "Purdue student" };
    job . next = NULL;
    basicStudentFields . next = &job;

    Tree tree = (Tree) { basicStudentFields };
    printList(&tree . fields);
    printf("Actually job is a string: %s\n", (char *) findField("job", &tree . fields));
}

// We can't return just a Tree tree {} because we need this to be a different pointer, when storing each tree.
Tree initTree(List * * outHead, List * * outCurr) {
    List * list = malloc(sizeof(List));
    list -> value = (Field) { NULL, NULL, NULL };
    list -> next = NULL;
    * outHead = list;
    * outCurr = * outHead;
    Tree tree;
    tree . fields = * list;
    return tree;
}

typedef struct Trees {
    Tree * tree;
    struct Trees * next;
} Trees;

// In case Trees is NULL, return the same thing we append to.
// DO NOT Replace trees whose first fields have the same name (this is the name of the tree)
void appendTrees(Trees * trees, Tree * tree) {
    Trees * newTrees = malloc(sizeof(Trees));
    newTrees -> tree = tree;
    newTrees -> next = NULL;

    /*
    for (Trees * curr = trees -> next; curr; curr = curr -> next) {
        if (strcmp(curr -> tree -> fields . value . name, tree -> fields . value . name) == 0) {
            curr -> tree = tree; 
            return;
        }
    }
    */

    Trees * end = trees;
    while (end -> next) {
        end = end -> next;
    }
    end -> next = newTrees;
}

List * newList(Field * field) {
    List * list = malloc(sizeof(List));
    list -> value = * field;
    list -> next = NULL;
    return list;
}

Tree * cloneTree(Tree * tree) {
    Tree * newTree = malloc(sizeof(Tree));
    List * newFields = malloc(sizeof(List));
    List * head = newFields;
    for (List * list = &tree -> fields; list; list = list -> next) {
        // MAYBE MALLOC IT
        Field field;
        field . type = list -> value . type;
        field . name = list -> value . name;
        field . value = list -> value . value;

        List * nextFields = newList(&field);
        
        newFields -> next = nextFields;
        newFields = newFields -> next;
    }
    head = head -> next;
    newTree -> fields = * head;
    return newTree;
}

// The most recent _, by convention, is like Job _=start ... Wage wage=_. Do not refer to itself, obviously.
Tree * findMostRecentName(Trees createdTrees[], Tree * tree, Field * field) {
    Tree * mostRecent = NULL;
    
    for (Trees * t = createdTrees; t; t = t -> next) {
        if (t -> tree == tree) {
            return mostRecent;
        }

        // printf("name =? value for <%s>, <%s>\n", t -> tree -> fields . value . name, (char *) field -> value);
        if (strcmp(t -> tree -> fields . value . name, (char *) field -> value) == 0) {
            // printf("name = value for <%s>\n", t -> tree -> fields . value . name);
            mostRecent = t -> tree;
        }
    }
    return mostRecent;
}

void indent(unsigned int depth) {
    while (depth--) {
        printf("    ");
    }
}

void recurPrint(const unsigned int depth, Trees createdTrees[], Tree * tree) {
    List * fields = &tree -> fields;

    if (!fields) {
        return;
    }
    // Make sure to skip the first field, because it is of the tree type, itself.
    indent(depth);
    printString(fields -> value . name, fields -> value . value);

    for (fields = fields -> next; fields; fields = fields -> next) {
        const Field field = fields -> value;
        // Is it like Wage wage=null?
        if (strcmp(field . value, "null") == 0) {
            indent(depth + 1);
            printf("%s is null\n", field . name);
            continue;
        }
        
        // Is this a tree?
        bool isTree = false;
        for (Trees * t = createdTrees; t; t = t -> next) {
            if (strcmp(t -> tree -> fields . value . type, field . type) == 0) {
                isTree = true;
                break;
            }
        }
        if (isTree) {
            recurPrint(depth + 1, createdTrees, (Tree *) field . value);
        } else {
            indent(depth + 1);
            printString(field . name, field . value);
        }

        #if 0
        // Check if the field we have is a tree.
        Tree * mostRecent = findMostRecentName(createdTrees, tree, &fields -> value);
        if (mostRecent) {
            recurPrint(depth + 1, createdTrees, mostRecent);
        } else {
        /*
        for (Trees * t = createdTrees; t; t = t -> next) {
            // Find the matching name
            if (strcmp(t -> tree -> fields . value . name, (char *) field . value) == 0) {
                printf("recurPrint\n");
                printf("field value as a string %s\n", (char *) field . value);
                
                recurPrint(depth + 1, createdTrees, (Tree *) field . value);

                // Ok, we need to find what underscore came right before tree.
                Tree * mostRecentName = findMostRecentName(createdTrees, tree, &fields -> value);
                if (mostRecentName == NULL) {
                    return;
                }
                // printf("Tree with type %s name %s\n", field . type, field . name);
                printf("mostRecentName name %s\n", mostRecentName -> fields . value . name);
                recurPrint(depth + 1, createdTrees, mostRecentName);

                foundTree = true;
                break;
            }
        }
        if (!foundTree) {
        */
            indent(depth + 1);
            printString(field . name, field . value);
        }
        #endif
    }
}

void print(Trees createdTrees[], Tree * tree) {
    /*
    printf("\ntrees: ");
    for (Trees * t = createdTrees; t; t = t -> next) {
        printf("%s, ", t -> tree -> fields . value . name);
    }
    printf("\n\n");
    */
    recurPrint(0, createdTrees, tree);
}

void stringBecomeTreeMaybe(Trees * trees, List * node) {
    // Replace the value of the node if node has the same name as a tree
    const char const * nodeValue = (const char *) node -> value . value;
    // printf("node value %s\n", nodeValue);
    if (!trees) {
        return;
    }
    Tree * tryMostRecentName = NULL;
    for (Trees * t = trees; t; t = t -> next) {
        if (strcmp(t -> tree -> fields . value . name, nodeValue) == 0) {
            // printf("match for %s\n", nodeValue); 
            tryMostRecentName = t -> tree;
        }
    }
    if (tryMostRecentName) {
        node -> value . value = tryMostRecentName;
    }
}

Tree * findMhh(Trees * createdTrees) {
    for (Trees * tree = createdTrees; tree; tree = tree -> next) {
        if (strcmp(tree -> tree -> fields . value . name, "mhh") == 0) {
            return tree -> tree;
        }
    }
    return NULL;
}

// TODO
/*
void set(Trees * trees, Tree * tree, const char * accessors[], void * value) {
    for (List * field = &tree -> fields; field; field = field -> next) {
        if (strcmp(field -> value . name, * accessors) == 0) {
            // We found the field, but if the field is a tree, we have to call recurSet on it.
            for (Trees * t = trees; t; t = t -> next) {
                if (strcmp(t -> tree -> fields . value . name, * accessors) == 0) {
                    set(trees, 
                }
            }
            if (strcmp(field -> value . type
            field -> value . value = value;
            return;
        }
    }
}
*/

int main(int argc, char * argv[]) {
    // testTree();
    // printFilesRead(argc, argv);

    FILE * input = fopen("input1", "r");
    if (input == NULL) {
        return 1;
    }

    char line[100];
    List * head;
    List * curr;
    Tree tree = initTree(&head, &curr);

    PrintType printers[] = {
        { .type = "string", .printer = printString },
        { .type = "int", .printer = printInt },
        { .type = "Job", .printer = printJob }
    };
    unsigned int printersSize = sizeof(printers) / sizeof(printers[0]);
    printersSize = printersSize;
    
    // Always have a meaningless pointer at the front of this linkedlist, because it will be easy to append to. Also, therefore pass createdTrees -> next as the head.
    Trees * createdTrees = malloc(sizeof(Trees));
    createdTrees -> next = NULL;

    // Read line by line
    while (fgets(line, sizeof(line), input) != NULL) {
        // DO NOT CHANGE THIS!
        char * newLine = malloc(sizeof(line));
        strcpy(newLine, line);

        // Ignore whitespace lines.
        bool whitespace = true;
        for (char * ch = newLine; * ch != '\0'; ch++) {
            if (* ch != ' ' && * ch != '\t' && * ch != '\n') {
                whitespace = false;
                break;
            }
        }
        if (whitespace) {
            continue;
        }

        // Print every tree we come across.
        if (strcmp(line, "end\n") == 0) {
            tree . fields = * head -> next;

            // PRINT TREE
            // Check createdTrees for a match, within a new version of printFields called print.

            // CreatedTrees has a dummy stupid first pointer, to make appendTrees easier to use. Therefore the effective head is appendTrees -> next.
            // printFields(&tree . fields, printers, printersSize);
            Tree * cloned = cloneTree(&tree);
            print(createdTrees -> next, cloned);
            appendTrees(createdTrees, cloned);

            // printList(&tree . fields);
            tree = initTree(&head, &curr);
            // printf("the initTree have the SAME addr of %p\n", (void *) &tree);
            fflush(stdout);
            printf("\n\n\n");
        } else {
            char * stripped = NULL;
            for (int i = 0; i < 4; i++) {
                if (line[i] == ' ') {
                    stripped = &line[i] + 1;
                    break;
                }
            }

            if (stripped != NULL) {
                strcpy(line, stripped);
            }

            // strtok uses a STATIC pointer.
            char * type = strtok(newLine, " ");
            char * name = strtok(NULL, "=");
            char * value = strtok(NULL, "\n");

            Field * field = malloc(sizeof(Field));
            field -> type = type;
            field -> name = name;
            field -> value = value;

            List * node = newList(field);

            // But, field -> value could be a reference to another tree.
            stringBecomeTreeMaybe(createdTrees -> next, node);
            
            curr -> next = node;
            curr = curr -> next;
        }
    }

    // Use one of the nodes for integer math.
    Tree * found = findMhh(createdTrees -> next);
    Tree mhh = * found;
    mhh = mhh;

    /*
    const char * jobWageAccessor[] = { "job", "wage" };
    set(createdTrees -> next, &mhh, jobWageAccessor, * ((int *) get(&mhh, jobWageAccessor)) + 5);
    printf("%d\n", * ((int *) get(&mhh, jobWageAccessor)));
    */

    fclose(input);
}
