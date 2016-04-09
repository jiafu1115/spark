#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>

#include "param.hpp"
#include "libspark.hpp"

int print_usage()
{
    printf(
        "Usage: spark [VERB] [OBJECT] [IDENTIFIER..] <data..>\n"
        "     VERBS:\n"
        "        -l,  --list    [OBJECT]  [IDENTIFIER]          Show OBJECTs associated with roomId,email,..\n"
        "        -c,  --create  [OBJECT]  [IDENTIFIER]          Create OBJECT with recipient,name,..\n"
        "        -d,  --delete  [OBJECT]  [IDENTIFIER]          Delte OBJECT by id:room\n"
        "\n"
        "     OBJECTS:\n"
        "        message                                        Perform VERB on message(s)\n"
        "        room                                           Perform VERB on room\n"
        "        people                                         Perform VERB on people/person\n"
        "        membership                                     Perform VERB on subscription\n"
        "\n"
        "     PARAMETERS:\n"
        "        -a,  --auth      token                         Access token\n"
        "                                                       Preferably specified via the SPARK_AUTH env. variable\n"
        "        -i,  --id        roomId/messageId/email        Recipient identifier\n"
        "        -v,  --verbose                                 Show verbose information\n"
        "        -h,  --help                                    Show this information\n"
        "\n"
        "Example usage:\n"
        "Get your access token by clicking on your avatar/name in the top right corner here: https://developer.ciscospark.com\n"
        "\n"
        "export SPARK_AUTH=NjYzMTA0Y2MtN2VjNS00MDE5LTkwMDUtMDI4MjJjMGE1ODBiY2FiNDg1NWItNzI1\n"
        "\n"
        "   spark --list   rooms                                Show subscribed rooms \n"
        "   spark --list   messages --id 'Hackers Corner'       Show message(s) posted to 'Hackers Corner'\n"
        "   spark --list   people   --id pwahlman@cisco.com     Show details for mail address\n"
        "   spark --list   people   --id petter                 Show all users with name starting with 'petter'\n"
        "   spark --list   people   --id me                     Show information about yourself\n"
        "   spark --create message  --id pwahlman@cisco.com     Send personal message\n"
        "   spark --create message  --id 'Hackers Corner'       Send message to room\n"
        "   spark --delete room     --id 'Deleteme'             Delete room (must be owner/admin)\n"
        "   spark --delete message  --id HASH                   Delete message by id-hash (must be owner/admin)\n"
        "\n"

    );
    exit(1);
}

int main(int argc, char ** argv)
{
    Param opt(argc, argv);
    if (opt.auth.empty()) {
        std::cerr << "Error, no authentication token" << std::endl << std::endl;
        print_usage();
    }

    if (opt.verb == Param::VRB_UNDEFINED) {
        std::cerr << "Error, no VERB specified" << std::endl << std::endl;
        print_usage();
    }

    if (opt.object == Param::OBJ_UNDEFINED) {
        std::cerr << "Error, no OBJECT specified" << std::endl << std::endl;
        print_usage();
    }

    if (opt.cmd == Param::CMD_UNDEFINED) { // should not happend
        std::cerr << "Error, no valid VERB/OBJECT combination specified" << std::endl << std::endl;
        print_usage();
    }

    libspark::client spark(opt.auth);
    if (opt.verbose)
        spark.debug(true);

    std::string spark_id = opt.resolve_spark_id(spark, opt.id);
    if (!spark_id.empty())
        opt.id = spark_id;
    switch (opt.cmd) {
        // LIST
        case Param::CMD_LIST_MESSAGES: {
            if (opt.id.empty()) {
                std::cerr << "Error, no message id/room specified" << std::endl << std::endl;
                print_usage();
            }
            std::deque<libspark::message> messages = spark.list_messages(opt.id);
            for (auto & message : messages)
                std::cout << message.id << " " << "  " << std::left << std::setw(20) << message.personEmail << "   \"" << message.text << "\"" << std::endl;
            break;
        }
        case Param::CMD_LIST_ROOMS: {
            std::deque<libspark::room> rooms = spark.list_rooms(opt.id);
            for (auto & room : rooms)
                std::cout << room.id << " \"" << room.title << "\"" << std::endl;
            break;
        }
        case Param::CMD_LIST_PEOPLE: {
            if (opt.id.empty()) {
                if (opt.data.empty()) {
                    std::cerr << "Error, no id/name/email specified" << std::endl << std::endl;
                    print_usage();
                }
                opt.id = opt.data; // "regexp"
            }
            std::deque<libspark::person> people = spark.list_people(opt.id);
            for (auto & person : people) {
                if (!person.emails.empty())
                    std::cout << person.id << " <" << person.emails[0] << ">  " << person.displayname << std::endl;
                //std::cout << person.id << "   " << person.avatar << std::endl;
            }
            break;
        }
        case Param::CMD_LIST_MEMBERSHIPS: {
            std::deque<libspark::membership> memberships = spark.list_memberships(opt.id);
            for (auto & elem : memberships)
                std::cout << elem.id <<  " mod: " << elem.isModerator << " mon: " <<  elem.isMonitor << " " << spark.get_room_name_by_id(elem.roomId) << std::endl;
            break;
        }

        // CREATE
        case Param::CMD_CREATE_MESSAGE: {
            if (opt.id.empty()) {
                std::cerr << "Error, no id/room specified" << std::endl << std::endl;
                print_usage();
            }
            if (opt.data.empty()) {
                std::cerr << "Error, no message specified" << std::endl << std::endl;
                print_usage();
            }
            if (opt.type == Param::ID_EMAIL)
                spark.create_pm(opt.data, opt.id);
            else
                spark.create_message(opt.data, opt.id);
            break;
        }
        case Param::CMD_CREATE_ROOM: {
            if (opt.data.empty()) {
                std::cerr << "Error, no name specified" << std::endl << std::endl;
                print_usage();
            }
            spark.create_room(opt.data);
            break;
        }
        case Param::CMD_CREATE_MEMBERSHIP: {
            if (opt.id.empty()) {
                std::cerr << "Error, no roomId specified" << std::endl << std::endl;
                print_usage();
            }
            std::deque<libspark::message> messages = spark.list_messages(opt.id);
            if (opt.data.empty()) {
                std::cerr << "Error, no email specified" << std::endl << std::endl;
                print_usage();
            }
            spark.create_membership(opt.id, opt.data);
            break;
        }

        // DELETE
        case Param::CMD_DELETE_MESSAGE:
            if (opt.id.empty()) {
                std::cerr << "Error, no id specified" << std::endl << std::endl;
                print_usage();
            }
            spark.delete_message(opt.id);
            break;
        case Param::CMD_DELETE_ROOM:
            if (opt.id.empty()) {
                std::cerr << "Error, no id/name specified" << std::endl << std::endl;
                print_usage();
            }
            spark.delete_room(opt.id);
            break;
        default:
            break;
    }

    return 0;
}

