<?php
// Report simple running errors
ini_set('error_reporting', 0);
ini_set('display_errors', 0);
// ini_set('display_errors', '1');
// ini_set('display_startup_errors', '1');
// error_reporting(E_ALL);
// error_reporting(E_ERROR | E_WARNING | E_PARSE);

require_once __DIR__.'/../config.php';
require_once("DB_Class.php");

#
# get device status from UDP port
#
function packageJSON($status_str)
{
    $status_array = explode("\n", $status_str);
    $emptyRemoved = array_filter($status_array);

    $status = [];
    foreach ($emptyRemoved as $item_num => $item)
    {
        $pair = explode("=", $item);
        $status[$pair[0]] = $pair[1];
    }

    return $status;
}

#
# get device status from UDP port
#
function getServerInfo($type)
{
    //Reduce errors
    //error_reporting(~E_WARNING);

    $server = 'localhost';
    $port = get_Web_port();

    if(!($sock = socket_create(AF_INET, SOCK_DGRAM, 0)))
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);

        die("Couldn't create socket: [$errorcode] $errormsg \n");
    }

    $timeout = array('sec'=>20,'usec'=>0);
    socket_set_option($sock, SOL_SOCKET, SO_RCVTIMEO, $timeout);

    #echo "Socket created \n";
    #Send the message to the server
    if( ! socket_sendto($sock, $type , strlen($type) , 0 , $server , $port))
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);

        die("Could not send data: [$errorcode] $errormsg \n");
    }

    //Now receive reply from server and print it
    //if(socket_recv ( $sock , $reply , 2045 , MSG_WAITALL ) === FALSE)
    if(socket_recv ( $sock , $reply , 65535 , MSG_WAITALL ) === FALSE)
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);

        die("Could not receive data: [$errorcode] $errormsg \n");
    }

    #echo "Reply : $reply";

    socket_close($sock);

    return $reply;
}

#
# send message to server
#
function sendMessageToServer($send_string)
{
    //Reduce errors
    //error_reporting(~E_WARNING);
    writeLogFile([$send_string], 'logfiletest.txt');

    $server = 'localhost';
    $port = get_Web_port();

    if(!($sock = socket_create(AF_INET, SOCK_DGRAM, 0)))
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        // writeLogFile([$errormsg], 'logfiletest.txt');

        die("Couldn't create socket: [$errorcode] $errormsg \n");
    }

    #echo "Socket created \n";

    #Send the message to the server
    //die("msgservicestring:$send_string \n");
    if( ! socket_sendto($sock, $send_string , strlen($send_string) , 0 , $server , $port))
    {
        $errorcode = socket_last_error();
        $errormsg = socket_strerror($errorcode);
        // writeLogFile([$errormsg], 'logfiletest.txt');

        die("Could not send data: [$errorcode] $errormsg \n");
    }

    socket_close($sock);

}

#
# read log file to table
#
function readLogFile($file_name)
{
    $file = file_get_contents($file_name, true);

    $status_array = explode("\n", $file);
    $emptyRemoved = array_filter($status_array);

    $status = [];
    foreach ($emptyRemoved as $item_num => $item)
    {
        $pair = explode("=", $item);
        $status[$pair[0]] = $pair[1];
    }

    return $status;
}

#
# write log file
#
function writeLogFile($pair_array, $file_name)
{
    #file_put_contents($file_name, print_r($pair_array, true));

    $text = "";
    foreach($pair_array as $key => $value)
    {
        $text .= $key."=".$value."\n";
    }
    $fh = fopen($file_name, "w");// or die("Could not open log file.");
    fwrite($fh, $text) or die("Could not write file!");
    fclose($fh);
}

function get_issuetime_list()
{
    $issues = array();
    $issues['0'] = 'Immediately';
    $issues['1'] = '1 minute';
    $issues['5'] = '5 minutes';
    $issues['10'] = '10 minutes';

    return $issues;
}

function get_issuetime_name($issuetime)
{
    $issuename = $issuetime;
    if($issuetime == '0')
        $issuename = 'Immediately';
    if($issuetime == '1')
        $issuename = '1 minute';
    if($issuetime == '5')
        $issuename = '5 minutes';
    if($issuetime == '10')
        $issuename = '10 minutes';

    return $issuename;
}

function get_action_list()
{
    $actions = array();
    $actions['Enter'] = 'Enter';
    $actions['Leave'] = 'Leave';
    $actions['Stay'] = 'Stay';
    $actions['Alert'] = 'Alert';
    $actions['Cancel'] = 'Cancel';
    $actions['enter'] = 'Enter';
    $actions['leave'] = 'Leave';
    $actions['stay'] = 'Stay';
    $actions['alert'] = 'Alert';
    $actions['cancel'] = 'Cancel';
    $actions['finish'] = 'Finish';
    $actions['exceed'] = 'Exceed';
    $actions['shorttime'] = 'shorttime';
    $actions['longtime'] = 'longtime';
    $actions['CallOut'] = 'CallOut';
    $actions['PowerOff'] = 'PowerOff';
    $actions['PowerOn'] = 'PowerOn';
    $actions['OpenDoor'] = 'OpenDoor';
    $actions['CloseDoor'] = 'CloseDoor';
    $actions['EnterDoor'] = 'EnterDoor';
    $actions['ExitDoor'] = 'ExitDoor';
    $actions['SensorAlert'] = '附近有異常人物入侵, 請儘速巡視!';
    $actions['unknowncard'] = '發現未註冊卡片';

    return $actions;
}

function updateMap($layerid, $maingroupid, $projectid, $layer_name, $POSX, $POSY, $SCALE, $COEFF, $FILE, $MAPWIDTH, $MAPHEIGHT)
{
    $json_array = [];
    $json_array['project_id'] = $projectid;
    $json_array['layerid'] = $layerid;
    $json_array['maingroupid'] = $maingroupid;
    $json_array['layer_name'] = $layer_name;
    $json_array['POSX'] = $POSX;
    $json_array['POSY'] = $POSY;
    $json_array['SCALE'] = $SCALE;
    $json_array['COEFF'] = $COEFF;
    $json_array['FILE'] = $FILE;
    $json_array['MAPWIDTH'] = $MAPWIDTH;
    $json_array['MAPHEIGHT'] = $MAPHEIGHT;
    $json_string = json_encode($json_array);

    $send_string = 'updatemap=' . $json_string;
    sendMessageToServer($send_string);
}

function getMaps($projectid, $groupid, $maingroupid)
{
    $db = new DB($projectid, 'base');

    if ($groupid == '1' || $groupid == '2')
        $check_res = $db->query("SELECT * FROM maplayer");
    else
        $check_res = $db->query("SELECT * FROM maplayer where maingroupid in($maingroupid)");

    $Maps = [];

    if ( $check_res )
    {
        $index = 0;
        while($result = $db->fetch_array())
        {
            $Map = [];
            $Map['layerid'] = $result['layerid'];
            $Map['maingroupid'] = $result['maingroupid'];
            $Map['layer_name'] = $result['layer_name'];
            $Map['layer_description'] = $result['layer_description'];
            $Map['WGS48OriginX'] = $result['WGS48OriginX'];
            $Map['WGS48OriginY'] = $result['WGS48OriginY'];
            $Map['MAP_North'] = $result['MAP_North'];
            $Map['BuildingID'] = $result['BuildingID'];
            $Map['BuildingFloor'] = $result['BuildingFloor'];
            $Map['POSX'] = $result['POSX'];
            $Map['POSY'] = $result['POSY'];
            $Map['SCALE'] = $result['SCALE'];
            $Map['COEFF'] = $result['COEFF'];
            $Map['FILE'] = $result['FILE'];
            $Map['MAPWIDTH'] = $result['MAPWIDTH'];
            $Map['MAPHEIGHT'] = $result['MAPHEIGHT'];

            $Maps[$index] = $Map;
            $index += 1;
        }
    }

    if ( count($Maps) == 0 )
    {
        $Map = create_maplayer($projectid, 0, $maingroupid, 'New Layer');
        $Maps[0] = $Map;
    }
    return $Maps;
}

function getMapLayerIds($projectid, $groupid, $maingroupid)
{
    $db = new DB($projectid, 'base');

    if ($groupid == '1' || $groupid == '2')
        $check_res = $db->query("SELECT * FROM maplayer");
    else
        $check_res = $db->query("SELECT * FROM maplayer where maingroupid in($maingroupid)");

    $layerids = [];

    if ( $check_res )
    {
        $index = 0;
        while($result = $db->fetch_array())
        {
            $layerids[$index] = $result['layerid'];
            $index += 1;
        }

    }
    return $layerids;
}

function updateMapLayer($maplayer)
{
    $json_array = [];
    $json_array['project_id'] = $maplayer['project_id'];
    $json_array['layerid'] = $maplayer['layerid'];
    $json_array['maingroupid'] = $maplayer['maingroupid'];
    $json_array['layer_name'] = $maplayer['layer_name'];
    $json_array['layer_description'] = $maplayer['layer_description'];
    $json_array['WGS48OriginX'] = $maplayer['WGS48OriginX'];
    $json_array['WGS48OriginY'] = $maplayer['WGS48OriginY'];
    $json_array['MAP_North'] = $maplayer['MAP_North'];
    $json_array['BuildingID'] = $maplayer['BuildingID'];
    $json_array['BuildingFloor'] = $maplayer['BuildingFloor'];
    $json_array['POSX'] = $maplayer['POSX'];
    $json_array['POSY'] = $maplayer['POSY'];
    $json_array['SCALE'] = $maplayer['SCALE'];
    $json_array['COEFF'] = $maplayer['COEFF'];
    $json_array['FILE'] = $maplayer['FILE'];
    $json_array['MAPWIDTH'] = $maplayer['MAPWIDTH'];
    $json_array['MAPHEIGHT'] = $maplayer['MAPHEIGHT'];
    $json_string = json_encode($json_array);

    $send_string = 'updatemaplayer=' . $json_string;
    sendMessageToServer($send_string);
}

function getMapLayer($projectid, $maplayerid)
{
    $db = new DB($projectid, 'base');
    $db->query("SELECT * FROM maplayer WHERE layerid=". $maplayerid ." ");

    $maplayer = [];
    $maplayer['project_id'] = $projectid;
    $maplayer['layerid'] = 1;
    $maplayer['maingroupid'] = 1;
    $maplayer['layer_name'] = 'layer_name';
    $maplayer['layer_description'] = 'Layer Description';
    $maplayer['WGS48OriginX'] = '121.513362';
    $maplayer['WGS48OriginY'] = '25.044696';
    $maplayer['MAP_North'] = '0.0';
    $maplayer['BuildingID'] = 1;
    $maplayer['BuildingFloor'] = 1;
    $maplayer['POSX'] = '1187.897123336792';
    $maplayer['POSY'] = '1488.5751976966858';
    $maplayer['SCALE'] = '11.183189941629768';
    $maplayer['COEFF'] = '100000.0';
    $maplayer['FILE'] = '/upload/field.png';
    $maplayer['MAPWIDTH'] = '1400';
    $maplayer['MAPHEIGHT'] = '900';


    while($result = $db->fetch_array())
    {
        $maplayer['project_id'] = $result['project_id'];
        $maplayer['layerid'] = $result['layerid'];
        $maplayer['maingroupid'] = $result['maingroupid'];
        $maplayer['layer_name'] = $result['layer_name'];
        $maplayer['layer_description'] = $result['layer_description'];
        $maplayer['WGS48OriginX'] = $result['WGS48OriginX'];
        $maplayer['WGS48OriginY'] = $result['WGS48OriginY'];
        $maplayer['MAP_North'] = $result['MAP_North'];
        $maplayer['BuildingID'] = $result['BuildingID'];
        $maplayer['BuildingFloor'] = $result['BuildingFloor'];
        $maplayer['POSX'] = $result['POSX'];
        $maplayer['POSY'] = $result['POSY'];
        $maplayer['SCALE'] = $result['SCALE'];
        $maplayer['COEFF'] = $result['COEFF'];
        $maplayer['FILE'] = $result['FILE'];
        $maplayer['MAPWIDTH'] = $result['MAPWIDTH'];
        $maplayer['MAPHEIGHT'] = $result['MAPHEIGHT'];

        break;
    }

    return $maplayer;
}

function update_map($Map)
{
    $db = new DB($projectid, 'base');

    $db->query("INSERT INTO Map(layerid, layer_name, layer_description, POSX, POSY, SCALE, COEFF, FILE, MAPWIDTH, MAPHEIGHT) " .
               "VALUES ('".$Map['layerid']."', '".$Map['layer_name']."', '".$Map['layer_description']."', '".$Map['POSX']."', '" .
               $Map['POSY']."', '".$Map['SCALE']."', '".$Map['COEFF']."', '".$Map['FILE']."', '".$Map['MAPWIDTH']."', '" .
               $Map['MAPHEIGHT']."')");
}

function create_maplayer($projectid, $maplayerid, $maingroupid, $layer_name)
{
    $db = new DB($projectid, 'base');

    $Map = [];
    // set default value
    $Map['layerid'] = $maplayerid;
    $Map['maingroupid'] = $maingroupid;
    $Map['layer_name'] = $layer_name;
    $Map['layer_description'] = 'Layer Description';
    $Map['WGS48OriginX'] = '121.513362';
    $Map['WGS48OriginY'] = '25.044696';
    $Map['MAP_North'] = '0.0';
    $Map['BuildingID'] = 1;
    $Map['BuildingFloor'] = 1;
    $Map['POSX'] = '1187.897123336792';
    $Map['POSY'] = '1488.5751976966858';
    $Map['SCALE'] = '11.183189941629768';
    $Map['COEFF'] = '100000.0';
    $Map['FILE'] = '/upload/field.png';
    $Map['MAPWIDTH'] = '1400';
    $Map['MAPHEIGHT'] = '900';

    if ( $maplayerid == 0 )
    {
        $db->query("INSERT INTO maplayer(maingroupid, layer_name, layer_description, WGS48OriginX, WGS48OriginY, MAP_North, BuildingID, BuildingFloor, " .
                   "POSX, POSY, SCALE, COEFF, FILE, MAPWIDTH, MAPHEIGHT ) " .
                   "VALUES (".$Map['maingroupid'].", '".$Map['layer_name']."', '".$Map['layer_description']."', '".$Map['WGS48OriginX']."', '".$Map['WGS48OriginY']."', '" .
                   $Map['MAP_North']."', '".$Map['BuildingID']."', '".$Map['BuildingFloor']."', '".$Map['POSX']."', '" .$Map['POSY']."', '" .
                   $Map['SCALE']."', '".$Map['COEFF']."', '".$Map['FILE']."', '".$Map['MAPWIDTH']."', '".$Map['MAPHEIGHT']."')");

       $Map['layerid'] = $db->get_insert_id();
    }
    else
    {
        $db->query("INSERT INTO maplayer(layerid, maingroupid, layer_name, layer_description, WGS48OriginX, WGS48OriginY, MAP_North, BuildingID, BuildingFloor, " .
                   "POSX, POSY, SCALE, COEFF, FILE, MAPWIDTH, MAPHEIGHT ) " .
                   "VALUES (".$maplayerid.",".$Map['maingroupid'].",'".$Map['layer_name']."', '".$Map['layer_description']."', '".$Map['WGS48OriginX']."', '".$Map['WGS48OriginY']."', '" .
                   $Map['MAP_North']."', '".$Map['BuildingID']."', '".$Map['BuildingFloor']."', '".$Map['POSX']."', '" .$Map['POSY']."', '" .
                   $Map['SCALE']."', '".$Map['COEFF']."', '".$Map['FILE']."', '".$Map['MAPWIDTH']."', '".$Map['MAPHEIGHT']."')");
    }


    return $Map;
}

function delete_maplayer($projectid, $maplayerid)
{
    $db = new DB($projectid, 'base');

    $db->query("DELETE FROM maplayer WHERE layerid=".$maplayerid." ");
}

function delete_maplayerarea_all($projectid, $maplayerid)
{
    $db = new DB($projectid, 'base');

    $db->query("DELETE FROM maplayerarea WHERE layerid=".$maplayerid." ");
}

function getMaps_old($projectid)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($projectid, 'base');

    $db->query("SELECT * FROM Map WHERE PROJECT_ID=".$projectid." ");

    $Map = [];

    // set default value
    $Map['PROJECT_ID'] = $projectid;
    $Map['mapid'] = 1;
    $Map['mapname'] = 'map';
    $Map['POSX'] = '0.0';
    $Map['POSY'] = '0.0';
    $Map['SCALE'] = '51.933455519444976';
    $Map['COEFF'] = '100000.0';
    $Map['FILE'] = '/upload/2dgrid.png';
    $Map['MAPWIDTH'] = '600';
    $Map['MAPHEIGHT'] = '600';

    while($result = $db->fetch_array())
    {
        $Map['PROJECT_ID'] = $result['PROJECT_ID'];
        $Map['mapid'] = $result['mapid'];
        $Map['mapname'] = $result['mapname'];
        $Map['POSX'] = $result['POSX'];
        $Map['POSY'] = $result['POSY'];
        $Map['SCALE'] = $result['SCALE'];
        $Map['COEFF'] = $result['COEFF'];
        $Map['FILE'] = $result['FILE'];
        $Map['MAPWIDTH'] = $result['MAPWIDTH'];
        $Map['MAPHEIGHT'] = $result['MAPHEIGHT'];
        break;
    }
    //$db->close();

    if (strlen($Map['mapname']) == 0)
        $Map['mapname'] = 'map';
    if (strlen($Map['POSX']) == 0)
        $Map['POSX'] = '0.0';
    if (strlen($Map['POSY']) == 0)
        $Map['POSY'] = '0.0';
    if (strlen($Map['SCALE']) == 0)
        $Map['SCALE'] = '51.933455519444976';
    if (strlen($Map['COEFF']) == 0)
        $Map['COEFF'] = '100000.0';
    if (strlen($Map['FILE']) == 0)
        $Map['FILE'] = '/upload/2dgrid.png';
    if (strlen($Map['MAPWIDTH']) == 0)
        $Map['MAPWIDTH'] = '600';
    if (strlen($Map['MAPHEIGHT']) == 0)
        $Map['MAPHEIGHT'] = '600';


    return $Map;
}

function print_fail_msg($msg)
{
    $json_array = [];
    $json_array['isOk'] = false;
    $json_array['msg'] = $msg;
    $json_string = json_encode($json_array);

    echo $json_string;
}

function print_success_msg($id)
{
    $json_array = [];
    $json_array['isOk'] = true;
    $json_array['id'] = $id;
    $json_string = json_encode($json_array);

    echo $json_string;
}

function print_success_uploadImg_msg($fileName, $filePath)
{
    $json_array = [];
    $json_array['isOk'] = true;
    $json_array['fileName'] = $fileName;
    $json_array['filePath'] = $filePath;
    $json_string = json_encode($json_array);

    echo $json_string;
}

function get_area_name($projectid, $areaid)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($projectid, 'base');

    $db->query("SELECT * FROM maplayerarea WHERE areaid='".$areaid."' ");

    $areaname = $areaid;
    while($result = $db->fetch_array())
    {
        $areaname = $result['areaname'];
    }
    //$db->close();

    return $areaname;
}

function get_node_name($projectid, $nodeid)
{
    $db = new DB($projectid, 'base');

    $db->query("SELECT * FROM NodeInfo WHERE nodeid='".$nodeid."' ");

    $nodename = $nodeid;
    while($result = $db->fetch_array())
    {
        $nodename = $result['nodename'];
    }

    return $nodename;
}

function get_user_name($projectid, $userid)
{
    $db = new DB($projectid, 'base');

    $db->query("SELECT name FROM User WHERE userid=$userid ");

    $username = $userid;
    if($result = $db->fetch_array())
    {
        $username = $result['name'];
    }

    return $username;
}

function get_node_macaddress($projectid, $nodeid)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($projectid, 'base');

    $db->query("SELECT macaddress FROM NodeInfo WHERE nodeid='".$nodeid."' ");

    $macaddress = "";
    while($result = $db->fetch_array())
    {
        $macaddress = $result['macaddress'];
    }
    //$db->close();

    return $macaddress;
}

function get_node_nodeid_from_userid($projectid, $userid)
{
    $db = new DB($projectid, 'base');

    $db->query("SELECT nodeid FROM NodeInfo WHERE userid='".$userid."' ");

    $nodeid = "";
    while($result = $db->fetch_array())
    {
        $nodeid = $result['nodeid'];
    }
    return $nodeid;
}
// function get_course($projectid, $courseId)
// {
//     // global $_DB;
//     // $db = new DB();
//     // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
//     $db = new DB($projectid, 'base');
//
//     $db->query("SELECT * FROM Course WHERE courseId='".$courseId."' ");
//
//     $course = "";
//     while($result = $db->fetch_array())
//     {
//         $course = $result;
//     }
//     //$db->close();
//
//     return $course;
// }

function get_User_data($useraccount)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB(1, 'base');

    $db->query("SELECT * FROM User WHERE account='".$useraccount."' ");

    $user_data = [];
    $user_data['account'] = $useraccount;
    while($result = $db->fetch_array())
    {
        $user_data['name'] = $result['name'];
        $user_data['groupid'] = $result['groupid'];
    }
    //$db->close();

    return $user_data;
}

function get_ERP_IP()
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB(1, 'base');

    $db->query("SELECT ERPServer FROM systemconfig ");

    $IP = '';
    while($result = $db->fetch_array())
    {
        $IP = $result['ERPServer'];
    }
    //$db->close();

    return $IP;
}

function get_SSO_IP()
{
    global $_DB;
    $db = new DB(1, 'base');
    $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);

    $db->query("SELECT SSOIP FROM systemconfig ");

    $IP = '';
    while($result = $db->fetch_array())
    {
        $IP = $result['SSOIP'];
    }
    //$db->close();
    return $IP;
}

function get_Web_port()
{
    global $_DB;

    $web_udp_port = 18613;
    $has_memcached = true;

    $cached_name = $_DB['username'].'web_udp_port';

    try {
        echo 'test';
        $memcached = new Memcached();
        echo 'test2';
        $memcached->addServer('127.0.0.1',11211);
        $web_udp_port = $memcached->get($cached_name);

        if($web_udp_port)
        {
            return $web_udp_port;
        }
    }
    catch (Throwable $t)
    {
        //echo $t->getMessage();
        $has_memcached = false;
    }
    catch (Exception $e)
    {
        //echo $e->getMessage();
        $has_memcached = false;
    }

    $db = new DB(1, 'base');
    $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);

    $db->query("SELECT web_udp_port FROM systemconfig where CONFIG_ID = 1 ");

    while($result = $db->fetch_array())
    {
        $web_udp_port = (int)$result['web_udp_port'];
    }

    if($has_memcached)
    {
        $memcached->add($cached_name, $web_udp_port, 1800);
    }

    return $web_udp_port;
}

function checkValidProject($project_id)
{
    $valid = FALSE;
    $db = new DB(1, 'config');
    $db->query("SELECT * from config WHERE PROJECT_ID=" . $project_id);
    while($result = $db->fetch_array())
    {
        if ( $project_id == $result['PROJECT_ID'] )
            $valid = TRUE;
    }
    //$db->close();
    return $valid;
}

function check_valid_apikey($key)
{
    $db = new DB(1, 'base');

    $db->query("SELECT count(*) as count from APIKey WHERE apikey='$key' AND enable=1 ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $count = $result['count'];
    }
    if ( $count > 0 )
        return TRUE;
    else
        return FALSE;
}

?>
