#include "socket_wrapper.h"
#include "transfer/from_xml.h"
#include "database/include/file/tableBlocks.h"
#include "database/include/file/fileApi.h"
#include "database/include/util/unitTests.h"

char *insertXML();

char *deleteXML();

char *updateXML();

xmlNodePtr getCond();

int main() {
//    test1();
//    test2();
//    test5();
    FILE *file = fopen("/home/iwaa0303/CLionProjects/llp_3/server/database/test/file.bin", "rb+");
    char *xml = updateXML();

    printf("%s\n", from_xml(xml, file));

//    xmlNodePtr root = getCond();
//    printf("%s\n", traverseChildren(root, 3)->name);


//    int server_socket = create_server_socket();
//    bind_socket(server_socket);
//    listen_for_connections(server_socket);
//
//    printf("Server waiting for connections...\n");
//
//    int client_socket = accept_connection(server_socket);
//    printf("Connection established with client.\n");
//
//    int times = 5;
//    while (times) {
//        times--;
//        char buffer[MAX_BUFFER_SIZE];
//        receive_data(client_socket, buffer);
//
//        printf("Received from client: %s\n", buffer);
//
//        // Echo the received data back to the client
//        send_data(client_socket, buffer);
//    }
//
//    close_socket(client_socket);
//    close_socket(server_socket);

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