<?php

    session_start();

    require_once("common.php");

    $method = $_SERVER['REQUEST_METHOD'];

    switch ($method)
    {
        case 'GET':
            VILSNODES_do_GET();
        break;
        case 'PUT':
            VILSNODES_do_PUT();
        break;
        case 'POST':
            VILSNODES_do_POST();
        break;
        case 'DELETE':
            VILSNODES_do_DELETE();
        break;
    }

    function VILSNODES_do_GET()
    {
        if (isset($_GET['fetchsystemtime']))
        {

        }
        else
        if (isset($_GET['loadfirefightingcases']))
        {
            $project_id = 1;
            if (isset($_GET['project_id']))
                $project_id = (int)$_GET['project_id'];

            $closed = -1;
            if (isset($_GET['closed']))
                $closed = (int)$_GET['closed'];

            $session_maingroups = $_SESSION["maingroups"];

            $cases = getFireFightingCases($project_id, $session_maingroups, $closed);
            echo json_encode($cases);
        }
        else
        if (isset($_GET['loadfirefightingusers']))
        {
            $project_id = 1;
            if (isset($_GET['project_id']))
                $project_id = (int)$_GET['project_id'];

            $casenumber = '';
            if (isset($_GET['casenumber']))
                $casenumber = $_GET['casenumber'];

            $areaType = '';
            if (isset($_GET['areaType']))
                $areaType = $_GET['areaType'];

            // $casenumbers = explode(",", $casenumber);

            $session_maingroups = $_SESSION["maingroups"];

            $cases = getFireFightingUsers($project_id, $session_maingroups, $casenumber, $areaType);
            echo json_encode($cases);
        }
        else
        if (isset($_GET['loaduserairusing']))
        {
            $project_id = 1;
            if (isset($_GET['project_id']))
                $project_id = (int)$_GET['project_id'];

            $userid = $_GET['userid'];
            $UserAirUsing = getUserAirUsing($project_id, $userid);
            echo json_encode($UserAirUsing);
        }
        else
        if (isset($_GET['loadfirefightingevent']))
        {
            $project_id = 1;
            if (isset($_GET['project_id']))
                $project_id = (int)$_GET['project_id'];

            $casenumber = '';
            if (isset($_GET['casenumber']))
                $casenumber = $_GET['casenumber'];

            $eventtype = '';
            if (isset($_GET['eventtype']))
                $eventtype = $_GET['eventtype'];

            $cases = getFireFightingEvents($project_id, $casenumber, $eventtype);
            echo json_encode($cases);
        }
        else
        {

        }
    }

    function VILSNODES_do_POST()
    {
        if (isset($_POST['updatedeviceinfo']))
        {
            $json_array = [];
            $json_array['project_id'] = $_POST['project_id'];
            $json_array['casenumber'] = $_POST['casenumber'];
            $json_array['event'] = 'updatedeviceinfo';
            $json_array['NFCDeviceIds'] = $_POST['NFCDeviceIds'];
            $json_array['StandbyDeviceIds'] = $_POST['StandbyDeviceIds'];
            // $json_array['AoADeviceIds'] = $_POST['AoADeviceIds'];

            $json_string = json_encode($json_array);

            $send_string = 'updatefirefightingdeviceinfo=' . $json_string;
            sendMessageToServer($send_string);
        }
        else
        if (isset($_POST['updateuserairremain']))
        {
            $json_array = [];
            $json_array['project_id'] = $_POST['project_id'];
            $json_array['casenumber'] = $_POST['casenumber'];
            $json_array['event'] = 'updateuserairremain';
            $json_array['userid'] = $_POST['userid'];
            $json_array['airremain'] = $_POST['airremain'];

            $json_string = json_encode($json_array);

            $send_string = 'updatefirefightinguserairremain=' . $json_string;
            sendMessageToServer($send_string);
        }
        else
        if (isset($_POST['updateuserarea']))
        {
            $json_array = [];
            $json_array['project_id'] = $_POST['project_id'];
            $json_array['casenumber'] = $_POST['casenumber'];
            $json_array['event'] = 'updateuserarea';
            $json_array['userid'] = $_POST['userid'];
            $json_array['areaType'] = $_POST['areaType'];

            $json_string = json_encode($json_array);

            $send_string = 'updatefirefightinguserarea=' . $json_string;
            sendMessageToServer($send_string);
        }
        else
        if (isset($_POST['createFireFighting']))
        {
            $project_id = 1;
            if (isset($_POST['project_id']))
                $project_id = (int)$_POST['project_id'];

            $address = '';
            $lat = '';
            $address = '';
            if (isset($_POST['address']))
                $address = $_POST['address'];
            if (isset($_POST['lat']))
                $lat = $_POST['lat'];
            if (isset($_POST['lng']))
                $lng = $_POST['lng'];

            $maingroupid = 1;
            $maingroups = explode(",", $_SESSION['maingroups']);
            if (count($maingroups) > 0)
                $maingroupid = (int)$maingroups[0];

            $json_array = [];
            $json_array['project_id'] = $project_id;
            $json_array['maingroupid'] = $maingroupid;
            $json_array['address'] = $address;
            $json_array['lat'] = $lat;
            $json_array['lng'] = $lng;
            $json_string = json_encode($json_array);

            $status = getServerInfo('createFireFighting='.$json_string);
            echo $status;
        }
        else
        if (isset($_POST['finishcase']))
        {
            $json_array = [];
            $json_array['project_id'] = $_POST['project_id'];
            $json_array['casenumber'] = $_POST['casenumber'];
            $json_array['event'] = 'finishcase';
            $json_array['calcairusing'] = $_POST['calcairusing'];

            $json_string = json_encode($json_array);

            $send_string = 'finishFireFighting=' . $json_string;
            sendMessageToServer($send_string);
        }
        else
        {

        }
    }

    function VILSNODES_do_PUT()
    {
    }

    function VILSNODES_do_DELETE()
    {
    }


    function getFireFightingCases($projectid, $maingroups, $closed)
    {
        $cases = [];

        $db = new DB($projectid, 'base');

        if ($closed == -1)
            $check_res = $db->query("SELECT * FROM FireFighting where maingroupid in($maingroups) ");
        else
            $check_res = $db->query("SELECT * FROM FireFighting where maingroupid in($maingroups) AND caseclosed=$closed ");

        if ( $check_res )
        {
            $index = 0;
            while($result = $db->fetch_array())
            {
                $case = [];
                $case['id'] = $result['id'];
                $case['maingroupid'] = $result['maingroupid'];
                $case['caseclosed'] = $result['caseclosed'];
                $case['casenumber'] = $result['casenumber'];
                $case['address'] = $result['address'];
                $case['lat'] = $result['lat'];
                $case['lng'] = $result['lng'];
                $case['starttime'] = $result['starttime'];
                $case['closetime'] = $result['closetime'];
                $case['NFCDeviceIds'] = $result['NFCDeviceIds'];
                $case['StandbyDeviceIds'] = $result['StandbyDeviceIds'];
                // $case['AoADeviceIds'] = $result['AoADeviceIds'];
                $case['datetime'] = $result['datetime'];

                if ($case['StandbyDeviceIds'] == null)
                {
                    $case['StandbyDeviceIds'] = "";
                }

                $cases[$index] = $case;
                $index += 1;
            }
        }

        return $cases;
    }

    function getFireFightingUsers($projectid, $maingroups, $casenumbers, $areaType)
    {
        $users = [];

        $db = new DB($projectid, 'base');

        $loadAreaType = '';
        if (strlen($areaType) > 0)
        {
            $loadAreaType = " AND areaType=$areaType ";
        }

        $check_res = $db->query("SELECT * FROM FireFightingUser where maingroupid in($maingroups) AND casenumber in($casenumbers) $loadAreaType ");

        if ( $check_res )
        {
            $index = 0;
            while($result = $db->fetch_array())
            {
                $user = [];
                $user['userid'] = $result['userid'];
                $user['casenumber'] = $result['casenumber'];
                $user['maingroupid'] = $result['maingroupid'];
                $user['areaType'] = $result['areaType'];
                $user['airremain'] = $result['airremain'];
                $user['enterTime'] = $result['enterTime'];
                $user['activity'] = $result['activity'];
                $user['HR'] = $result['HR'];
                $user['SPO2'] = $result['SPO2'];
                $user['battery'] = $result['battery'];

                $users[$index] = $user;
                $index += 1;
            }
        }

        return $users;
    }

    function getUserAirUsing($projectid, $userid)
    {
        $db = new DB($projectid, 'base');

        $AirUsing = [];
        $AirUsing['userid'] = $userid;
        $AirUsing['airusing'] = 5;

        $check_res = $db->query("SELECT * FROM FireFightingAirUsing WHERE userid=$userid AND casenumber='1' ");

        if ( $check_res )
        {
            if($result = $db->fetch_array())
            {
                $AirUsing['userid'] = $result['userid'];
                $AirUsing['airusing'] = $result['airusing'];
                if ($AirUsing['airusing'] == 'nan')
                    $AirUsing['airusing'] = 5;
            }
        }

        return $AirUsing;
    }

    function getFireFightingEvents($projectid, $casenumbers, $eventtype)
    {
        $events = [];

        $db = new DB($projectid, 'base');

        $loadeventtype = '';
        if (strlen($eventtype) > 0)
        {
            $loadeventtype = " AND eventtype='$eventtype' ";
        }

        $check_res = $db->query("SELECT * FROM FireFightingEvent where casenumber in($casenumbers) $loadeventtype ");

        if ( $check_res )
        {
            $index = 0;
            while($result = $db->fetch_array())
            {
                $event = [];
                $event['id'] = $result['id'];
                $event['eventtype'] = $result['eventtype'];
                $event['json'] = json_decode($result['jsonstring']);
                $event['datetime'] = $result['datetime'];

                $events[$index] = $event;
                $index += 1;
            }
        }

        return $events;
    }
?>
