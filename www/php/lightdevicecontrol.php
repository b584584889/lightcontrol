<?php

    session_start();

    require_once("common.php");
    require_once("User.php");

    $method = $_SERVER['REQUEST_METHOD'];

    switch ($method) {
      case 'GET':
        REST_do_GET();
        break;

      case 'PUT':
        REST_do_PUT();
        break;

      case 'POST':
        REST_do_POST();
        break;
    }

    function REST_do_GET()
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['loadlightid']))
        {
            $objResult = loadlightid($project_id);
            echo json_encode($objResult);
        }
        if (isset($_GET['loadlightsettingbyid']))
        {
            $light_id = $_GET['id'];
            $objResult = loadlightsettingbyid($project_id, $light_id);
            echo json_encode($objResult);
        }
        if (isset($_GET['SendMqttCmd']))
        {
            $id = $_GET['id'];
            $gateway_mac = $_GET['gateway_mac'];
            $cmd = $_GET['cmd'];
            $uniAddress = $_GET['uniAddress'];
            $objResult = SendMqttCmd($project_id, $id, $gateway_mac, $cmd, $uniAddress);
            echo json_encode;
        }
    }

    function REST_do_PUT()
    {

    }

    function REST_do_POST()
    {

    }

    function loadlightid($project_id)
    {
        $sensors = [];
        $count = 0;
        $db = new DB($project_id, 'base');
        $t = "SELECT DISTINCT * FROM sensordevice WHERE id LIKE 'wentaitek_light_%' AND jsonvalue not like '%SW%' ORDER BY name ASC ";
        $db->query($t);
        // writeLogFile([$db->get_num_rows()], 'logfiletest.txt');
        // writeLogFile(['123'], 'logfiletest.txt');
        while ($result = $db->fetch_array())
        {
            // writeLogFile([explode('_', $result['name'])[2]], 'logfiletest.txt');
            writeLogFile([$result['name']], 'logfiletest.txt');
            $record = json_decode($result['jsonvalue'], true);

            $sensor['key'] = $result['id'];
            $sensor['name'] = $result['name'];
            $sensor['id'] = $record['id'];
            $sensor['uniAddress'] = $record['uniAddress'];
            $sensors[$count] = $sensor;
            $count++;
        }
        return $sensors;
    }

    function loadlightevent($project_id, $strId, $dteStart, $dteEnd)
    {
        $records = [];
        $count = 0;
        $db = new DB($project_id, 'base');
        $q = "SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_light_$strId%' AND datetime BETWEEN '$dteStart' AND '$dteEnd' ORDER BY datetime DESC LIMIT 50 ";
        writeLogFile([$q], 'logfiletest.txt');
        $db->query($q);
        while ($result = $db->fetch_array())
        {
            $record = json_decode($result['jsonvalue'], true);
            $record['name'] = $result['name'];
            $record['time'] = $result['datetime'];
            $record['mac'] = $reuslt[''];
            $records[$count] = $record;
            $count++;
        }
        return $records;
    }

    function loadlightsettingbyid($project_id, $light_id)
    {
        $records = [];
        $db = new DB($project_id, 'base');
        $q = "SELECT * FROM sensorinfo WHERE name LIKE '$light_id' ORDER BY datetime DESC LIMIT 1 ";
        $db->query($q);
        while ($result = $db->fetch_array()) {
            $record = json_decode($result['jsonvalue'], true);
            $records[0] = $record;
        }
        return $records;
    }

    function SendMqttCmd($project_id, $id, $gateway_mac, $cmd, $uniAddress)
    {
        // writeLogFile(["IN"], 'logfiletest1.txt');

        $act = explode('=', $cmd);
        $state = [];
        $state[$act[0]] = (int)$act[1];

        $device = [];
        $device['id'] = (int)$id;
        $device['uniAddress'] = (int)$uniAddress;
        $device['state'] = $state;

        $payload = [];
        $payload['gateway_mac'] = $gateway_mac;
        $payload['device'] = $device;

        // writeLogFile([json_encode($payload)], 'logfiletest1.txt');
        // writeLogFile([json_encode($payload)], 'logfiletest1.txt');

        $json = [];
        $json['payload'] = $payload;
        $json_string = json_encode($payload);
        writeLogFile([$json_string], 'logfiletest1.txt');

        $send_string = 'send_lightCommand=' . $json_string;


        sendMessageToServer($send_string);
        echo '{"code":"200","msg":"ok","detail":""}';

        //Reduce errors
        //error_reporting(~E_WARNING);

    }


?>
