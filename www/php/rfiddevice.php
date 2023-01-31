<?php

session_start();

require_once("common.php");
require_once("User.php");
require_once("cardrollcall.php");

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

    if (isset($_GET['devicecount']))
    {
        $count = devicecount($project_id);
        $status = [];
        $status['DEVICE_NUM'] = $count;
        echo json_encode($status);
    }
    else
    if (isset($_GET['loaddevice']))
    {
        $count = 0;
        $offset = 0;
        $showonline = 1;
        $showoffline = 1;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['showonline']))
        {
            $showonline = (int)$_GET['showonline'];
        }
        if (isset($_GET['showoffline']))
        {
            $showoffline = (int)$_GET['showoffline'];
        }

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['start'] = $offset;
        $json_array['count'] = $count;
        $json_array['showonline'] = $showonline;
        $json_array['showoffline'] = $showoffline;
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_string = json_encode($json_array);
        $status = getServerInfo('rfiddevicetatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadcard']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $showonline = 1;
        $showoffline = 1;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $json_array['start'] = (int)$_GET['s'];
            $json_array['count'] = (int)$_GET['count'];
        }
        if (isset($_GET['showonline']))
        {
            $showonline = (int)$_GET['showonline'];
        }
        if (isset($_GET['showoffline']))
        {
            $showoffline = (int)$_GET['showoffline'];
        }

        $json_array['showonline'] = $showonline;
        $json_array['showoffline'] = $showoffline;
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status = getServerInfo('rfidcardtatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['querycards']))
    {
        $cardid = $_GET['cardid'];

        $cardidlist = explode(",", $cardid);
        $cardids = '\'' . implode("','", $cardidlist).'\'';

        $status = queryRFIDcards($project_id, $cardids);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcardscan']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];

        $count = 0;
        $offset = 0;
        $matchname = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['matchname']))
        {
            $matchname = $_GET['matchname'];
        }

        $status = loadrfidcardscan($project_id, $offset, $count, $matchname);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadunknowncard']))
    {
        $count = 0;
        $offset = 0;
        $showonline = 1;
        $showoffline = 1;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['showonline']))
        {
            $showonline = (int)$_GET['showonline'];
        }
        if (isset($_GET['showoffline']))
        {
            $showoffline = (int)$_GET['showoffline'];
        }

        $status = loadunknowncard($project_id, $showonline, $showoffline, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcardrollcall']))
    {
        $count = 0;
        $offset = 0;
        $rollcallid = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['rollcallid']))
        {
            $rollcallid = (int)$_GET['rollcallid'];
        }

        if ( $rollcallid == 0)
        {
            $status = loadcardrollcalls($project_id, $offset, $count);
            echo json_encode($status);
        }
        else
        {
            $status = loadcardrollcall($project_id, $rollcallid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadcardrollcallrecord']))
    {
        $count = 0;
        $offset = 0;
        $rollcallid = 0;
        $eventid = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['rollcallid']))
        {
            $rollcallid = (int)$_GET['rollcallid'];
        }
        if (isset($_GET['eventid']))
        {
            $eventid = (int)$_GET['eventid'];
        }
        if ($rollcallid == 0 || $eventid == 0)
        {
            $status = loadcardrollcallrecord($project_id, $offset, $count);
            echo json_encode($status);
        }
        else
        {
            $status = loadcardrollcallrecord_singleevent($project_id, $rollcallid, $eventid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadcardrollcallevent']))
    {
        $count = 0;
        $offset = 0;
        $eventid = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['eventid']))
        {
            $eventid = (int)$_GET['eventid'];
        }

        if ($eventid == 0)
        {
            $status = loadcardrollcallevents($project_id, $offset, $count);
            echo json_encode($status);
        }
        else
        {
            $status = loadcardrollcallevent($project_id, $eventid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadcardrollcallsubgroupevent']))
    {
        $eventids = '';
        if (isset($_GET['eventids']))
        {
            $eventids = $_GET['eventids'];
        }
        $status = loadcardrollcallsubgroupevent($project_id, $eventids);
        echo json_encode($status);
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

    if (isset($_POST['create_new_rfiddevice']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['macaddress'] = $_POST['macaddress'];
        $device['name'] = $_POST['name'];
        $device['type'] = $_POST['type'];
        $device['ip'] = $_POST['ip'];
        $device['gwid'] = $_POST['gwid'];
        $device['LoRAMacaddress'] = $_POST['LoRAMacaddress'];

        save_new_rfiddevice($project_id, $device);

        sendMessageToServer('update_rfiddevice');
        // sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['update_device']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['alive'] = $_POST['alive'];
        $device['deviceid'] = $_POST['deviceid'];
        $device['macaddress'] = $_POST['macaddress'];
        $device['name'] = $_POST['name'];
        //$device['type'] = $_POST['type'];
        $device['ip'] = $_POST['ip'];
        $device['gwid'] = $_POST['gwid'];
        $device['LoRAMacaddress'] = $_POST['LoRAMacaddress'];

        update_rfiddevice($project_id, $device);

        sendMessageToServer('update_rfiddevice');
        // sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_device']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['deviceid'] = $_POST['deviceid'];

        $deviceid = $_POST['deviceid'];

        delete_rfiddevice($project_id, $deviceid);

        sendMessageToServer('update_rfiddevice');
        // sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['update_rfidcard_name']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];
        $device['cardname'] = $_POST['cardname'];

        $cardid = $_POST['cardid'];
        $cardname = $_POST['cardname'];

        update_rfidcard($project_id, $cardid, $cardname);

        sendMessageToServer('update_rfidcard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['create_new_rfidcard']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];

        $cardid = $_POST['cardid'];

        save_new_rfidcard($project_id, $cardid);

        sendMessageToServer('update_rfidcard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_card']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];

        $cardid = $_POST['cardid'];

        delete_rfidcard($project_id, $cardid);

        delete_rfidcard_from_User($project_id, $cardid);

        sendMessageToServer('update_rfidcard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['register_card']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];

        $cardid = $_POST['cardid'];

        register_rfidcard($project_id, $cardid);

        sendMessageToServer('update_rfidcard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_all_unknowncard']))
    {
        $device = [];
        $device['project_id'] = $project_id;

        delete_all_unknowncard($project_id);

        //sendMessageToServer('update_rfidcard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['create_new_cardrollcall']))
    {
        $rollcall = [];
        $rollcall['project_id'] = $project_id;
        $rollcall['rollcallname'] = $_POST['rollcallname'];
        $rollcall['StartTime'] = $_POST['StartTime'];
        $rollcall['EndTime'] = $_POST['EndTime'];
        $rollcall['StartDate'] = $_POST['StartDate'];
        $rollcall['EndDate'] = $_POST['EndDate'];
        $rollcall['rfiddeviceids'] = $_POST['rfiddeviceids'];
        $rollcall['nfclocators'] = $_POST['nfclocators'];
        $rollcall['subgroups'] = $_POST['subgroups'];
        $rollcall['weekday'] = $_POST['weekday'];
        $rollcall['validdate'] = $_POST['validdate'];
        $rollcall['invaliddate'] = $_POST['invaliddate'];
        $rollcall['presentnotify'] = $_POST['presentnotify'];
        $rollcall['presentcomment'] = $_POST['presentcomment'];
        $rollcall['absentnotify'] = $_POST['absentnotify'];
        $rollcall['absentcomment'] = $_POST['absentcomment'];
        $rollcall['resend'] = $_POST['resend'];
        $rollcall['maingroups'] = $_SESSION["maingroups"];
        $rollcall['type'] = $_POST['type'];
        $rollcall['curriculumrecordid'] = $_POST['curriculumrecordid'];

        save_new_rfidrollcall($project_id, $rollcall);

        sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($rollcall);
        echo $json_string;
    }
    else
    if (isset($_POST['update_cardrollcall']))
    {
        $rollcall = [];
        $rollcall['project_id'] = $project_id;
        $rollcall['rollcallid'] = $_POST['rollcallid'];
        $rollcall['rollcallname'] = $_POST['rollcallname'];
        $rollcall['StartTime'] = $_POST['StartTime'];
        $rollcall['EndTime'] = $_POST['EndTime'];
        $rollcall['StartDate'] = $_POST['StartDate'];
        $rollcall['EndDate'] = $_POST['EndDate'];
        $rollcall['rfiddeviceids'] = $_POST['rfiddeviceids'];
        $rollcall['nfclocators'] = $_POST['nfclocators'];
        $rollcall['subgroups'] = $_POST['subgroups'];
        $rollcall['weekday'] = $_POST['weekday'];
        $rollcall['validdate'] = $_POST['validdate'];
        $rollcall['invaliddate'] = $_POST['invaliddate'];
        $rollcall['presentnotify'] = $_POST['presentnotify'];
        $rollcall['presentcomment'] = $_POST['presentcomment'];
        $rollcall['absentnotify'] = $_POST['absentnotify'];
        $rollcall['absentcomment'] = $_POST['absentcomment'];
        $rollcall['resend'] = $_POST['resend'];
        $rollcall['maingroups'] = $_POST["maingroups"];
        $rollcall['type'] = $_POST['type'];
        $rollcall['curriculumrecordid'] = $_POST['curriculumrecordid'];

        update_cardrollcall($project_id, $rollcall);

        sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($rollcall);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_cardrollcall']))
    {
        $rollcall = [];
        $rollcall['project_id'] = $project_id;
        $rollcall['rollcallid'] = $_POST['rollcallid'];

        $rollcallid = $_POST['rollcallid'];

        delete_cardrollcall($project_id, $rollcallid);

        sendMessageToServer('updateCARDRollCall');

        $json_string = json_encode($rollcall);
        echo $json_string;
    }
    else
    if (isset($_POST['scan_rfidcard']))
    {
        sendMessageToServer('scanRFIDCard');

        $json_string = json_encode($_POST);
        echo $json_string;
    }
    else
    if (isset($_POST['update_rollcall_record']))
    {
        $rollcallresult = [];
        $rollcallresult['project_id'] = $project_id;
        $rollcallresult['rollcallid'] = $_POST['rollcallid'];
        $rollcallresult['eventid'] = $_POST['eventid'];
        $rollcallresult['cardid'] = $_POST['cardid'];
        $rollcallresult['userid'] = $_POST['userid'];

        $json_string = json_encode($rollcallresult);

        $send_string = 'updaterollcallrecord=' . $json_string;
        sendMessageToServer($send_string);
        echo $json_string;
    }
    else
    if (isset($_POST['update_rollcall_record_batch']))
    {
        $rollcallresult = [];
        $rollcallresult['project_id'] = $project_id;
        $rollcallresult['rollcallid'] = $_POST['rollcallid'];
        $rollcallresult['eventid'] = $_POST['eventid'];
        $rollcallresult['cardids'] = $_POST['cardids'];
        $rollcallresult['userids'] = $_POST['userids'];

        $json_string = json_encode($rollcallresult);

        $send_string = 'updaterollcallrecords=' . $json_string;
        sendMessageToServer($send_string);
        echo $json_string;
    }
}

function REST_do_DELETE()
{
}

function save_new_rfiddevice($project_id, $device)
{
    $db = new DB($project_id, 'base');

    $macaddress = $device['macaddress'];
    $name = $device['name'];
    $type = $device['type'];
    $ip = $device['ip'];
    $gwid = $device['gwid'];
    $LoRAMacaddress = $device['LoRAMacaddress'];

    $db->query("INSERT INTO RFIDDevice(name, type, ip, macaddress, LoRAMacaddress, gwid) " .
                "VALUES ('$name', '$type', '$ip', '$macaddress', '$LoRAMacaddress', '$gwid')");
}

function update_rfiddevice($project_id, $device)
{
    $db = new DB($project_id, 'base');

    $alive = $device['alive'];
    $deviceid = $device['deviceid'];
    $macaddress = $device['macaddress'];
    $name = $device['name'];
    //$type = $device['type'];
    $ip = $device['ip'];
    $gwid = $device['gwid'];
    $LoRAMacaddress = $device['LoRAMacaddress'];

    $db->query("UPDATE RFIDDevice SET name='$name', ip='$ip', macaddress='$macaddress', LoRAMacaddress='$LoRAMacaddress', gwid='$gwid', alive=$alive WHERE deviceid=$deviceid");
    //$db->query("UPDATE RFIDDevice SET name='$name', type='$type', ip='$ip', macaddress='$macaddress', LoRAMacaddress='$LoRAMacaddress', gwid='$gwid', alive=$alive WHERE deviceid=$deviceid");
}

function loaddevice($project_id, $queryoffset, $querycount)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from RFIDDevice ORDER BY datetime ASC ");
    else
        $db->query("SELECT * from RFIDDevice ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $device = [];
        $device['alive'] = $result['alive'];
        $device['deviceid'] = $result['deviceid'];
        $device['macaddress'] = $result['macaddress'];
        $device['name'] = $result['name'];
        $device['type'] = $result['type'];
        $device['ip'] = $result['ip'];
        $device['gwid'] = $result['gwid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['datetime'] = $result['datetime'];
        $devices[$count] = $device;
        $count = $count + 1;
    }

    return $devices;
}

function devicecount($project_id)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT count(*) as count FROM RFIDDevice ");

    $count = 0;
    if ($result = $db->fetch_array())
    {
        $count = (int)$result['count'];
    }

    return $count;
}

function delete_rfiddevice($project_id, $deviceid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM RFIDDevice WHERE deviceid=$deviceid ");
}

function save_new_rfidcard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO RFIDCard(cardid) VALUES ('$cardid')");
}

function update_rfidcard($project_id, $cardid, $cardname)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE RFIDCard SET name='$cardname' WHERE cardid='$cardid' ");
}

function delete_rfidcard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM RFIDCard WHERE cardid='$cardid' ");
}

function register_rfidcard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE RFIDCard SET unknown=0 WHERE cardid='$cardid' ");
}

function queryRFIDcards($project_id, $querycards)
{
    $cards = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM RFIDCard WHERE cardid in($querycards) ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $card = [];
        $card['cardid'] = $result['cardid'];
        $card['name'] = $result['name'];
        $card['userid'] = $result['userid'];
        $cards[$count] = $card;
        $count = $count + 1;
    }

    return $cards;
}

function loadrfidcardscan($project_id, $queryoffset, $querycount, $matchname)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    $matchquery = '';
    if (strlen($matchname) > 0 )
    {
        $matchquery = " AND cardid LIKE '%{$matchname}%' ";
    }

    if ( $querycount == 0 )
        $db->query("SELECT * from CARDScanRecord WHERE cardtype=1 $matchquery ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from CARDScanRecord WHERE cardtype=1 $matchquery ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $device = [];
        $device['id'] = $result['id'];
        $device['cardid'] = $result['cardid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['datetime'] = $result['datetime'];
        $devices[$count] = $device;
        $count = $count + 1;
    }

    return $devices;
}

function delete_all_unknowncard($project_id)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM RFIDCard WHERE unknown=1 ");
}

function loadunknowncard($project_id, $showonline, $showoffline, $queryoffset, $querycount)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    $show_cmd = '';
    if ($showonline == 1 && $showoffline == 1)
    {
        $show_cmd = '';
    }
    else
    if ($showonline == 1)
    {
        $show_cmd = 'AND alive > 0';
    }
    else
    if ($showoffline == 1)
    {
        $show_cmd = 'AND alive == 0';
    }
    else
    {
        $show_cmd = 'AND alive == -1';
    }

    if ( $querycount == 0 )
        $db->query("SELECT * from RFIDCard WHERE unknown=1 $show_cmd ORDER BY datetime ASC ");
    else
        $db->query("SELECT * from RFIDCard WHERE unknown=1 $show_cmd ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $device = [];
        $device['alive'] = $result['alive'];
        if ($device['alive'] == null)
            $device['alive'] = '0';

        $device['cardid'] = $result['cardid'];
        $device['name'] = $result['name'];
        $device['type'] = $result['type'];
        $device['maplayer'] = $result['maplayer'];
        $device['userid'] = $result['userid'];
        $device['deviceid'] = $result['deviceid'];
        $device['datetime'] = $result['datetime'];
        $device['updatetime'] = $result['updatetime'];
        $devices[$count] = $device;
        $count = $count + 1;
    }

    return $devices;
}

function cardcount($project_id)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT count(*) as count FROM RFIDCard ");

    $count = 0;
    if ($result = $db->fetch_array())
    {
        $count = (int)$result['count'];
    }

    return $count;
}

?>
