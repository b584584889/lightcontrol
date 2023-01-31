/*
 * DBManager.cpp
 *
 *  Created on: 2019/09/17
 *      Author: ikki
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "DBManager.h"
using namespace std;

DBManager::DBManager()
{
}

DBManager::~DBManager()
{
}

bool DBManager::init()
{
    m_MysqlDBList.clear();
    m_MysqlTSDBList.clear();
    m_TrackDBList.clear();
    m_FootprintDBList.clear();

    m_PrjSysConfigList.clear();

    vilslog::printf("DBManager::init() m_MysqlDBList.size()[%d]\n", (int)m_MysqlDBList.size());

    if ((int)m_MysqlDBList.size() == 0)
    {
        vilslog::printf("DBManager::init() new MySQLDBAccess(1)\n");

        MySQLDBAccess * mysqlDB = new MySQLDBAccess(1);

        vilslog::printf("DBManager::init() new MySQLDBAccess Over\n");
        // wait MySql server wakeup
        for(int i=0;i<200;i++)
        {
            if (mysqlDB->checkMySQLConnection())
            {
                vilslog::printf("DBManager::init() checkMySQLConnection OK!!!!\n");
                //sleep(1);
                break;
            }
            else
            {
                vilslog::printf("DBManager::init() checkMySQLConnection failed!, i = %d\n", i);
                sleep(3);
            }
        }

        vilslog::printf("DBManager::init() MySQLDBAccess check_valid_table.....\n");
        mysqlDB->check_valid_table();
        vilslog::printf("DBManager::init() MySQLDBAccess check_valid_table finished!\n");

        m_MysqlDBList.push_back(mysqlDB);
    }

    vilslog::printf("DBManager::init() m_MysqlTSDBList.size()[%d]\n", (int)m_MysqlTSDBList.size());
    if ((int)m_MysqlTSDBList.size() == 0)
    {
        TimeSerialsDBAccess * mysqTSlDB = new TimeSerialsDBAccess(1);
        mysqTSlDB->check_valid_table();
        m_MysqlTSDBList.push_back(mysqTSlDB);
    }

    vilslog::printf("DBManager::init() m_TrackDBList.size()[%d]\n", (int)m_TrackDBList.size());
    if (m_TrackDBList.size() == 0)
    {
        TrackDBAccess * trackDB = new TrackDBAccess(1);
        trackDB->check_valid_table();
        m_TrackDBList.push_back(trackDB);
    }

    vilslog::printf("DBManager::init() m_FootprintDBList.size()[%d]\n", (int)m_FootprintDBList.size());
    if (m_FootprintDBList.size() == 0)
    {
        FootprintDBAccess * fpDB = new FootprintDBAccess(1);
        fpDB->check_valid_table();
        m_FootprintDBList.push_back(fpDB);
    }

    m_ConfigDB = new ConfigDBAccess();
    m_ConfigDB->check_valid_table();
    m_ConfigList = m_ConfigDB->read_all_config();

    vilslog::printf("DBManager::init() m_ConfigList->size()[%d]\n", (int)m_ConfigList->size());
    if (m_ConfigList->size() == 0)
    {
        create_new_config();

        //
        // copy old map to new maplayer;
        //
        copy_map_to_maplayer();

        //
        // copy old maparea to new maplayerarea;
        //
        copy_maparea_to_maplayerarea();
    }

    //
    // get system config
    //
    for (std::list<ProjectConfig*>::iterator prj = m_ConfigList->begin(); prj != m_ConfigList->end(); prj++)
    {
        int projectId = (*prj)->projectid;
        MySQLDBAccess * baseDb = getBaseDB(projectId);

        vilslog::printf("DBManager::init() projectId[%d]\n", projectId);

        ProjectSystemConfigConfig * newPrj = new ProjectSystemConfigConfig();
        newPrj->projectid = projectId;
        baseDb->read_sys_config(&newPrj->systemConfig);
        m_PrjSysConfigList.push_back(newPrj);

        bool updateNew = false;
        //check VILSServer
        if ( newPrj->systemConfig.VILSServer.length() == 0 )
        {
            newPrj->systemConfig.VILSServer = "http://127.0.0.1/";
            vilslog::printf("DBManager::init() projectId[%d] update VILSServer[%s]\n", projectId, newPrj->systemConfig.VILSServer.c_str());
            updateNew = true;
        }

        //check VILSUID
        if ( newPrj->systemConfig.VILSUID.length() == 0 )
        {
            newPrj->systemConfig.VILSUID = random_string(8);
            vilslog::printf("DBManager::init() projectId[%d] update VILSUID[%s]\n", projectId, newPrj->systemConfig.VILSUID.c_str());
            updateNew = true;
        }

        if ( updateNew )
        {
            baseDb->update_systemconfigudpport(newPrj->systemConfig);
        }

    }
    vilslog::printf("DBManager::init() m_PrjSysConfigList.size()[%d]\n", (int)m_PrjSysConfigList.size());

    return true;
}

void DBManager::create_new_config()
{
    MySQLDBAccess * mysqlDB = m_MysqlDBList[0];
    std::list<VILSConfig*> * allConfigList = mysqlDB->read_all_config();

    //
    // copy old config
    //
    if (allConfigList->size() > 0)
    {
        //
        // copy existed project info
        //
        for (std::list<VILSConfig*>::iterator config = allConfigList->begin(); config != allConfigList->end(); config++)
        {
            ProjectConfig * newconfig = new ProjectConfig();

            newconfig->projectid = (*config)->projectid;
            newconfig->projectname = (*config)->projectname;
            newconfig->projectdesc = (*config)->projectdesc;

            m_ConfigList->push_back(newconfig);

            vilslog::printf("DBManager::create_new_config() copy existed project info projectid[%d] projectname[%s] projectdesc[%s]\n",
                (*config)->projectid, (*config)->projectname.c_str(), (*config)->projectdesc.c_str());

            m_ConfigDB->write_config(newconfig->projectid, newconfig->projectname, newconfig->projectdesc);

            //
            // only create first one
            //
            break;
        }
    }
    else
    {
        // create new one
        ProjectConfig * newconfig = new ProjectConfig();

        newconfig->projectid = 1;
        newconfig->projectname = "New Project";
        newconfig->projectdesc = "SMIMS VILS IPS Solutions";

        m_ConfigList->push_back(newconfig);

        m_ConfigDB->write_config(newconfig->projectid, newconfig->projectname, newconfig->projectdesc);
    }

    clear_Config_list(allConfigList);
    delete allConfigList;
}

//
// copy old map to new maplayer;
//
void DBManager::copy_map_to_maplayer()
{
    // only check project_id = 1
    MySQLDBAccess * mysqlDB = m_MysqlDBList[0];
    std::list<VILSConfig*> * allConfigList = mysqlDB->read_all_config();
    for (std::list<VILSConfig*>::iterator config = allConfigList->begin(); config != allConfigList->end(); config++)
    {
        MapStruct * map = new MapStruct();
        mysqlDB->read_map((*config)->projectid, map);

        MapLayerStruct * maplayer = new MapLayerStruct();
        maplayer->projectid = (*config)->projectid;
        maplayer->layerid = 0;
        maplayer->maingroupid = 1;
        maplayer->layer_name = (*config)->projectname;
        maplayer->layer_description = (*config)->projectdesc;
        maplayer->WGS48OriginX = (*config)->WGS48_Origin_X;
        maplayer->WGS48OriginY = (*config)->WGS48_Origin_Y;
        maplayer->MAPNorth = (*config)->MAP_North;
        maplayer->BuildingID = (*config)->BuildingID;
        maplayer->BuildingFloor = (*config)->BuildingFloor;
        maplayer->POSX = map->POSX;
        maplayer->POSY = map->POSY;
        maplayer->SCALE = map->SCALE;
        maplayer->COEFF = map->COEFF;
        maplayer->FILE = map->FILE;
        maplayer->MAPWIDTH = map->MAPWIDTH;
        maplayer->MAPHEIGHT = map->MAPHEIGHT;

        mysqlDB->create_maplayer(maplayer);

        vilslog::printf("DBManager::copy_map_to_maplayer() copy existed project info projectid[%d] projectname[%s] WGS48_Origin[%s,%s]\n",
            (*config)->projectid, (*config)->projectname.c_str(), (*config)->WGS48_Origin_X.c_str(), (*config)->WGS48_Origin_Y.c_str());
        vilslog::printf("DBManager::copy_map_to_maplayer() projectid[%d] MAP_North[%s] BuildingID[%d] BuildingFloor[%d]\n",
            (*config)->projectid, (*config)->MAP_North.c_str(), (*config)->BuildingID, (*config)->BuildingFloor);
        vilslog::printf("DBManager::copy_map_to_maplayer() projectid[%d] FILE[%s] MAPWIDTH[%s] MAPHEIGHT[%s]\n",
            (*config)->projectid, map->FILE.c_str(), map->MAPWIDTH.c_str(), map->MAPHEIGHT.c_str());

        delete maplayer;
        delete map;
    }
}

//
// copy old maparea to new maplayerarea;
//
void DBManager::copy_maparea_to_maplayerarea()
{
    MySQLDBAccess * mysqlDB = m_MysqlDBList[0];
    std::list<MapLayerStruct*> * maplayerList = mysqlDB->read_maplayers();

    std::list<VILSConfig*> * allConfigList = mysqlDB->read_all_config();
    for (std::list<VILSConfig*>::iterator config = allConfigList->begin(); config != allConfigList->end(); config++)
    {
        MapStruct * map = new MapStruct();
        mysqlDB->read_map((*config)->projectid, map);

        // find the same map
        for (std::list<MapLayerStruct*>::iterator maplayer = maplayerList->begin(); maplayer != maplayerList->end(); maplayer++)
        {
            if ( ((*maplayer)->FILE.compare(map->FILE) == 0) && ((*maplayer)->layer_name.compare((*config)->projectname) == 0) )
            {
                //(*maplayer)->layerid;
                vilslog::printf("DBManager::copy_maparea_to_maplayerarea() projectid[%d] (*maplayer)->FILE[%s] layer_name[%s] layerid[%d]\n",
                    (*config)->projectid, (*maplayer)->FILE.c_str(), (*maplayer)->layer_name.c_str(), (*maplayer)->layerid);
                vilslog::printf("DBManager::copy_maparea_to_maplayerarea() projectid[%d] map->FILE[%s] projectname[%s]\n",
                    (*config)->projectid, map->FILE.c_str(), (*config)->projectname.c_str());

                std::list<AreaStruct*> * areaList = mysqlDB->read_area((*config)->projectid);

                vilslog::printf("DBManager::copy_maparea_to_maplayerarea() projectid[%d] to layerid[%d] areaList->size()[%d]\n",
                    (*config)->projectid, (*maplayer)->layerid, (int)areaList->size());

                for (std::list<AreaStruct*>::iterator area = areaList->begin(); area != areaList->end(); area++)
                {
                    vilslog::printf("DBManager::copy_maparea_to_maplayerarea() projectid[%d] layerid[%d] areaid[%s] areaname[%s]\n",
                        (*config)->projectid, (*maplayer)->layerid, (*area)->areaid.c_str(), (*area)->areaname.c_str());

                    MapLayerAreaStruct maplayerarea;
                    maplayerarea.layerid = (*maplayer)->layerid;
                    maplayerarea.areaid = (*area)->areaid;
                    maplayerarea.areaname = (*area)->areaname;
                    maplayerarea.geojson = (*area)->geojson;
                    maplayerarea.type = (*area)->type;
                    mysqlDB->create_maplayerarea(&maplayerarea);
                }

                clear_area_list(areaList);
                delete areaList;

                break;
            }
        }

        delete map;
    }

    clear_MapLayerStruct_list(maplayerList);
    delete maplayerList;
}

void DBManager::reload_sys_config()
{
//    std::list<ProjectConfig*> * tmp_ConfigList = m_ConfigDB->read_all_config();
//    std::list<ProjectConfig*> * old_ConfigList = m_ConfigList;
//
//    m_ConfigList = tmp_ConfigList;
//
//    clear_ProjectConfig_list(old_ConfigList);
//    delete old_ConfigList;
//
    //
    // get system config
    //
    for (std::list<ProjectConfig*>::iterator prj = m_ConfigList->begin(); prj != m_ConfigList->end(); prj++)
    {
        int projectid = (*prj)->projectid;
        MySQLDBAccess * baseDb = getBaseDB(projectid);

        for (std::list<ProjectSystemConfigConfig*>::iterator prjsys = m_PrjSysConfigList.begin(); prjsys != m_PrjSysConfigList.end(); prjsys++)
        {
            if ( (*prjsys)->projectid == projectid )
            {
                baseDb->read_sys_config(&(*prjsys)->systemConfig);
                break;
            }
        }
    }
}

void DBManager::reload_prj_config()
{
    if ( m_ConfigList != NULL)
    {
        vilslog::printf("DBManager::reload_prj_config() old m_ConfigList->size()[%d]\n", (int)m_ConfigList->size());

        clear_ProjectConfig_list(m_ConfigList);
        delete m_ConfigList;
    }

    m_ConfigList = m_ConfigDB->read_all_config();

    vilslog::printf("DBManager::reload_prj_config() new  m_ConfigList->size()[%d]\n", (int)m_ConfigList->size());
}

std::list<int> * DBManager::getAllProjectConfig()
{
    std::list<int> * prjidList = new std::list<int>();
    for (std::list<ProjectConfig*>::iterator prj = m_ConfigList->begin(); prj != m_ConfigList->end(); prj++)
    {
        prjidList->push_back((*prj)->projectid);
    }
    return prjidList;
}

bool DBManager::validProject(int projectID)
{
    bool bFound = false;
    for (std::list<ProjectConfig*>::iterator prj = m_ConfigList->begin(); prj != m_ConfigList->end(); prj++)
    {
        if( (*prj)->projectid == projectID )
        {
            bFound = true;
            break;
        }
    }
    return bFound;
}

//std::list<ProjectConfig *> * DBManager::getAllProjectConfig()
//{
//    return m_ConfigList;
//}

MySQLDBAccess * DBManager::getBaseDB(int projectID)
{
    MySQLDBAccess * baseDB = NULL;
    for(int i=0; i<(int)m_MysqlDBList.size();i++)
    {
        if ( m_MysqlDBList[i]->getProjectID() == projectID )
        {
            baseDB = m_MysqlDBList[i];
            break;
        }
    }

    if (baseDB == NULL)
    {
        //
        // check valid project
        //
        if ( validProject(projectID) )
        {
            baseDB = new MySQLDBAccess(projectID);
            baseDB->check_valid_table();
            m_MysqlDBList.push_back(baseDB);
        }
    }

    return baseDB;
}

TimeSerialsDBAccess * DBManager::getTSDB(int projectID)
{
    TimeSerialsDBAccess * tsDB = NULL;
    for(int i=0; i<(int)m_MysqlTSDBList.size();i++)
    {
        if ( m_MysqlTSDBList[i]->getProjectID() == projectID )
        {
            tsDB = m_MysqlTSDBList[i];
            break;
        }
    }

    if (tsDB == NULL)
    {
        //
        // check valid project
        //
        if ( validProject(projectID) )
        {
            tsDB = new TimeSerialsDBAccess(projectID);
            tsDB->check_valid_table();
            m_MysqlTSDBList.push_back(tsDB);
        }
    }

    return tsDB;
}

TrackDBAccess * DBManager::getTrackDB(int projectID)
{
    TrackDBAccess * trackDB = NULL;
    for(int i=0; i<(int)m_TrackDBList.size();i++)
    {
        if ( m_TrackDBList[i]->getProjectID() == projectID )
        {
            trackDB = m_TrackDBList[i];
            break;
        }
    }

    if (trackDB == NULL)
    {
        //
        // check valid project
        //
        if ( validProject(projectID) )
        {
            trackDB = new TrackDBAccess(projectID);
            trackDB->check_valid_table();
            m_TrackDBList.push_back(trackDB);
        }
    }

    return trackDB;
}

FootprintDBAccess * DBManager::getFootprintDB(int projectID)
{
    FootprintDBAccess * fpDB = NULL;
    for(int i=0; i<(int)m_FootprintDBList.size();i++)
    {
        if ( m_FootprintDBList[i]->getProjectID() == projectID )
        {
            fpDB = m_FootprintDBList[i];
            break;
        }
    }

    if (fpDB == NULL)
    {
        //
        // check valid project
        //
        if ( validProject(projectID) )
        {
            fpDB = new FootprintDBAccess(projectID);
            fpDB->check_valid_table();
            m_FootprintDBList.push_back(fpDB);
        }
    }

    return fpDB;
}

VILSSystemConfig * DBManager::get_sys_config(int projectID)
{
    //vilslog::printf ("DBManager::get_sys_config() projectID[%d] m_PrjSysConfigList.size()[%d]\n", projectID, (int)m_PrjSysConfigList.size());

    for (std::list<ProjectSystemConfigConfig*>::iterator prj = m_PrjSysConfigList.begin(); prj != m_PrjSysConfigList.end(); prj++)
    {
        if( (*prj)->projectid == projectID )
        {
            return &(*prj)->systemConfig;
        }
    }

    vilslog::printf ("DBManager::get_sys_config() create new one\n");

    ProjectConfig * config = m_ConfigDB->read_config(projectID);
    m_ConfigList->push_back(config);

    // create new one
    ProjectSystemConfigConfig * newPrj = new ProjectSystemConfigConfig();
    newPrj->projectid = projectID;
    vilslog::printf ("DBManager::get_sys_config() create new one 1\n");

    MySQLDBAccess * baseDb = getBaseDB(projectID);

    vilslog::printf ("DBManager::get_sys_config() create new one 2\n");

    baseDb->read_sys_config(&newPrj->systemConfig);

    vilslog::printf ("DBManager::get_sys_config() create new one 3\n");

    m_PrjSysConfigList.push_back(newPrj);

    // create new db
    getTSDB(projectID);
    vilslog::printf ("DBManager::get_sys_config() create new one TSDB\n");

    getTrackDB(projectID);

    vilslog::printf ("DBManager::get_sys_config() create new one 4\n");

    getFootprintDB(projectID);

    vilslog::printf ("DBManager::get_sys_config() create new one 5\n");

    return &newPrj->systemConfig;
}

int DBManager::getDatabaseSize(int projectID)
{
    int size = 0;

    MySQLDBAccess * baseDb = getBaseDB(projectID);
    size = baseDb->getDatabaseSize();

    return size;
}

bool DBManager::update_config(VILSConfig config)
{
    return m_ConfigDB->update_config(config);
}

bool DBManager::delete_config(int projectID)
{
    m_ConfigDB->delete_config(projectID);

//    for (auto it = m_MysqlDBList.begin(); it != m_MysqlDBList.end(); ++it)
//    {
//        if ( (*it)->getProjectID() == projectID )
//        {
//            (*it)->deletedb();
//            m_MysqlDBList.erase(it);
//            delete (*it);
//            break;
//        }
//    }
//
//    for (auto it = m_MysqlTSDBList.begin(); it != m_MysqlTSDBList.end(); ++it)
//    {
//        if ( (*it)->getProjectID() == projectID )
//        {
//            (*it)->deletedb();
//            m_MysqlTSDBList.erase(it);
//            delete (*it);
//            break;
//        }
//    }
//
//    for (auto it = m_TrackDBList.begin(); it != m_TrackDBList.end(); ++it)
//    {
//        if ( (*it)->getProjectID() == projectID )
//        {
//            (*it)->deletedb();
//            m_TrackDBList.erase(it);
//            delete (*it);
//            break;
//        }
//    }

    return true;
}
