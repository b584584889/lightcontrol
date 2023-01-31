/*
 * DBManager.h
 *
 *  Created on: 2019/09/17
 *      Author: ikki
 */

#ifndef DBManager_H
#define DBManager_H


#include "../config.h"
#include "../common.h"

#include "ConfigDBAccess.h"
#include "MySQLDBAccess.h"
#include "TimeSerialsDBAccess.h"
#include "TrackDBAccess.h"
#include "FootprintDBAccess.h"

typedef struct
{
   int projectid;
   VILSSystemConfig systemConfig;
} ProjectSystemConfigConfig;

class DBManager
{
public:
	DBManager();
	~DBManager();

    bool init();

    void reload_sys_config();
    void reload_prj_config();
    std::list<int> * getAllProjectConfig();
    bool validProject(int projectID);
    MySQLDBAccess * getBaseDB(int projectID);
    TimeSerialsDBAccess * getTSDB(int projectID);
    TrackDBAccess * getTrackDB(int projectID);
    FootprintDBAccess * getFootprintDB(int projectID);
    VILSSystemConfig * get_sys_config(int projectID);

    int getDatabaseSize(int projectID);

    bool update_config(VILSConfig config);
    bool delete_config(int projectID);
private:
    ConfigDBAccess * m_ConfigDB;
    std::vector<MySQLDBAccess *> m_MysqlDBList;
    std::vector<TimeSerialsDBAccess *> m_MysqlTSDBList;
    std::vector<TrackDBAccess *> m_TrackDBList;
    std::vector<FootprintDBAccess *> m_FootprintDBList;

    std::list<ProjectConfig*> * m_ConfigList;
    std::list<ProjectSystemConfigConfig*> m_PrjSysConfigList;

    void create_new_config();
    void copy_map_to_maplayer();
    void copy_maparea_to_maplayerarea();

};

#endif
