<?php

session_start();

require_once("common.php");
require_once("User.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        REST_do_GET();
    break;
    case 'PUT':
        REST_do_PUT();
    break;
    case 'POST':
        REST_do_POST();
    break;
    case 'DELETE':
        REST_do_DELETE();
    break;
}

function REST_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadprojectconfig']))
    {
        $return_str = loadprojectconfig($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadconfig']))
    {
        $return_str = loadconfig($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadallconfig']))
    {
        $return_str = loadallconfig();
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadsystemconfig']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadsystemconfig='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadsystemstatus']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('systemstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['waitatcmd']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['waitkey'] = $_GET['waitkey'];
        $json_string = json_encode($json_array);
        $status = getServerInfo('waitatcmd='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['cmd']))
    {
        // localhost/php/configvils.php?cmd=printAreaRollCallServiceTagAreaBuffer
        sendMessageToServer('cmd='.$_GET['cmd']);
        echo $_GET['cmd'];
    }
    else
    if (isset($_GET['loaduiblock']))
    {
        $accountid = $_SESSION["accountid"];

        $return_str = loaduiblock($project_id, $accountid);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadsubgroupuiblock']))
    {
        $return_str = loadsubgroupuiblock($project_id);
        echo json_encode($return_str);
    }
}

function REST_do_PUT()
{
}

function REST_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];
    if (isset($_POST['PROJECT_NAME']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['PROJECT_NAME'] = $_POST['PROJECT_NAME'];
        $json_array['PROJECT_DESCRIPTION'] = $_POST['PROJECT_DESCRIPTION'];
        $json_string = json_encode($json_array);
        sendMessageToServer('updateconfig='.$json_string);
        echo json_encode($json_string);
    }
    else
    if (isset($_POST['create_new_project']))
    {
        // $project_id = -1;
        // $return_str = loadconfig($project_id);
        $json_string = create_new_project();
        //echo $json_string;
        $status = json_encode($json_string);
        echo $status;
    }
    else
    if (isset($_POST['restart_app']))
    {
        sendMessageToServer('restart_app');
    }
    else
    if (isset($_POST['setcenterudpport']))
    {
        $systemconfig = [];
        $systemconfig['project_id'] = $project_id;
        $systemconfig['web_udp_port'] = $_POST['setcenterwebport'];
        $systemconfig['center_udp_port'] = $_POST['setcenterudpport'];
        $systemconfig['coord_udp_port'] = $_POST['setcoordudpport'];
        $systemconfig['center_broadcast_port'] = $_POST['setcenterbroadcastport'];
        $systemconfig['web_socket_port'] = $_POST['setwebsocketport'];
        $systemconfig['TagAlertTimeout'] = $_POST['settagalerttimeout'];
        $systemconfig['TagAlertVoltageLow'] = $_POST['settagvoltagelow'];
        $systemconfig['VILSServer'] = $_POST['setvilsserver'];
        $systemconfig['ERPServer'] = $_POST['seterpserver'];
        $systemconfig['SSOIP'] = $_POST['setssoip'];
        //$systemconfig['PushNotifyPrefix'] = $_POST['setpushnotifyprefix'];
        //$systemconfig['EnablePushNotify'] = $_POST['setenablepushnotify'];
        $json_string = json_encode($systemconfig);
        sendMessageToServer('updatesystemconfigudpport='.$json_string);

        echo $json_string;
    }
    else
    if (isset($_POST['setpushnotifyprefix']))
    {
        $systemconfig = [];
        $systemconfig['PushNotifyPrefix'] = $_POST['setpushnotifyprefix'];
        $systemconfig['EnablePushNotify'] = $_POST['setenablepushnotify'];
        $json_string = json_encode($systemconfig);
        sendMessageToServer('updatesystemconfigudpport='.$json_string);

        echo $json_string;
    }
    else
    if (isset($_POST['updateAlgorithmConfig']))
    {
        $json_string = json_encode($_POST);
        sendMessageToServer('updateAlgorithmConfig='.$json_string);
        echo $json_string;
    }
    else
    if (isset($_POST['updateLoRAWANConfig']))
    {
        $json_string = json_encode($_POST);
        sendMessageToServer('updateLoRAWANConfig='.$json_string);
        echo $json_string;
    }
    else
    if (isset($_POST['loadqrcode']))
    {
        $type = "";
        $id = "";
        $accountid = "";
        if (isset($_POST['type']))
            $type = $_POST['type'];
        if (isset($_POST['id']))
            $id = $_POST['id'];
        if (isset($_POST['accountid']))
            $accountid = $_POST['accountid'];

        $valid = false;
        if (strcmp($type,"USER") == 0)
        {
            if ( strlen($id) > 0 )
                $valid = true;

            // if ( strcmp($id, $_SESSION['accountid']) == 0 )
            // {
            //     $valid = true;
            //     if ( strlen($id) > 0 )
            //         $valid = true;
            // }
        }
        else
        if (strcmp($type,"TAG") == 0 || strcmp($type,"LOCATOR") == 0 || strcmp($type,"CARD") == 0)
        {
            if ( strlen($id) > 0 )
                $valid = true;
        }
        else
        if (strcmp($type,"MAINGROUP") == 0)
        {
            if ( strlen($id) > 0 )
                $valid = true;
        }
        else
        if (strcmp($type,"SUBGROUP") == 0)
        {
            if ( strlen($id) > 0 )
                $valid = true;
        }

        if ($valid)
        {

            $json_array = [];
            $json_array['project_id'] = $project_id;
            $json_array['type'] = $type;
            $json_array['id'] = $id;
            $json_array['accountid'] = $accountid;

            $maingroups = explode(",", $_SESSION['maingroups']);
            if (count($maingroups) > 0)
                $json_array['maingroupid'] = $maingroups[0];
            else
                $json_array['maingroupid'] = '1';

            $json_string = json_encode($json_array);
            $status_str = getServerInfo('loadqrcode='.$json_string);
            echo $status_str;
        }
        else
        {
            $json_array = [];
            $json_array['success'] = 'false';
            $json_array['qrcode'] = '';
            echo json_encode($json_array);
        }
    }
    else
    if (isset($_POST['loadpdfreport']))
    {
        $type = "";
        $id = "";
        if (isset($_POST['type']))
            $type = $_POST['type'];
        if (isset($_POST['id']))
            $id = $_POST['id'];

        $valid = false;
        if (strcmp($type,"USER") == 0)
        {
            if ( strcmp($id, $_SESSION['accountid']) == 0 )
            {
                $valid = true;
                if ( strlen($id) > 0 )
                    $valid = true;
            }
        }

        if ($valid)
        {
            $json_array = [];
            $json_array['project_id'] = $project_id;
            $json_array['type'] = $type;
            $json_array['id'] = $id;
            $json_string = json_encode($json_array);
            $status_str = getServerInfo('loadpdfreport='.$json_string);
            echo $status_str;
        }
        else
        {
            $json_array = [];
            $json_array['success'] = 'false';
            $json_array['pdfreport'] = '';
            echo json_encode($json_array);
        }
    }
    else
    if (isset($_POST['loadareainoutpdfreport']))
    {
        $date = "";
        $startdate = "";
        $enddate = "";
        $id = "";
        if (isset($_POST['date']))
            $date = $_POST['date'];
        if (isset($_POST['startdate']))
            $startdate = $_POST['startdate'];
        if (isset($_POST['enddate']))
            $enddate = $_POST['enddate'];
        if (isset($_POST['id']))
            $id = $_POST['id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['date'] = $date;
        $json_array['startdate'] = $startdate;
        $json_array['enddate'] = $enddate;
        $json_array['id'] = $id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadareainoutpdfreport='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['loadareainoutxlsxreport']))
    {
        $date = "";
        $startdate = "";
        $enddate = "";
        $id = "";
        if (isset($_POST['date']))
            $date = $_POST['date'];
        if (isset($_POST['startdate']))
            $startdate = $_POST['startdate'];
        if (isset($_POST['enddate']))
            $enddate = $_POST['enddate'];
        if (isset($_POST['id']))
            $id = $_POST['id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['date'] = $date;
        $json_array['startdate'] = $startdate;
        $json_array['enddate'] = $enddate;
        $json_array['id'] = $id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadareainoutxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['loadeventpdfreport']))
    {
        $date = "";
        $id = "";
        if (isset($_POST['date']))
            $date = $_POST['date'];
        if (isset($_POST['id']))
            $id = $_POST['id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['date'] = $date;
        $json_array['id'] = $id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadeventpdfreport='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['loadtagdailypdfreport']))
    {
        $date = "";
        $id = "";
        if (isset($_POST['date']))
            $date = $_POST['date'];
        if (isset($_POST['id']))
            $id = $_POST['id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['date'] = $date;
        $json_array['id'] = $id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadtagdailypdfreport='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['sendatcmd']))
    {
        $json_string = json_encode($_POST);
        $status_str = getServerInfo('sendatcmd='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['resetLoRA']))
    {
        sendMessageToServer('resetLoRA');
        echo $_POST;
    }
    else
    if (isset($_POST['delete_project']))
    {
        if (isset($_POST['project_id']))
        {
            $project_id = (int)$_POST['project_id'];
            if ( $project_id == 1 )
            {
                // can not delete this project
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
            else
            if ( delete_config($project_id))
            {
                $project_id = 1;
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
            else
            {
                $project_id = 1;
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
        }
    }
    else
    if (isset($_POST['updateuiblock']))
    {
        if (isset($_POST['project_id']))
        {
            $accountid = $_SESSION["accountid"];
            $project_id = (int)$_POST['project_id'];
            $uidata = $_POST['uidata'];

            $return_str = updateuiblock($project_id, $accountid, $uidata);
            echo json_encode($return_str);
        }
    }
    else
    if (isset($_POST['resetuiblock']))
    {
        if (isset($_POST['project_id']))
        {
            $accountid = $_SESSION["accountid"];
            $project_id = (int)$_POST['project_id'];

            $return_str = resetuiblock($project_id, $accountid);
            echo json_encode($return_str);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if(isset($_POST['updatesubgroupuiblock']))
    {
        if (isset($_POST['project_id']))
        {
            $project_id = (int)$_POST['project_id'];
            $subgroupids = $_POST['subgroupids'];
            $uidata = $_POST['uidata'];

            $return_str = updatesubgroupuiblock($project_id, $subgroupids, $uidata);
            echo json_encode($return_str);
        }
    }
    // else
    // if (isset($_POST['UPDATE_PUSHNOTIFY']))
    // {
    //     $functions = "";
    //     $account = "";
    //     if (isset($_POST['FUNCTIONS']))
    //         $functions = $_POST['FUNCTIONS'];
    //     if (isset($_POST['ACCOUNT']))
    //         $account = $_POST['ACCOUNT'];
    //
    //     update_PushNotify($project_id, $account, $functions);
    //
    //     $json_array = [];
    //     $json_array['project_id'] = $project_id;
    //     $json_array['ACCOUNT'] = $account;
    //     $json_string = json_encode($json_array);
    //     sendMessageToServer('updatePushNotify='.$json_string);
    // }
    // else
    // if (isset($_POST['sendnotification']))
    // {
    //     $json_string = json_encode($_POST);
    //     $status_str = getServerInfo('sendnotification='.$json_string);
    //     echo $status_str;
    // }
    // else
    // if (isset($_POST['sendmultinotification']))
    // {
    //     $json_string = json_encode($_POST);
    //     $status_str = getServerInfo('sendmultinotification='.$json_string);
    //     echo $status_str;
    // }
}

function REST_do_DELETE()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);

    if (isset($_GET['delete_project']))
    {
        if (isset($_GET['project_id']))
        {
            $project_id = (int)$_GET['project_id'];
            if ( $project_id == 1 )
            {
                // can not delete this project
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
            else
            if ( delete_config($project_id))
            {
                $project_id = 1;
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
            else
            {
                $project_id = 1;
                $return_str = loadconfig($project_id);
                echo json_encode($return_str);
            }
        }
    }
    else
    {
        $return_str = loadconfig($project_id);
        echo json_encode($return_str);
    }
}

function delete_config($project_id)
{
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    sendMessageToServer('deleteconfig='.$json_string);
    return true;
}

function loadprojectconfig($project_id)
{
    $config = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from config WHERE PROJECT_ID=" . $project_id);
    while($result = $db->fetch_array())
    {
        $config = [];
        $config['PROJECT_ID'] = $result['PROJECT_ID'];
        $config['PROJECT_NAME'] = $result['PROJECT_NAME'];
        $config['PROJECT_DESCRIPTION'] = $result['PROJECT_DESCRIPTION'];
        $config['WGS48OriginX'] = $result['WGS48OriginX'];
        $config['WGS48OriginY'] = $result['WGS48OriginY'];
        $config['MAP_North'] = $result['MAP_North'];
        $config['BuildingID'] = $result['BuildingID'];
        $config['BuildingFloor'] = $result['BuildingFloor'];
        // $config['uploadresult'] = $result['uploadresult'];
    }
    //$db->close();
    return $config;
}

function create_new_project()
{
    // create new project
    $config = [];
    $config['PROJECT_ID'] = getEmptyProjectID();
    $config['PROJECT_NAME'] = 'SMIMS VILS';
    $config['PROJECT_DESCRIPTION'] = 'SMIMS VILS PROJECT DESCRIPTION';

    write_config($config['PROJECT_ID'], $config);

    $json_array = [];
    $json_array['project_id'] = $config['PROJECT_ID'];
    $json_string = json_encode($json_array);
    //$status_str = getServerInfo('createconfig='.$json_string);
    sendMessageToServer('createconfig='.$json_string);
    return $config;
}

function loadconfig($project_id)
{
    $config = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    //$db = new DB($project_id, 'base');
    $db = new DB(1, 'config');

    $db->query("SELECT * from config WHERE PROJECT_ID=" . $project_id);
    while($result = $db->fetch_array())
    {
        $config = [];
        $config['PROJECT_ID'] = $result['PROJECT_ID'];
        $config['PROJECT_NAME'] = $result['PROJECT_NAME'];
        $config['PROJECT_DESCRIPTION'] = $result['PROJECT_DESCRIPTION'];
    }
    //$db->close();

    if ( count($config) == 0 )
    {
        // create default config
        $config = [];
        $config['PROJECT_ID'] = $project_id;
        $config['PROJECT_NAME'] = 'SMIMS VILS';
        $config['PROJECT_DESCRIPTION'] = 'SMIMS VILS';

        write_config($project_id, $config);
    }

    return $config;
}

function write_config($project_id, $config)
{
    $db = new DB(1, 'config');

    $db->query("INSERT INTO config(PROJECT_ID, PROJECT_NAME, PROJECT_DESCRIPTION) " .
                "VALUES (".$config['PROJECT_ID'].", '".$config['PROJECT_NAME']."', '".$config['PROJECT_DESCRIPTION']."' )");

    return $config;
}

function loadallconfig()
{
    $allconfigs = [];

    $db = new DB(1, 'config');

    $db->query("SELECT * from config");
    while($result = $db->fetch_array())
    {
        $config = [];
        $config['PROJECT_ID'] = $result['PROJECT_ID'];
        $config['PROJECT_NAME'] = $result['PROJECT_NAME'];
        $config['PROJECT_DESCRIPTION'] = $result['PROJECT_DESCRIPTION'];

        $allconfigs[$config['PROJECT_ID']] = $config;
    }


    if ( count($allconfigs) == 0 )
    {
        // create default config
        $config = [];
        $config['PROJECT_ID'] = 1;
        $config['PROJECT_NAME'] = 'SMIMS VILS';
        $config['PROJECT_DESCRIPTION'] = 'SMIMS VILS';
        $allconfigs[$config['PROJECT_ID']] = $config;

        write_config($project_id, $config);
    }

    return $allconfigs;
}

function getEmptyProjectID()
{
    $db = new DB(1, 'config');

    $MAXprojectid = 1;
    $db->query("SELECT MAX(PROJECT_ID) AS PROJECT_ID FROM config");
    if($result = $db->fetch_array())
    {
        $MAXprojectid = (int)$result['PROJECT_ID'] + 1;
    }

    //printf("MySQLDBAccess::getEmptyProjectID() MAXprojectid[%d]\n", MAXprojectid);
    return $MAXprojectid;
}

function loaduiblock($project_id, $accountid)
{
    $uiblock = [];

    $db = new DB($project_id, 'base');

    if ( $db->query("SELECT * from uiblock WHERE accountid='$accountid' ") == TRUE )
    {
        if($result = $db->fetch_array())
        {
            $uiblock = [];
            $uiblock['accountid'] = $result['accountid'];
            $uiblock['uidata'] = json_decode($result['jsonstring']);
        }
    }

    if ( count($uiblock['uidata']) == 0 )
    {
        // create default config
        $config = getDefaultUIBlock($project_id, $accountid);

        $uiblock = [];
        $uiblock['accountid'] = $accountid;
        $uiblock['uidata'] = $config;
        write_uiblock($project_id, $uiblock);
    }

    return $uiblock;
}

function write_uiblock($project_id, $uiblock)
{
    $db = new DB($project_id, 'base');

    $accountid = $uiblock['accountid'];
    $jsonstring = json_encode($uiblock['uidata'], JSON_UNESCAPED_UNICODE);

    $db->query("INSERT INTO uiblock(accountid, jsonstring) VALUES ('$accountid', '$jsonstring' ) ON DUPLICATE KEY UPDATE jsonstring=VALUES(jsonstring) ");

    return $uiblock;
}

function updateuiblock($project_id, $accountid, $uidata)
{
    $db = new DB($project_id, 'base');

    $jsonstring = $uidata;

    $db->query("INSERT INTO uiblock(accountid, jsonstring) VALUES ('$accountid', '$jsonstring' ) ON DUPLICATE KEY UPDATE jsonstring=VALUES(jsonstring) ");

    return $uidata;
}

function resetuiblock($project_id, $accountid)
{
    $config = getDefaultUIBlock($project_id, $accountid);

    $uiblock = [];
    $uiblock['accountid'] = $accountid;
    $uiblock['uidata'] = $config;

    write_uiblock($project_id, $uiblock);

    return $uiblock;
}

function getDefaultUIBlock($project_id, $accountid)
{
    $user = loadsingleuser($project_id, $accountid);

    $config = [];

    if (strlen($user['subgroups']) > 0)
    {
        $config = getUserSubgroupUIBlock($project_id, $user['subgroups']);

        // echo 'getDefaultUIBlock() $config:'. $config;
    }

    if (count($config) == 0)
    {
        $groupid = $user['groupid'];

        $config = fill_uiblock_info($config, 0, 0, "id_course", "courses.php", "活動表", "團體活動資料", "bg-maroon-gradient", "bg-maroon");
        $config = fill_uiblock_info($config, 0, 1, "id_courserecord", "courserecord.php", "出席記錄", "課程表上課紀錄", "bg-green-gradient", "bg-green");
        $config = fill_uiblock_info($config, 0, 2, "id_bodyinfo", "bodyinforecord.php", "生理量測", "查看量測紀錄", "bg-yellow-gradient", "bg-yellow");
        $config = fill_uiblock_info($config, 0, 3, "id_inspection", "inspection.php", "巡檢記錄", "巡檢歷史紀錄", "bg-teal-gradient", "bg-teal");
        $config = fill_uiblock_info($config, 1, 0, "id_pushnotificationrecords", "pushnotificationrecords.php", "推播紀錄", "顯示推播紀錄", "bg-aqua-gradient", "bg-aqua");
        $config = fill_uiblock_info($config, 1, 1, "id_cards", "mycards.php", "關連卡片", "顯示我的卡片", "bg-purple-gradient", "bg-purple");
        $config = fill_uiblock_info($config, 1, 2, "id_userrollcall", "userrollcallrecord.php", "人員點名", "顯示人員點名紀錄", "bg-light-blue-gradient", "bg-light-blue");
        $config = fill_uiblock_info($config, 1, 3, "id_qrcode", "javascript:showqrcode();", "QR Code", "我的 QR Code", "bg-black-gradient", "bg-black");
        $config = fill_uiblock_info($config, 2, 0, "id_weekschedule", "weekschedule.php", "功課表", "班級功課表", "bg-purple-gradient", "bg-purple");
        $config = fill_uiblock_info($config, 2, 1, "id_storagefile", "storagefile.php", "檔案庫", "班級檔案庫", "bg-light-blue-gradient", "bg-light-blue");
        $config = fill_uiblock_info($config, 2, 2, "id_storageimage", "storageimage.php", "相簿", "班級相簿", "bg-teal-gradient", "bg-teal");
        $config = fill_uiblock_info($config, 2, 3, "id_applicationformleave", "applicationformleave.php", "請假單", "我的請假單", "bg-purple-gradient", "bg-purple");
        $config = fill_uiblock_info($config, 3, 0, "id_assist", "assistrecords.php", "協助事項", "我的協助事項", "bg-green-gradient", "bg-green");
        $config = fill_uiblock_info($config, 3, 1, "id_billboardrecords", "billboardrecords.php", "公佈欄", "查看公佈事項", "bg-teal-gradient", "bg-teal");
        $config = fill_uiblock_info($config, 3, 2, "id_curriculum", "curriculumapply.php", "選課課程", "提供學生選修課程", "bg-light-blue-gradient", "bg-light-blue");
        $config = fill_uiblock_info($config, 3, 3, "id_curriculumschedule", "curriculumschedule.php", "課程表", "選修課程表", "bg-light-blue-gradient", "bg-light-black");

        if ($groupid != 3)
        {
            $config = fill_uiblock_info($config, 4, 0, "id_subgroup", "usersubgroupinfo.php", "子集合", "子集合管理", "bg-maroon-gradient", "bg-maroon");
            $config = fill_uiblock_info($config, 4, 1, "id_userinfo", "userinfo.php", "人員列表", "人員管理", "bg-yellow-gradient", "bg-yellow");
            $config = fill_uiblock_info($config, 4, 2, "id_pushnotification", "pushnotification.php", "推播訊息", "推播App訊息", "bg-aqua-gradient", "bg-aqua");
        }
    }

	// .bg-teal-gradient
	// .bg-light-blue-gradient
	// .bg-blue-gradient
	// .bg-aqua-gradient
	// .bg-yellow-gradient
	// .bg-purple-gradient
	// .bg-green-gradient
	// .bg-red-gradient
	// .bg-black-gradient
	// .bg-maroon-gradient

    return $config;
}

function getDefaultUserSubGroupUIBlock()
{
    $config = [];
    $config = fill_uiblock_info($config, 0, 0, "id_course", "courses.php", "活動表", "團體活動資料", "bg-maroon-gradient", "bg-maroon");
    $config = fill_uiblock_info($config, 0, 1, "id_pushnotificationrecords", "pushnotificationrecords.php", "推播紀錄", "顯示推播紀錄", "bg-aqua-gradient", "bg-aqua");
    $config = fill_uiblock_info($config, 0, 2, "id_cards", "mycards.php", "關連卡片", "顯示我的卡片", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 0, 3, "id_qrcode", "javascript:showqrcode();", "QR Code", "我的 QR Code", "bg-black-gradient", "bg-black");
    $config = fill_uiblock_info($config, 1, 0, "id_weekschedule", "weekschedule.php", "功課表", "班級功課表", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 1, 1, "id_storagefile", "storagefile.php", "檔案庫", "班級檔案庫", "bg-light-blue-gradient", "bg-light-blue");
    $config = fill_uiblock_info($config, 1, 2, "id_storageimage", "storageimage.php", "相簿", "班級相簿", "bg-teal-gradient", "bg-teal");
    $config = fill_uiblock_info($config, 1, 3, "id_applicationformleave", "applicationformleave.php", "請假單", "我的請假單", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 2, 0, "id_curriculum", "curriculumapply.php", "選課課程", "提供學生選修課程", "bg-light-blue-gradient", "bg-light-blue");
    $config = fill_uiblock_info($config, 2, 1, "id_curriculumschedule", "curriculumschedule.php", "課程表", "選修課程表", "bg-light-blue-gradient", "bg-light-black");
    return $config;
}

function create_new_UserSubgroupUIBlock($project_id, $subgroupids)
{
    $db = new DB($project_id, 'base');

    $config = getDefaultUserSubGroupUIBlock();
    $jsonstring = json_encode($config, JSON_UNESCAPED_UNICODE);
    $subgroupidList = explode(",", $subgroupids);

    foreach($subgroupidList as $subgroupid)
    {
        $db->query("INSERT INTO subgroupuiblock(subgroupid, jsonstring) VALUES ($subgroupid, '$jsonstring' ) ON DUPLICATE KEY UPDATE jsonstring=VALUES(jsonstring) ");
    }
}

function getUserSubgroupUIBlock($project_id, $subgroups)
{
    $config = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from subgroupuiblock where subgroupid in( $subgroups ) ");

    while($result = $db->fetch_array())
    {
        $uiblocks = json_decode($result['jsonstring']);

        $config_row_length = count($config);
        $uiblocks_length = count($uiblocks);

        // echo 'getUserSubgroupUIBlock() $config_row_length:'.$config_row_length. ' <br>';
        // echo 'getUserSubgroupUIBlock() $uiblocks_length:'.$uiblocks_length. ' <br>';

        // 檢查重複
        for($irow=0; $irow<$uiblocks_length; $irow++)
        {
            $uirowdata = $uiblocks[$irow];
            $uiblocks_col_length = count($uirowdata);

            // echo 'getUserSubgroupUIBlock() $uiblocks_col_length:'.$uiblocks_col_length. ' <br>';

            for($icol=0; $icol<$uiblocks_col_length; $icol++)
            {
                $uiblock = $uirowdata[$icol];

                $found = check_existed_block($config, $uiblock);
                if ( $found == false )
                {
                    // add
                    $config = put_to_existed_block($config, $irow, $icol, $uiblock);
                }
            }
        }
    }
    return $config;
}

function put_to_existed_block($config, $irow, $icol, $uiblock)
{
    // echo 'put_to_existed_block() $irow='.$irow.' $icol:'.$icol.'     <br>';

    $uirowdata = $config[$irow];
    if ( $uirowdata == null )
    {
        // echo 'put_to_existed_block() $uirowdata == null     <br>';

        $config[$irow][$icol] = $uiblock;
    }
    else
    {
        $existed_uiblock = $uirowdata[$icol];
        if ( $uirowdata == null )
        {
            $config[$irow][$icol] = $uiblock;
        }
        else
        {
            //
            // find empty space
            //
            $first_col = $icol;
            for ($itestrow = $irow; $itestrow < 10; $itestrow++)
            {
                $uirowdata = $config[$itestrow];
                if ( $uirowdata == null )
                {
                    $config[$irow][$icol] = $uiblock;
                    break;
                }

                $found = false;
                for ($itestcol = $first_col; $itestcol < 4; $itestcol++)
                {
                    $existed_uiblock = $uirowdata[$itestcol];
                    if ( $existed_uiblock == null )
                    {
                        $config[$itestrow][$itestcol] = $uiblock;
                        $found = true;
                        break;
                    }
                }
                $first_col = 0;
                if ($found == true)
                {
                    break;
                }
            }
        }
    }
    return $config;
}

function check_existed_block($config, $uiblock)
{
    $found = false;
    $config_row_length = count($config);

    if ($config_row_length == 0)
    {
        return false;
    }
    else
    {
        $found = false;
        for($irow=0; $irow<$config_row_length; $irow++)
        {
            $uirowdata = $config[$irow];
            $config_col_length = count($uirowdata);

            // echo 'check_existed_block() $irow:'. $irow. ' <br>';
            // echo 'check_existed_block() $config_col_length:'. $config_col_length. ' <br>';

            for($icol=0; ($icol<4) && ($icol<$config_col_length); $icol++)
            {
                // echo 'check_existed_block() $icol:'. $icol. ' <br>';

                // if( $uirowdata == null )
                // {
                //     echo 'check_existed_block() $uirowdata == null <br>';
                // }
                // else
                // {
                //     echo 'check_existed_block() $existed_uiblock != null <br>';
                //     echo 'check_existed_block() $uirowdata='.$uirowdata.' <br>';
                // }

                $existed_uiblock = $uirowdata[$icol];

                // if( $existed_uiblock == null )
                // {
                //     echo 'check_existed_block() $existed_uiblock == null <br>';
                // }
                // else
                // {
                //     echo 'check_existed_block() $existed_uiblock != null <br>';
                // }

                // echo 'check_existed_block() $existed_uiblock:'. $existed_uiblock. ' <br>';

                if ( $existed_uiblock->{'id'} == $uiblock->{'id'} )
                {
                    $found = true;
                    break;
                }
            }
            if ($found == true)
            {
                break;
            }
        }
    }

    return $found;
}

function fill_uiblock_info($config, $row, $col, $id, $action, $title, $context, $bgcolor, $btncolor)
{
    $config[$row][$col]["id"] = $id;
    $config[$row][$col]["action"] = $action;
    $config[$row][$col]["title"] = $title;
    $config[$row][$col]["context"] = $context;
    $config[$row][$col]["bgcolor"] =$bgcolor;
    $config[$row][$col]["btncolor"] = $btncolor;

    return $config;
}

function loadsubgroupuiblock($project_id)
{
    $uiblocks = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from subgroupuiblock ");

    while($result = $db->fetch_array())
    {
        $uiblock = [];
        $uiblock['subgroupid'] = $result['subgroupid'];
        $uiblock['uidata'] = json_decode($result['jsonstring']);

        $subgroupid = (int)$result['subgroupid'];
        $uiblocks[$subgroupid] = $uiblock;
    }

    // create default config
    $config = getDefaultSubgroupUIBlock();

    $uiblock = [];
    $uiblock['subgroupid'] = '0';
    $uiblock['uidata'] = $config;
    $uiblocks[0] = $uiblock;

    return $uiblocks;
}

function getDefaultSubgroupUIBlock()
{
    $config = [];

    $config = fill_uiblock_info($config, 0, 0, "id_course", "courses.php", "活動表", "團體活動資料", "bg-maroon-gradient", "bg-maroon");
    $config = fill_uiblock_info($config, 0, 1, "id_courserecord", "courserecord.php", "出席記錄", "課程表上課紀錄", "bg-green-gradient", "bg-green");
    $config = fill_uiblock_info($config, 0, 2, "id_bodyinfo", "bodyinforecord.php", "生理量測", "查看量測紀錄", "bg-yellow-gradient", "bg-yellow");
    $config = fill_uiblock_info($config, 0, 3, "id_inspection", "inspection.php", "巡檢記錄", "巡檢歷史紀錄", "bg-teal-gradient", "bg-teal");
    $config = fill_uiblock_info($config, 1, 0, "id_pushnotificationrecords", "pushnotificationrecords.php", "推播紀錄", "顯示推播紀錄", "bg-aqua-gradient", "bg-aqua");
    $config = fill_uiblock_info($config, 1, 1, "id_cards", "mycards.php", "關連卡片", "顯示我的卡片", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 1, 2, "id_userrollcall", "userrollcallrecord.php", "人員點名", "顯示人員點名紀錄", "bg-light-blue-gradient", "bg-light-blue");
    $config = fill_uiblock_info($config, 1, 3, "id_qrcode", "javascript:showqrcode();", "QR Code", "我的 QR Code", "bg-black-gradient", "bg-black");
    $config = fill_uiblock_info($config, 2, 0, "id_weekschedule", "weekschedule.php", "功課表", "班級功課表", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 2, 1, "id_storagefile", "storagefile.php", "檔案庫", "班級檔案庫", "bg-light-blue-gradient", "bg-light-blue");
    $config = fill_uiblock_info($config, 2, 2, "id_storageimage", "storageimage.php", "相簿", "班級相簿", "bg-teal-gradient", "bg-teal");
    $config = fill_uiblock_info($config, 2, 3, "id_applicationformleave", "applicationformleave.php", "請假單", "我的請假單", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 3, 0, "id_assist", "assistrecords.php", "協助事項", "我的協助事項", "bg-green-gradient", "bg-green");
    $config = fill_uiblock_info($config, 3, 1, "id_billboardrecords", "billboardrecords.php", "公佈欄", "查看公佈事項", "bg-teal-gradient", "bg-teal");

    $config = fill_uiblock_info($config, 3, 2, "id_subgroup", "usersubgroupinfo.php", "子集合", "子集合管理", "bg-maroon-gradient", "bg-maroon");
    $config = fill_uiblock_info($config, 3, 3, "id_userinfo", "userinfo.php", "人員列表", "人員管理", "bg-yellow-gradient", "bg-yellow");
    $config = fill_uiblock_info($config, 4, 0, "id_pushnotification", "pushnotification.php", "推播訊息", "推播App訊息", "bg-purple-gradient", "bg-purple");
    $config = fill_uiblock_info($config, 4, 1, "id_curriculum", "curriculumapply.php", "選修課程", "提供學生選修課程", "bg-light-blue-gradient", "bg-light-blue");
    $config = fill_uiblock_info($config, 4, 2, "id_curriculumschedule", "curriculumschedule.php", "課程表", "選修課程表", "bg-light-blue-gradient", "bg-light-black");

    return $config;
}

function updatesubgroupuiblock($project_id, $subgroupids, $uidata)
{
    $db = new DB($project_id, 'base');

    $jsonstring = $uidata;
    $subgroupidList = explode(",", $subgroupids);

    foreach($subgroupidList as $subgroupid)
    {
        $db->query("INSERT INTO subgroupuiblock(subgroupid, jsonstring) VALUES ($subgroupid, '$jsonstring' ) ON DUPLICATE KEY UPDATE jsonstring=VALUES(jsonstring) ");
    }

    return $uidata;
}

?>
