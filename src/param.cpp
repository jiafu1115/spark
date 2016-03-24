#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>

#include "param.hpp"
#include "libspark.hpp"

namespace {

Param::e_object resolve_object(const std::string & value)
{
    Param::e_object param = Param::OBJ_UNDEFINED;
    if (value.find("message") != std::string::npos)
        param = Param::OBJ_MESSAGE;
    else if (value.find("room") != std::string::npos)
        param = Param::OBJ_ROOM;
    else if (value.find("people") != std::string::npos)
        param = Param::OBJ_PERSON;
    else  {
        std::cerr << "error, \"" << optarg << "\"" << "parameter must be one of: message or room" << std::endl;
        exit(1);
    }
    return param;
}

Param::e_id resolve_id_type(const std::string id)
{
    if (id.empty())
        return Param::ID_UNDEFINED;

    if (id.find("@") != std::string::npos)
        return Param::ID_EMAIL;

    const std::size_t hash_length = 36;
    const std::size_t hash_delims = 4;
    bool is_hash = ((id.length() == hash_length) &&
                    (std::count(id.begin(), id.end(), '-') == hash_delims));
    if (is_hash)
        return Param::ID_HASH;
    return Param::ID_NAME;
}

} // namespace


Param::Param(int argc, char ** argv)
        : argc(argc)
        , argv(argv)
        , verbose(false)
        , auth()
        , id()
        , url()
        , data()
        , type(ID_UNDEFINED)
        , verb(VRB_UNDEFINED)
        , object(OBJ_UNDEFINED)
        , cmd(CMD_UNDEFINED)
{
    while (true) {
        static struct option long_options[] = {
            { "list",    1, 0, 'l' },
            { "create",  1, 0, 'c' },
            { "delete",  1, 0, 'd' },
            { "auth",    1, 0, 'a' },
            { "id",      1, 0, 'i' },
            { "verbose", 0, 0, 'v' },
            { nullptr,   0, 0,  0  }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "l:c:d:a:i:v", long_options, &option_index);
        if (-1 == c)
            break;

        switch (c) {
            // VERBs:
            case 'l': {
                verb = Param::VRB_LIST;
                object = resolve_object(optarg);
                switch(object) {
                    case Param::OBJ_MESSAGE: cmd = Param::CMD_LIST_MESSAGES; break;
                    case Param::OBJ_ROOM:    cmd = Param::CMD_LIST_ROOMS; break;
                    case Param::OBJ_PERSON:  cmd = Param::CMD_LIST_PEOPLE; break;
                    case Param::OBJ_UNDEFINED: default: break;
                }
                break;
            }
            case 'c': {
                verb = Param::VRB_CREATE;
                object = resolve_object(optarg);
                switch(object) {
                    case Param::OBJ_MESSAGE: cmd = Param::CMD_CREATE_MESSAGE; break;
                    case Param::OBJ_ROOM:    cmd = Param::CMD_CREATE_ROOM; break;
                    case Param::OBJ_UNDEFINED: default: break;
                }
                break;
            }
            case 'd': {
                verb = Param::VRB_DELETE;
                object = resolve_object(optarg);
                switch(object) {
                    case Param::OBJ_MESSAGE: cmd = Param::CMD_DELETE_MESSAGE; break;
                    case Param::OBJ_ROOM:    cmd = Param::CMD_DELETE_ROOM; break;
                    case Param::OBJ_UNDEFINED: default: break;
                }
                break;
            }
            // params:
           case 'a':
                auth = optarg;
                break;
            case 'i':
                id = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                break;
        }
    }
    if (argc > optind) // TODO: vector
        data = argv[optind];

    if (auth.empty())
        auth = getenv("SPARK_AUTH");
}

std::string Param::resolve_spark_id(Spark & spark, const std::string & id)
{
    if (id.empty())
        return "";
    type = resolve_id_type(id);
    std::string spark_id = "";

#if 0
    if (!id.empty()) {
        if (object == Param::OBJ_PERSON) // might be meta..
            return spark_id;
    }
#endif

    if (type == Param::ID_HASH)
        return id;
    if (type == Param::ID_EMAIL)
        return spark.get_id_by_email(id);

    switch (object) {
        case Param::OBJ_MESSAGE:
            return spark.get_id_by_room_name(id);
        case Param::OBJ_ROOM:
            return spark.get_id_by_room_name(id);
        case Param::OBJ_PERSON:
            // resolve person by start "regexp"
            break;
        case Param::OBJ_UNDEFINED:
            std::cerr << "Error, should not be here: "<<  __func__ << std::endl;
    }

    return "";
}


