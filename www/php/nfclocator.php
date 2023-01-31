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

function REST_do_PUT()
{
}

function REST_do_DELETE()
{
}

function REST_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadnfclocator']))
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
        $status = getServerInfo('nfcdevicetatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadnfclocatorcount']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_string = json_encode($json_array);
        $status = getServerInfo('nfcdevicecount='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadwifilocator']))
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
        $status = getServerInfo('wifidevicetatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadcelllocator']))
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
        $status = getServerInfo('celldevicetatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadcard']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $json_array['start'] = (int)$_GET['s'];
            $json_array['count'] = (int)$_GET['count'];
        }
        if (isset($_GET['showonline']) && isset($_GET['showoffline']))
        {
            $json_array['showonline'] = (int)$_GET['showonline'];
            $json_array['showoffline'] = (int)$_GET['showoffline'];
        }

        if ( $_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $status = queryMaingroupNFCCard($project_id, $json_array);
            echo json_encode($status);
        }
        else
        {
            $json_string = json_encode($json_array);
            $status = getServerInfo('nfccardtatus='.$json_string);
            echo $status;
        }
    }
    else
    if (isset($_GET['querycards']))
    {
        $cardid = $_GET['cardid'];

        $cardidlist = explode(",", $cardid);
        $cardids = '\'' . implode("','", $cardidlist).'\'';

        $status = queryNFCcards($project_id, $cardids);
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

        if ( $_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $json_array = [];
            $json_array['project_id'] = $_GET['project_id'];
            $json_array['showonline'] = 1;
            $json_array['showoffline'] = 1;
            $json_array['groupid'] = $_SESSION["groupid"];
            $json_array['maingroups'] = $_SESSION["maingroups"];
            $json_string = json_encode($json_array);
            $status = getServerInfo('nfcdevicetatus='.$json_string);

            $LoRAMacaddress_array = convertNFCDeviceToLoRAMacaddressArray(json_decode($status));

            $status = loadnfccardscan_fromdevices($project_id, $offset, $count, $matchname, $LoRAMacaddress_array);
            echo json_encode($status);
        }
        else
        {
            $status = loadnfccardscan($project_id, $offset, $count, $matchname);
            echo json_encode($status);
        }
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

        if ( $_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $status = loadunknowncard_MaingroupNFCCard($project_id, $offset, $count, $_SESSION["maingroups"]);
            echo json_encode($status);

            // $json_array = [];
            // $json_array['project_id'] = $_GET['project_id'];
            // $json_array['maingroups'] = $_SESSION["maingroups"];
            // $json_string = json_encode($json_array);
            // $status = getServerInfo('nfcdevicetatus='.$json_string);
            //
            // $LoRAMacaddress_array = convertNFCDeviceToLoRAMacaddressArray(json_decode($status));
            //
            // $status = loadunknowncard_fromdevices($project_id, $offset, $count, $LoRAMacaddress_array);
            // echo json_encode($status);
        }
        else
        {
            $status = loadunknowncard($project_id, $showonline, $showoffline, $offset, $count);
            echo json_encode($status);
        }
    }
}

function REST_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if(isset($_POST['create_new_nfclocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['macaddress'] = $_POST['macaddress'];
        $record['name'] = $_POST['name'];
        $record['type'] = $_POST['type'];
        $record['gwid'] = $_POST['gwid'];
        $record['LoRAMacaddress'] = $_POST['LoRAMacaddress'];

        save_new_nfclocator($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['update_nfclocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['alive'] = $_POST['alive'];
        $record['macaddress'] = $_POST['macaddress'];
        $record['name'] = $_POST['name'];
        //$record['type'] = $_POST['type'];
        $record['gwid'] = $_POST['gwid'];
        $record['LoRAMacaddress'] = $_POST['LoRAMacaddress'];

        update_nfclocator($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['update_wifilocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['alive'] = $_POST['alive'];
        $record['macaddress'] = $_POST['macaddress'];
        $record['name'] = $_POST['name'];
        $record['wifimac'] = $_POST['wifimac'];
        $record['ssid'] = $_POST['ssid'];

        update_wifilocator($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['update_celllocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['alive'] = $_POST['alive'];
        $record['macaddress'] = $_POST['macaddress'];
        $record['name'] = $_POST['name'];

        update_celllocator($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['delete_nfclocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['macaddress'] = $_POST['macaddress'];

        $maingroups = $_SESSION["maingroups"];
        $groupid = $_SESSION["groupid"];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            remove_nfclocator($project_id, $record);

            //
            // check user type
            // 是否擁有點名器
            check_user_type();
        }
        else
        {
            delete_nfclocator($project_id, $record);
        }

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['delete_wifilocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['macaddress'] = $_POST['macaddress'];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            remove_wifilocator($project_id, $record);

            //
            // check user type
            // 是否擁有點名器
            check_user_type();
        }
        else
        {
            delete_wifilocator($project_id, $record);
        }

        $json_string = json_encode($record);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_nfccard_name']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];
        $device['cardname'] = $_POST['cardname'];

        $cardid = $_POST['cardid'];
        $cardname = $_POST['cardname'];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $maingroups = $_SESSION["maingroups"];
            update_MaingroupNFCCard($project_id, $cardid, $cardname, $maingroups);
        }
        else
        {
            update_nfccard($project_id, $cardid, $cardname);
        }

        sendMessageToServer('update_nfccard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['create_new_nfccard']))
    {
        $device = [];
        $device['project_id'] = $project_id;
        $device['cardid'] = $_POST['cardid'];

        $cardid = $_POST['cardid'];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            save_new_nfccard($project_id, $cardid);

            $maingroups = $_SESSION["maingroups"];
            save_new_MaingroupNFCCard($project_id, $cardid, $maingroups);
        }
        else
        {
            save_new_nfccard($project_id, $cardid);
        }

        sendMessageToServer('update_nfccard');

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

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $maingroups = $_SESSION["maingroups"];
            delete_MaingroupNFCCard($project_id, $cardid, $maingroups);

            delete_nfccard_from_maingroupUser($project_id, $cardid, $maingroups);
        }
        else
        {
            delete_nfccard($project_id, $cardid);

            delete_nfccard_from_User($project_id, $cardid);
        }

        sendMessageToServer('update_nfccard');

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

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $maingroups = $_SESSION["maingroups"];
            register_MaingroupNFCCard($project_id, $cardid, $maingroups);
        }
        else
        {
            register_nfccard($project_id, $cardid);
        }

        sendMessageToServer('update_nfccard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_all_unknowncard']))
    {
        $device = [];
        $device['project_id'] = $project_id;

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            $maingroups = $_SESSION["maingroups"];
            delete_all_unknown_MaingroupNFCCard($project_id, $maingroups);
        }
        else
        {
            delete_all_unknowncard($project_id);
        }

        //sendMessageToServer('update_nfccard');

        $json_string = json_encode($device);
        echo $json_string;
    }
    else
    if (isset($_POST['scan_nfccard']))
    {
        sendMessageToServer('scanNFCCard');

        $json_string = json_encode($_POST);
        echo $json_string;
    }
    else
    if(isset($_POST['verify_new_nfclocator']))
    {
        $record = [];
        $record['project_id'] = $project_id;

        $verifykey = $_POST['verifykey'];
        $accountid = $_SESSION['accountid'];
        $userid = $_SESSION['userid'];

        $user_maingroupid = 1;
        $user_maingroups = explode(",", $_SESSION['maingroups']);
        if (count($user_maingroups) > 0)
            $user_maingroupid = $user_maingroups[0];

        list($verify_error, $verify_pass) = verifyNFCLocatorKey($verifykey, $accountid, $userid, $user_maingroupid);

        if( strlen($verify_pass) > 0 )
        {
            $_SESSION['type'] = 2;
        }

        $record['verify_error'] = $verify_error;
        $record['verify_pass'] = $verify_pass;

        $json_string = json_encode($record);
        echo $json_string;
    }

}

function update_nfclocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $alive = $record['alive'];
    $macaddress = $record['macaddress'];
    $name = $record['name'];
    //$type = $record['type'];
    $gwid = $record['gwid'];
    $LoRAMacaddress = $record['LoRAMacaddress'];

    $db->query("UPDATE NFCDevice SET name='$name', LoRAMacaddress='$LoRAMacaddress', gwid='$gwid', alive=$alive WHERE macaddress='$macaddress' ");
    //$db->query("UPDATE NFCDevice SET name='$name', type='$type', LoRAMacaddress='$LoRAMacaddress', gwid='$gwid', alive=$alive WHERE macaddress='$macaddress' ");
}

function update_nfclocator_maingroup($project_id, $macaddress, $maingroupid)
{
    $db = new DB($project_id, 'base');
    $db->query("UPDATE NFCDevice SET maingroupid=$maingroupid WHERE macaddress='$macaddress' ");

}

function update_wifilocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $alive = $record['alive'];
    $macaddress = $record['macaddress'];
    $name = $record['name'];
    $type = 'WIFI';
    $wifimac = $record['wifimac'];
    $ssid = $record['ssid'];

    $db->query("UPDATE NFCDevice SET name='$name', ssid='$ssid', wifimac='$wifimac', alive=$alive WHERE macaddress='$macaddress' AND type='$type' ");
}

function update_celllocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $alive = $record['alive'];
    $macaddress = $record['macaddress'];
    $name = $record['name'];

    $db->query("UPDATE NodeInfo SET name='$name', alive=$alive WHERE macaddress='$macaddress' ");
}

function remove_nfclocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("UPDATE NFCDevice SET maingroupid=1 WHERE macaddress='$macaddress' ");
}

function delete_nfclocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("DELETE FROM NFCDevice WHERE macaddress='$macaddress' ");
}

function remove_wifilocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("UPDATE NFCDevice SET maingroupid=1 WHERE macaddress='$macaddress' ");
}

function delete_wifilocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("DELETE FROM NFCDevice WHERE macaddress='$macaddress' ");
}

function save_new_nfclocator($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];
    $name = $record['name'];
    $type = $record['type'];
    $gwid = $record['gwid'];
    $LoRAMacaddress = $record['LoRAMacaddress'];

    $db->query("INSERT INTO NFCDevice(name, type, macaddress, LoRAMacaddress, gwid) VALUES ('$name', '$type', '$macaddress', '$LoRAMacaddress', '$gwid')");
}

function queryNFCcards($project_id, $querycards)
{
    $cards = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM NFCCard WHERE cardid in($querycards) ");

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

function save_new_nfccard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO NFCCard(cardid) VALUES ('$cardid')");
}

function update_nfccard($project_id, $cardid, $cardname)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE NFCCard SET name='$cardname' WHERE cardid='$cardid' ");
}

function delete_nfccard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM NFCCard WHERE cardid='$cardid' ");
}

function register_nfccard($project_id, $cardid)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE NFCCard SET unknown=0 WHERE cardid='$cardid' ");
}

function delete_all_unknowncard($project_id)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM NFCCard WHERE unknown=1 ");
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
        $db->query("SELECT * from NFCCard WHERE unknown=1 $show_cmd ORDER BY datetime ASC ");
    else
        $db->query("SELECT * from NFCCard WHERE unknown=1 $show_cmd ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");

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

//
// MaingroupNFCCard
//
function queryMaingroupNFCCard($project_id, $record)
{
    $cards = [];

    $db = new DB($project_id, 'base');

    $maingroups = $record['maingroups'];
    $query_start = $record['start'];
    $query_count = $record['count'];

    $db->query("SELECT * FROM MaingroupNFCCard WHERE unknown=0 AND maingroupid in($maingroups) ");

    $index = 0;
    $count = 0;
    while($result = $db->fetch_array())
    {
        $valid = false;
        if ($query_count > 0)
        {
            if ( $index >= $query_start && $count < $query_count )
            {
                $valid = true;
            }
        }
        else
        {
            $valid = true;
        }

        $index = $index + 1;

        if (!$valid)
        {
            continue;
        }

        $card = [];
        $card['alive'] = $result['alive'];
        $card['cardid'] = $result['cardid'];
        $card['name'] = $result['name'];
        $card['userid'] = $result['userid'];
        $card['maingroupid'] = $result['maingroupid'];
        $cards[$count] = $card;
        $count = $count + 1;
    }

    return $cards;
}

function save_new_MaingroupNFCCard($project_id, $cardid, $maingroups)
{
    $db = new DB($project_id, 'base');

    $maingroupList = explode(",", $maingroups);

    foreach ($maingroupList as $maingroup)
    {
        $db->query("INSERT INTO MaingroupNFCCard(cardid, maingroupid) VALUES ('$cardid', $maingroup)");
    }
}

function update_MaingroupNFCCard($project_id, $cardid, $cardname, $maingroups)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE MaingroupNFCCard SET name='$cardname' WHERE cardid='$cardid' AND maingroupid in($maingroups) ");
}

function delete_MaingroupNFCCard($project_id, $cardid, $maingroups)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM MaingroupNFCCard WHERE cardid='$cardid' AND maingroupid in($maingroups) ");
}

function register_MaingroupNFCCard($project_id, $cardid, $maingroups)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE MaingroupNFCCard SET unknown=0 WHERE cardid='$cardid' AND maingroupid in($maingroups) ");
}

function delete_all_unknown_MaingroupNFCCard($project_id, $maingroups)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM MaingroupNFCCard WHERE unknown=1 AND maingroupid in($maingroups) ");
}

function loadunknowncard_MaingroupNFCCard($project_id, $queryoffset, $querycount, $maingroups)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from MaingroupNFCCard WHERE unknown=1 AND maingroupid in($maingroups) ORDER BY datetime ASC ");
    else
        $db->query("SELECT * from MaingroupNFCCard WHERE unknown=1 AND maingroupid in($maingroups) ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");

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

function loadnfccardscan($project_id, $queryoffset, $querycount, $matchname)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    $matchquery = '';
    if (strlen($matchname) > 0 )
    {
        $matchquery = " AND cardid LIKE '%{$matchname}%' ";
    }

    if ( $querycount == 0 )
        $db->query("SELECT * from CARDScanRecord WHERE cardtype=0 $matchquery ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from CARDScanRecord WHERE cardtype=0 $matchquery ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $device = [];
        $device['id'] = $result['id'];
        $device['cardid'] = $result['cardid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['datetime'] = $result['datetime'];
        $device['ssid'] = $result['ssid'];
        $device['wifimac'] = $result['wifimac'];

        if ($device['ssid'] == null)
            $device['ssid'] = '';

        if ($device['wifimac'] == null)
            $device['wifimac'] = '';

        $devices[$count] = $device;
        $count = $count + 1;
    }

    return $devices;
}

function loadnfccardscan_fromdevices($project_id, $queryoffset, $querycount, $matchname, $LoRAMacaddress_array)
{
    $devices = [];

    $db = new DB($project_id, 'base');

    $matchquery = '';
    if (strlen($matchname) > 0 )
    {
        $matchquery = " AND cardid LIKE '%{$matchname}%' ";
    }

    $LoRAMacaddressArray = '\'' . implode("','", $LoRAMacaddress_array).'\'';

    $qrery_cmd = '';
    if ( $querycount == 0 )
        $qrery_cmd = "SELECT * from CARDScanRecord WHERE cardtype=0 $matchquery AND LoRAMacaddress in($LoRAMacaddressArray) ORDER BY datetime DESC ";
    else
        $qrery_cmd = "SELECT * from CARDScanRecord WHERE cardtype=0 $matchquery AND LoRAMacaddress in($LoRAMacaddressArray) ORDER BY datetime DESC LIMIT $queryoffset, $querycount ";

    $db->query($qrery_cmd);

    $count = 0;
    while($result = $db->fetch_array())
    {
        $device = [];
        $device['id'] = $result['id'];
        $device['cardid'] = $result['cardid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['datetime'] = $result['datetime'];
        $device['ssid'] = $result['ssid'];
        $device['wifimac'] = $result['wifimac'];

        if ($device['ssid'] == null)
            $device['ssid'] = '';

        if ($device['wifimac'] == null)
            $device['wifimac'] = '';

        $devices[$count] = $device;
        $count = $count + 1;
    }

    return $devices;
}

function check_nfcdevice_key($verifykey)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from NFCDevice WHERE verifykey='$verifykey' ");

    $device = [];

    $count = 0;
    if($result = $db->fetch_array())
    {
        $device['alive'] = $result['alive'];
        $device['macaddress'] = $result['macaddress'];
        $device['name'] = $result['name'];
        $device['gwid'] = $result['gwid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['maingroupid'] = $result['maingroupid'];

        $count++;
    }

    return array($count, $device);
}

function load_nfcdevice_maingroup($maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from NFCDevice WHERE maingroupid=$maingroupid ");

    $device = [];

    $count = 0;
    if($result = $db->fetch_array())
    {
        $device['alive'] = $result['alive'];
        $device['macaddress'] = $result['macaddress'];
        $device['name'] = $result['name'];
        $device['gwid'] = $result['gwid'];
        $device['LoRAMacaddress'] = $result['LoRAMacaddress'];
        $device['maingroupid'] = $result['maingroupid'];

        $count++;
    }

    return array($count, $device);
}

function convertNFCDeviceToLoRAMacaddressArray($devices)
{
    $LoRAMacaddress_array = [];

    $count = 0;
    foreach($devices as $device)
    {
        $LoRAMacaddress_array[$count] = $device->{'LoRAMacaddress'};
        $count = $count + 1;
    }

    return $LoRAMacaddress_array;
}

function verifyNFCLocatorKey($verifykey, $accountid, $userid, $user_maingroupid)
{
    $verify_error = '';
    $verify_pass = '';

    if ( strlen($verifykey) != 8)
    {
        $verify_error = '請輸入正確機器碼!';
    }
    else
    {
        list($count, $device) = check_nfcdevice_key($verifykey);
        if ( $count > 0 )
        {
            $device_maingroupid = $device['maingroupid'];
            if ( $device_maingroupid != 1 )
            {
                $verify_error = '您輸入的機器碼已被關連，請先確認已解除關連或輸入正確!';
            }
            else
            {
                if ( $user_maingroupid != 1 )
                {
                    //
                    // register this device to this maingroup
                    //
                    update_nfclocator_maingroup(1, $device['macaddress'], $user_maingroupid);

                    $json_string = json_encode($device);
                    $send_string = 'update_nfclocator='. $json_string;
                    sendMessageToServer($send_string);

                    //
                    // set User type = 2; // 0: vilscenter user 1:已註冊, 2:已購買點名裝置
                    //
                    update_user_type(1, $accountid, $userid, 2);

                    $verify_pass = '此裝置已成功註冊!';
                }
                else
                {
                    $verify_error = '目前使用者無法註冊此裝置!';
                }
            }
        }
        else
        {
            $verify_error = '您輸入的機器碼並非有效機器碼';
        }
    }

    return array($verify_error, $verify_pass);
}

function check_user_type()
{
    $accountid = $_SESSION['accountid'];
    $userid = $_SESSION['userid'];
    $user_maingroupid = 1;
    $user_maingroups = explode(",", $_SESSION['maingroups']);
    if (count($user_maingroups) > 0)
        $user_maingroupid = $user_maingroups[0];

    list($count, $device) = load_nfcdevice_maingroup($user_maingroupid);
    if ( $count == 0 )
    {
        //
        // set User type = 2; // 0: vilscenter user 1:已註冊, 2:已購買點名裝置
        //
        update_user_type(1, $accountid, $userid, 1);

        $_SESSION['type'] = 1;

        unset($_SESSION["authenticated"]);
    }
}


// function loadcard($project_id, $queryoffset, $querycount)
// {
//     $devices = [];
//
//     $db = new DB($project_id, 'base');
//
//     if ( $querycount == 0 )
//         $db->query("SELECT * from NFCCard ORDER BY datetime ASC ");
//     else
//         $db->query("SELECT * from NFCCard ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");
//
//     $count = 0;
//     while($result = $db->fetch_array())
//     {
//         $device = [];
//         $device['cardid'] = $result['cardid'];
//         $device['name'] = $result['name'];
//         $device['type'] = $result['type'];
//         $device['maplayer'] = $result['maplayer'];
//         $device['userid'] = $result['userid'];
//         $device['datetime'] = $result['datetime'];
//         $device['updatetime'] = $result['updatetime'];
//         $devices[$count] = $device;
//         $count = $count + 1;
//     }
//
//     return $devices;
// }

?>
