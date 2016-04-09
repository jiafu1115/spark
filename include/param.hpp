#ifndef PARAM_HPP
#define PARAM_HPP

#include "libspark.hpp"

class Param {
public:
    enum e_verb {
        VRB_UNDEFINED = -1,
        VRB_LIST,
        VRB_CREATE,
        VRB_DELETE,
    };
    enum e_object {
        OBJ_UNDEFINED = -1,
        OBJ_MESSAGE,
        OBJ_ROOM,
        OBJ_PERSON,
        OBJ_MEMBERSHIP,
    };
    // superset of e_verb and e_object
    enum e_cmd {
        CMD_UNDEFINED = -1,
        CMD_LIST_MESSAGES,
        CMD_LIST_ROOMS,
        CMD_LIST_PEOPLE,
        CMD_LIST_MEMBERSHIPS,
        CMD_CREATE_MESSAGE,
        CMD_CREATE_ROOM,
        CMD_CREATE_MEMBERSHIP,
        CMD_DELETE_MESSAGE,
        CMD_DELETE_ROOM,
    };
    enum e_id {
        ID_UNDEFINED = -1,
        ID_HASH,
        ID_NAME,
        ID_EMAIL
    };
    Param(int argc, char ** argv);
    std::string resolve_spark_id(libspark::client & spark, const std::string & id);

    int argc;
    char ** argv;
    bool verbose;
    std::string auth;
    std::string id;
    std::string url;
    std::string data;
    e_id type;
    e_verb verb;
    e_object object;
    e_cmd cmd;
};


#endif
