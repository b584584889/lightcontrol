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

        if (isset($_GET['loadsensorid']))
        {
            $objResult = loadsensorid($project_id);
            echo json_encode($objResult);
        }

        if (isset($_GET['loadpmrecord']))
        {
            $dteStart = $_GET['sdate'];
            $dteEnd = $_GET['edate'];
            $strId = $_GET['id'];
            $objResult = loadpmevent($project_id, $strId, $dteStart, $dteEnd);
            echo json_encode($objResult);
        }
    }

    function REST_do_PUT()
    {

    }

    function REST_do_POST()
    {

    }

    function loadsensorid($project_id)
    {
        $sensors = [];
        $count = 0;
        $db = new DB($project_id, 'base');
        $t = "SELECT DISTINCT name FROM sensorinfo WHERE name LIKE 'wentaitek_pm_47C1%' ORDER BY name ASC ";
        $db->query($t);
        // writeLogFile([$db->get_num_rows()], 'logfiletest.txt');
        while ($result = $db->fetch_array())
        {
            // writeLogFile([explode('_', $result['name'])[2]], 'logfiletest.txt');
            $sensor['id'] = explode('_', $result['name'])[2];
            $sensors[$count] = $sensor;
            $count++;
        }
        return $sensors;
    }

    function loadpmevent($project_id, $strId, $dteStart, $dteEnd)
    {
        $records = [];
        $count = 0;
        $db = new DB($project_id, 'base');
        $q = "SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_pm_$strId%' AND datetime BETWEEN '$dteStart' AND '$dteEnd' ORDER BY datetime DESC LIMIT 50 ";
        // $q = "SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_pm_$strId%' AND datetime BETWEEN '2022/07/11 15:36:25' AND '2022/07/12 15:36:25' ORDER BY datetime DESC LIMIT 50 ";
        // $q = "SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_pm_8291%' AND datetime BETWEEN '$dteStart' AND '$dteEnd' ORDER BY datetime DESC LIMIT 50 ";
        // writeLogFile([$q], 'logfiletest.txt');
        $db->query($q);
        while ($result = $db->fetch_array())
        {
            $record = json_decode($result['jsonvalue'], true);
            $record['time'] = $result['datetime'];
            $records[$count] = $record;
            $count++;
        }
        return $records;
    }
?>
