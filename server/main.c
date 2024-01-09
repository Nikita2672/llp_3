#include "net/socket_wrapper.h"
#include "transfer/from_xml.h"
#include "database/include/file/tableBlocks.h"
#include "database/include/util/unitTests.h"
#include "database/src/util/LoadData.h"

char *insertXML();

char *deleteXML();

char *updateXML();

char *selectWithJoin();

char *selectWithCond();

char *selectXML();

char *selectWithJoinAndCond();

xmlNodePtr getCond();


void preparing1(FILE *file) {
    test1(file);
    test2(file);
    test5(file);
    test10(file);
}

int main(int argc, char *argv[]) {
//    printf("%d", validateXmlAgainstSchemaFile(selectWithJoinAndCond(), "/home/iwaa0303/CLionProjects/llp_3/server/schema/query_schema.xsd"));
//    if (argc != 3) {
//        fprintf(stderr, "Использование: %s <server_ip> <server_port>\n", argv[0]);
//        return 1;
//    }
//    const char *filePath = argv[1];
//    int server_port = atoi(argv[2]);
    char *filePath = "/home/iwaa0303/CLionProjects/llp_3/server/database/test/file.bin";
    int server_port = 8095;
    FILE *file = fopen(filePath, "rb+");


    int server_socket = create_server_socket();
    bind_socket(server_socket, server_port);
    listen_for_connections(server_socket);

    printf("Server waiting for connections...\n");

    int client_socket = accept_connection(server_socket);
    printf("Connection established with client.\n");
    loadData(file);
    int times = 10;
    while (times) {
        times--;
        char buffer[MAX_BUFFER_SIZE];
        receive_data(client_socket, buffer);

        printf("Received from client: %s\n", buffer);

        char *xml = buffer;
        from_xml(xml, file, client_socket);
    }

    close_socket(client_socket);
    close_socket(server_socket);

    return 0;
}

xmlNodePtr getCond() {
    char *xml = "    <AND>"
                "      <condition>"
                "        <leftOp>fieldString</leftOp>"
                "        <operator>==</operator>"
                "        <rightOp>\"String\"</rightOp>"
                "      </condition>"
                "      <OR>"
                "        <condition>"
                "          <leftOp>field</leftOp>"
                "          <operator>&lt;</operator>"
                "          <rightOp>23</rightOp>"
                "        </condition>"
                "        <condition>"
                "          <leftOp>zxc</leftOp>"
                "          <operator>&gt;=</operator>"
                "          <rightOp>34.400000</rightOp>"
                "        </condition>"
                "      </OR>"
                "      <OR>"
                "        <condition>"
                "          <leftOp>field</leftOp>"
                "          <operator>&lt;</operator>"
                "          <rightOp>23</rightOp>"
                "        </condition>"
                "        <condition>"
                "          <leftOp>zxc</leftOp>"
                "          <operator>&gt;=</operator>"
                "          <rightOp>34.400000</rightOp>"
                "        </condition>"
                "      </OR>"
                "      <OR>"
                "        <condition>"
                "          <leftOp>field</leftOp>"
                "          <operator>&lt;</operator>"
                "          <rightOp>23</rightOp>"
                "        </condition>"
                "        <condition>"
                "          <leftOp>zxc</leftOp>"
                "          <operator>&gt;=</operator>"
                "          <rightOp>34.400000</rightOp>"
                "        </condition>"
                "      </OR>"
                "      <condition>"
                "        <leftOp>fieldString</leftOp>"
                "        <operator>==</operator>"
                "        <rightOp>\"String\"</rightOp>"
                "      </condition>"
                "    </AND>";
    xmlDocPtr doc = xmlReadMemory(xml, strlen(xml), "noname.xml", NULL, 0);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    return root;
}

char *deleteXML() {
    return "<Root>"
           "  <requestType>DELETE_QUERY</requestType>"
           "  <tableName>table</tableName>"
           "  <filter>"
           "    <AND>"
           "      <OR>"
           "        <condition>"
           "          <leftOp>fieldString</leftOp>"
           "          <operator>==</operator>"
           "          <rightOp>\"String\"</rightOp>"
           "        </condition>"
           "        <AND>"
           "          <condition>"
           "            <leftOp>fieldInteger</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>23</rightOp>"
           "          </condition>"
           "          <condition>"
           "            <leftOp>fieldDouble</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>2.710000</rightOp>"
           "          </condition>"
           "        </AND>"
           "      </OR>"
           "      <condition>"
           "        <leftOp>zxc</leftOp>"
           "        <operator>&gt;</operator>"
           "        <rightOp>23</rightOp>"
           "      </condition>"
           "    </AND>"
           "  </filter>"
           "</Root>";;
}

char *insertXML() {
    return "<Root>"
           "  <requestType>INSERT_QUERY</requestType>"
           "  <tableName>User</tableName>"
           "  <insertValues>"
           "    <field>"
           "      <type>STRING_LITERAL</type>"
           "      <value>\"Lubovv\"</value>"
           "    </field>"
           "    <field>"
           "      <type>STRING_LITERAL</type>"
           "      <value>\"Vitalievna\"</value>"
           "    </field>"
           "    <field>"
           "      <type>INTEGER_LITERAL</type>"
           "      <value>20</value>"
           "    </field>"
           "    <field>"
           "      <type>DOUBLE_LITERAL</type>"
           "      <value>357</value>"
           "    </field>"
           "    <field>"
           "      <type>BOOL_LITERAL</type>"
           "      <value>\"false\"</value>"
           "    </field>"
           "  </insertValues>"
           "</Root>";
}

char *updateXML() {
    return "<Root>"
           "  <requestType>UPDATE_QUERY</requestType>"
           "  <tableName>users</tableName>"
           "  <updateField>"
           "    <updateFieldName>name</updateFieldName>"
           "    <updateFieldValue>\"Alexey\"</updateFieldValue>"
           "  </updateField>"
           "  <filter>"
           "    <AND>"
           "      <OR>"
           "        <condition>"
           "          <leftOp>fieldString</leftOp>"
           "          <operator>==</operator>"
           "          <rightOp>\"String\"</rightOp>"
           "        </condition>"
           "        <AND>"
           "          <condition>"
           "            <leftOp>fieldInteger</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>23</rightOp>"
           "          </condition>"
           "          <condition>"
           "            <leftOp>fieldDouble</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>2.710000</rightOp>"
           "          </condition>"
           "        </AND>"
           "      </OR>"
           "      <condition>"
           "        <leftOp>zxc</leftOp>"
           "        <operator>&gt;</operator>"
           "        <rightOp>23</rightOp>"
           "      </condition>"
           "    </AND>"
           "  </filter>"
           "</Root>";
}

char *selectXML() {
    return "<Root>"
           "  <requestType>SELECT_QUERY</requestType>"
           "  <tableName>User</tableName>"
           "  <selectedVal>"
           "    <entity>user</entity>"
           "  </selectedVal>"
           "</Root>";
}

char *selectWithCond() {
    return "<Root>"
           "  <requestType>SELECT_QUERY</requestType>"
           "  <tableName>User</tableName>"
           "  <filter>"
           "    <AND>"
           "      <condition>"
           "        <leftOp>Age</leftOp>"
           "        <operator>&gt;</operator>"
           "        <rightOp>19</rightOp>"
           "      </condition>"
           "      <condition>"
           "        <leftOp>Score</leftOp>"
           "        <operator>==</operator>"
           "        <rightOp>128.000000</rightOp>"
           "      </condition>"
           "    </AND>"
           "  </filter>"
           "  <selectedVal>"
           "    <entity>user</entity>"
           "  </selectedVal>"
           "</Root>";
}

char *selectWithJoinAndCond() {
    return "<Root>"
           "  <requestType>SELECT_QUERY</requestType>"
           "  <tableName>invoiceCollection</tableName>"
           "  <filter>"
           "    <AND>"
           "      <OR>"
           "        <condition>"
           "          <leftOp>fieldString</leftOp>"
           "          <operator>==</operator>"
           "          <rightOp>\"String\"</rightOp>"
           "        </condition>"
           "        <AND>"
           "          <condition>"
           "            <leftOp>fieldInteger</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>23</rightOp>"
           "          </condition>"
           "          <condition>"
           "            <leftOp>fieldDouble</leftOp>"
           "            <operator>==</operator>"
           "            <rightOp>2.710000</rightOp>"
           "          </condition>"
           "        </AND>"
           "      </OR>"
           "      <condition>"
           "        <leftOp>zxc</leftOp>"
           "        <operator>&gt;</operator>"
           "        <rightOp>23</rightOp>"
           "      </condition>"
           "    </AND>"
           "  </filter>"
           "  <Join>"
           "    <leftOperand>"
           "      <leftTable>invoice</leftTable>"
           "      <leftField>invoiceTypeId</leftField>"
           "    </leftOperand>"
           "    <rightOperand>"
           "      <rightTable>invoiceType</rightTable>"
           "      <rightField>id</rightField>"
           "    </rightOperand>"
           "  </Join>"
           "  <selectedVal>"
           "    <entity>invoice</entity>"
           "  </selectedVal>"
           "</Root>";
}

char *selectWithJoin() {
    return "<Root>"
           "  <requestType>SELECT_QUERY</requestType>"
           "  <tableName>Employee</tableName>"
           "  <join>"
           "    <leftOperand>"
           "      <leftTable>Employee</leftTable>"
           "      <leftField>DepartmentId</leftField>"
           "    </leftOperand>"
           "    <rightOperand>"
           "      <rightTable>Department</rightTable>"
           "      <rightField>DepartmentId</rightField>"
           "    </rightOperand>"
           "  </join>"
           "  <selectedVal>"
           "    <entity>Employee</entity>"
           "  </selectedVal>"
           "</Root>";
}
