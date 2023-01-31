<?php

    if (!isset($_SESSION))
    {
      session_start();
    }

    global $GL_CONFIG;
    $GL_CONFIG['IMAGE_UPLOAD_PATH'] = "/home/norgay/www/upload/"; //Mac OS
    $GL_CONFIG['STORGAE_FILE_UPLOAD_PATH'] = "/home/norgay/www/upload/files/"; //Mac OS
    $GL_CONFIG['STORGAE_IMAGE_UPLOAD_PATH'] = "/home/norgay/www/upload/images/"; //Mac OS
    $GL_CONFIG['WEB_LOG_PATH'] = "/home/norgay/www/log/";

    global $APP_BIN_FILE;
    $APP_BIN_FILE = "/home/norgay/vilscenter/vilscenter_norgay";

    // Language dependent
    include ('php/SMIMS_config.php');
    $PAGE_CONFIG = $_SESSION['PAGE_CONFIG'];

    if ($PAGE_CONFIG == 'pthg')
    {
        include ('php/PingFire_config.php');
    }
    else
    {
        include ('php/NORGAY_config.php');
    }

    //include ('php/iHome_config.php');
    // include ('php/NORGAY_config.php');
    // include ('php/PingFire_config.php');
    // include ('php/SECOM_config.php');
    // include ('php/SMIMS_config.php');
    // include ('php/AUO_config.php');

    global $_DB;
    $_DB['host'] = "127.0.0.1";
    // $_DB['username'] = "norgay";
    // $_DB['password'] = "norgay@vils2020K6GF91";
    $_DB['dbname'] = "norgay_vils_db";
    $_DB['dbname_ts'] = "norgay_vils_ts_db";
    $_DB['dbname_fp'] = "norgay_vils_fp_db";
    $_DB['dbname_track'] = "norgay_vils_track_db";
    $_DB['dbname_config'] = "norgay_vils_config_db";
    $_DB['dbname_arinspection'] = "khh_vils_arinspection_db";

    // $_DB['host'] = "127.0.0.1";
    $_DB['username'] = "soccer";
    $_DB['password'] = "soccer@soccers2022cJgyRd";
    // $_DB['dbname'] = "soccer_vils_db";
    // $_DB['dbname_ts'] = "soccer_vils_ts_db";
    // $_DB['dbname_fp'] = "soccer_vils_fp_db";
    // $_DB['dbname_track'] = "soccer_vils_track_db";
    // $_DB['dbname_config'] = "soccer_vils_config_db";

    $web_version = '3.1.5';
    $UPDATE_DATE = '20220428';

    // functions
    $GL_CONFIG_ENABLE_EMAIL_ACCOUNT = 1;
    $GL_CONFIG_SHOW_ROLE_INFO = 1;
    $GL_CONFIG_SHOW_GPS_INFO = 1;

    $GL_CONFIG_ENABLE_MIN_SIDEBAR = 0;
    $GL_CONFIG_ENABLE_DASHBOARD = 0;
    $GL_CONFIG_ENABLE_NODELIST = 0;
    $GL_CONFIG_EDIT_NODELIST = 0;
    $GL_CONFIG_ENABLE_RFIDDEVICELIST = 0;
    $GL_CONFIG_ENABLE_USEINFO = 0;
    $GL_CONFIG_EDIT_USERINFO = 0;

    $GL_CONFIG_ENABLE_MAINGROUP = 1;

    $GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP = 0;

    $GL_CONFIG_ENABLE_SUBGROUP = 1;
    $GL_CONFIG_UNLOCK_SUBGROUP = 1;

    $GL_CONFIG_ENABLE_CARDROLLCALL = 0;
    $GL_CONFIG_UNLOCK_CARDROLLCALL = 0;

    $GL_CONFIG_ENABLE_NFC_SCANRESULT = 1;
    $GL_CONFIG_UNLOCK_NFC_SCANRESULT = 1;

    $GL_CONFIG_ENABLE_UHF_SCANRESULT = 1;
    $GL_CONFIG_UNLOCK_UHF_SCANRESULT = 1;

    $GL_CONFIG_ENABLE_BODYINFO = 0;
    $GL_CONFIG_ENABLE_FORACARE = 0;

    $GL_CONFIG_ENABLE_PUSHNOTIFICATION = 0;
    $GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION = 0;

    $GL_CONFIG_ENABLE_SURVEY = 1;
    $GL_CONFIG_ENABLE_ASSIST = 1;
    $GL_CONFIG_ENABLE_BILLBOARD = 1;

    $GL_CONFIG_ENABLE_LOCATION = 0;
    $GL_CONFIG_ENABLE_RANGING = 0;
    $GL_CONFIG_ENABLE_2DMAP = 0;
    $GL_CONFIG_EDIT_2DMAP = 0;
    $GL_CONFIG_2DMAP_SHOW_GMAP = 0;
    $GL_CONFIG_ENABLE_FIELDCONTROLL = 0;
    $GL_CONFIG_ENABLE_FIREFIGHTING = 0;
    $GL_CONFIG_ENABLE_3DMAP = 0;

    $GL_CONFIG_ENABLE_ACTIVITY = 1;

    $GL_CONFIG_ENABLE_MAINTENANCE = 0;
    $GL_CONFIG_EDIT_MAINTENANCE = 0;

    $GL_CONFIG_ENABLE_VERSION_HISTORY = 1;
    $GL_CONFIG_ENABLE_CURRICULUM = 1;

    $GL_CONFIG_ENABLE_COURSE = 0;

    $GL_CONFIG_ENABLE_WEEK_SCHEDULE = 1;
    $GL_CONFIG_ENABLE_APPLICATION_FORM = 1;

    $GL_CONFIG_ENABLE_FOOTPRINT = 0;
    $GL_CONFIG_ENABLE_EVENT = 0;
    $GL_CONFIG_ENABLE_INSPECTION = 0;
    $GL_CONFIG_ENABLE_SECTION = 0;
    $GL_CONFIG_ENABLE_FENCE = 0;

    $GL_CONFIG_ENABLE_FILESTORAGE = 1;
    $GL_CONFIG_ENABLE_BILLBOARD = 1;

    $GL_CONFIG_ENABLE_SETTING = 0;
    $GL_CONFIG_ENABLE_DI_SETTING = 0;
    $GL_CONFIG_ENABLE_ENGINEERMODE = 0;

    $GL_CONFIG_ENABLE_POWERMETER = 1;
    $GL_CONFIG_ENABLE_MQTTApplication = 1;

    if ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2)
    {
        // 1, "工程人員" 2, "Power User"

        $GL_CONFIG_ENABLE_DASHBOARD = 1;

        $GL_CONFIG_ENABLE_NODELIST = 1;
        $GL_CONFIG_EDIT_NODELIST = 1;

        $GL_CONFIG_ENABLE_USEINFO = 1;
        $GL_CONFIG_EDIT_USERINFO = 1;
        $GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP = 1;

        $GL_CONFIG_ENABLE_CARDROLLCALL = 1;
        $GL_CONFIG_UNLOCK_CARDROLLCALL = 1;

        $GL_CONFIG_ENABLE_BODYINFO = 1;
        $GL_CONFIG_ENABLE_FORACARE = 1;

        $GL_CONFIG_ENABLE_PUSHNOTIFICATION = 1;
        $GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION = 1;

        $GL_CONFIG_ENABLE_LOCATION = 1;

        $GL_CONFIG_ENABLE_RANGING = 1;

        $GL_CONFIG_ENABLE_2DMAP = 1;
        $GL_CONFIG_EDIT_2DMAP = 1;

        $GL_CONFIG_ENABLE_3DMAP = 1;

        $GL_CONFIG_ENABLE_MAINTENANCE = 1;
        $GL_CONFIG_EDIT_MAINTENANCE = 1;

        $GL_CONFIG_ENABLE_COURSE = 1;

        $GL_CONFIG_ENABLE_FOOTPRINT = 1;

        $GL_CONFIG_ENABLE_EVENT = 1;

        $GL_CONFIG_ENABLE_INSPECTION = 1;

        $GL_CONFIG_ENABLE_SECTION = 1;

        $GL_CONFIG_ENABLE_FENCE = 1;

        if ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2)
        {
            $GL_CONFIG_ENABLE_SETTING = 1;
            $GL_CONFIG_ENABLE_DI_SETTING = 1;
            $GL_CONFIG_ENABLE_ENGINEERMODE = 1;
        }
    }
    else
    if ($_SESSION["groupid"] == 4)
    {
        // "工作人員", "能看部份網頁
        $GL_CONFIG_ENABLE_NODELIST = 1;

        $GL_CONFIG_ENABLE_USEINFO = 1;
        $GL_CONFIG_EDIT_USERINFO = 1;
        $GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP = 1;

        $GL_CONFIG_ENABLE_CARDROLLCALL = 1;
        $GL_CONFIG_UNLOCK_CARDROLLCALL = 1;

        $GL_CONFIG_ENABLE_BODYINFO = 1;

        $GL_CONFIG_ENABLE_PUSHNOTIFICATION = 1;
        $GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION = 1;

        $GL_CONFIG_ENABLE_LOCATION = 1;

        $GL_CONFIG_ENABLE_RANGING = 1;

        $GL_CONFIG_ENABLE_2DMAP = 1;

        $GL_CONFIG_ENABLE_3DMAP = 1;

        $GL_CONFIG_ENABLE_ACTIVITY = 1;

        $GL_CONFIG_ENABLE_MAINTENANCE = 1;

        $GL_CONFIG_ENABLE_COURSE = 1;

        $GL_CONFIG_ENABLE_FOOTPRINT = 1;

        $GL_CONFIG_ENABLE_WEEK_SCHEDULE = 1;

        $GL_CONFIG_ENABLE_APPLICATION_FORM = 1;

        $GL_CONFIG_ENABLE_FILESTORAGE = 1;
    }
    else
    if ($_SESSION["groupid"] == 5)
    {
        // "子集合管理員", 可以操作推播, 待辦事項, 請假單, 團體活動表, 班級功課表, 檔案庫
        // "主集合管理員"


        $GL_CONFIG_ENABLE_NODELIST = 1;
        $GL_CONFIG_EDIT_NODELIST = 1;

        $GL_CONFIG_ENABLE_USEINFO = 1;
        $GL_CONFIG_EDIT_USERINFO = 1;
        $GL_CONFIG_ENABLE_CREATE_NEW_SUBGROUP = 1;
        $GL_CONFIG_ENABLE_SUBGROUP = 1;
        $GL_CONFIG_UNLOCK_SUBGROUP = 1;

        $GL_CONFIG_ENABLE_CARDROLLCALL = 1;
        $GL_CONFIG_UNLOCK_CARDROLLCALL = 0;

        $GL_CONFIG_ENABLE_NFC_SCANRESULT = 1;
        $GL_CONFIG_UNLOCK_NFC_SCANRESULT = 0;

        $GL_CONFIG_ENABLE_UHF_SCANRESULT = 1;
        $GL_CONFIG_UNLOCK_UHF_SCANRESULT = 0;

        $GL_CONFIG_ENABLE_BODYINFO = 1;

        $GL_CONFIG_ENABLE_PUSHNOTIFICATION = 1;
        $GL_CONFIG_ENABLE_SEND_ALL_PUSHNOTIFICATION = 0;

        $GL_CONFIG_ENABLE_ASSIST = 0;

        $GL_CONFIG_ENABLE_BILLBOARD = 1;

        $GL_CONFIG_ENABLE_LOCATION = 1;

        $GL_CONFIG_ENABLE_RANGING = 1;

        $GL_CONFIG_ENABLE_2DMAP = 1;
        $GL_CONFIG_EDIT_2DMAP = 1;
        $GL_CONFIG_2DMAP_SHOW_GMAP = 1;

        $GL_CONFIG_ENABLE_FIELDCONTROLL = 1;
        $GL_CONFIG_ENABLE_FIREFIGHTING = 1;

        // $GL_CONFIG_ENABLE_3DMAP = 1;

        $GL_CONFIG_ENABLE_ACTIVITY = 1;

        // $GL_CONFIG_ENABLE_MAINTENANCE = 1;
        $GL_CONFIG_ENABLE_CURRICULUM = 0;

        $GL_CONFIG_ENABLE_COURSE = 1;

        $GL_CONFIG_ENABLE_FOOTPRINT = 1;

        $GL_CONFIG_ENABLE_INSPECTION = 1;

        $GL_CONFIG_ENABLE_SECTION = 1;

        $GL_CONFIG_ENABLE_WEEK_SCHEDULE = 1;

        $GL_CONFIG_ENABLE_APPLICATION_FORM = 1;

        $GL_CONFIG_ENABLE_FILESTORAGE = 1;
    }
    else
    if ($_SESSION["groupid"] == 6)
    {
        // 版本紀錄
        $GL_CONFIG_ENABLE_VERSION_HISTORY = 0;
        // 物聯網
        $GL_CONFIG_ENABLE_ACTIVITY = 0;
        // 功課表
        $GL_CONFIG_ENABLE_WEEK_SCHEDULE = 0;
        // 工作指示
        $GL_CONFIG_ENABLE_SURVEY = 0;
        $GL_CONFIG_ENABLE_ASSIST = 0;
        // 表單
        $GL_CONFIG_ENABLE_APPLICATION_FORM = 0;
        // 學校課程
        $GL_CONFIG_ENABLE_CURRICULUM = 0;
        // 檔案庫
        $GL_CONFIG_ENABLE_FILESTORAGE = 0;
        // 公布欄
        $GL_CONFIG_ENABLE_BILLBOARD = 0;
        // 智慧電表監控
        $GL_CONFIG_ENABLE_POWERMETER = 0;

    }
?>
