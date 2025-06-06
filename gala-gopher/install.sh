#!/bin/bash

PROGRAM=$0
PROJECT_FOLDER=$(dirname $(readlink -f "$0"))
EXT_PROBE_FOLDER=${PROJECT_FOLDER}/src/probes/extends
EXT_PROBE_INSTALL_LIST=`find ${EXT_PROBE_FOLDER} -maxdepth 2 | grep "\<install.sh\>"`

TAILOR_PATH=${PROJECT_FOLDER}/tailor.conf
TAILOR_PATH_TMP=${TAILOR_PATH}.tmp

function load_tailor()
{
    if [ -f ${TAILOR_PATH} ];then
        cp ${TAILOR_PATH} ${TAILOR_PATH_TMP}

        sed -i '/^$/d' ${TAILOR_PATH_TMP}
        sed -i 's/ //g' ${TAILOR_PATH_TMP}
        sed -i 's/^/export /' ${TAILOR_PATH_TMP}
        eval `cat ${TAILOR_PATH_TMP}`
        rm -rf ${TAILOR_PATH_TMP}
    fi
}

function install_daemon_bin()
{
    GOPHER_BIN_FILE=${PROJECT_FOLDER}/gala-gopher
    GOPHER_BIN_TARGET_DIR=/usr/bin

    if [ $# -eq 1 ]; then
        GOPHER_BIN_TARGET_DIR=$1
    fi

    cd ${PROJECT_FOLDER}
    if [ ! -f ${GOPHER_BIN_FILE} ]; then
        echo "${GOPHER_BIN_FILE} not exist. please check if build success."
        exit 1
    fi

    # install gala-gopher bin
    cp -f ${GOPHER_BIN_FILE} ${GOPHER_BIN_TARGET_DIR}
    echo "install ${GOPHER_BIN_FILE} success."

}

function install_conf()
{
    GOPHER_CONF_FILE=${PROJECT_FOLDER}/config/gala-gopher.conf
    TASKPROBE_WHITELIST_FILE=${PROJECT_FOLDER}/config/task_whitelist.conf
    GOPHER_CONF_TARGET_DIR=/opt/gala-gopher

    if [ $# -eq 1 ]; then
        GOPHER_CONF_TARGET_DIR=$1
    fi

    cd ${PROJECT_FOLDER}
    if [ ! -f ${GOPHER_CONF_FILE} ]; then
        echo "${GOPHER_CONF_FILE} not exist. please check ./config dir."
        exit 1
    fi
    if [ ! -f ${TASKPROBE_WHITELIST_FILE} ]; then
        echo "${TASKPROBE_WHITELIST_FILE} not exist. please check ./config dir."
    fi

    # install gala-gopher.conf
    if [ ! -d ${GOPHER_CONF_TARGET_DIR} ]; then
        mkdir ${GOPHER_CONF_TARGET_DIR}
    fi
    cp -f ${GOPHER_CONF_FILE} ${GOPHER_CONF_TARGET_DIR}
    echo "install ${GOPHER_CONF_FILE} success."
    cp -f ${TASKPROBE_WHITELIST_FILE} ${GOPHER_CONF_TARGET_DIR}
    echo "install ${TASKPROBE_WHITELIST_FILE} success."

}

function install_meta()
{
    GOPHER_META_DIR=/opt/gala-gopher/meta

    if [ $# -eq 1 ]; then
        GOPHER_META_DIR=$1/meta
    fi

    cd ${PROJECT_FOLDER}

    # install meta files
    if [ ! -d ${GOPHER_META_DIR} ]; then
        mkdir -p ${GOPHER_META_DIR}
    fi
    META_FILES=`find ${PROJECT_FOLDER}/src -name "*.meta"`
    for file in ${META_FILES}
    do
        cp ${file} ${GOPHER_META_DIR}
    done
    echo "install meta file success."

}

function install_extend_probes()
{
    GOPHER_EXTEND_PROBE_DIR=/opt/gala-gopher/extend_probes

    if [ $# -eq 1 ]; then
        GOPHER_EXTEND_PROBE_DIR=$1/extend_probes
    fi

    if [ ! -d ${GOPHER_EXTEND_PROBE_DIR} ]; then
        mkdir -p ${GOPHER_EXTEND_PROBE_DIR}
    fi

    cd ${PROJECT_FOLDER}

    # Search for install.sh in extend probe directory
    cd ${EXT_PROBE_FOLDER}
    for INSTALL_PATH in ${EXT_PROBE_INSTALL_LIST}
    do
        echo "install path:" ${INSTALL_PATH}
        ${INSTALL_PATH} ${GOPHER_EXTEND_PROBE_DIR}
    done
}


function install_client_bin()
{
    CLI_BIN_FILE=${PROJECT_FOLDER}/gopher-ctl
    CLI_BIN_TARGET_DIR=/usr/bin

    if [ $# -eq 1 ]; then
        CLI_BIN_TARGET_DIR=$1
    fi

    cd ${PROJECT_FOLDER}
    if [ ! -f ${CLI_BIN_FILE} ]; then
        echo "${CLI_BIN_FILE} does not exist. Please check if build success."
        exit 1
    fi

    # install gopher-cli bin
    cp -f ${CLI_BIN_FILE} ${CLI_BIN_TARGET_DIR}
    echo "install ${CLI_BIN_FILE} success."

}

# main process
load_tailor
install_daemon_bin $1
install_conf $2
install_meta $2
install_extend_probes $2
install_client_bin $1
