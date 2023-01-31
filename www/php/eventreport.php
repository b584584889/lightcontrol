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

    if (isset($_GET['loadotherevent']))
    {
      $offset = $_GET['offset'];
      $count = $_GET['count'];
      $status = loadotherevent($project_id, $offset, $count);
      echo json_encode($status);
    }
    elseif (isset($_GET['loadlightevent'])) {
        $offset = $_GET['offset'];
        $count = $_GET['count'];
        $status = loadlightevent($project_id, $offset, $count);
        echo json_encode($status);
    }
    elseif (isset($_GET['loaduwevent'])) {
        $offset = $_GET['offset'];
        $count = $_GET['count'];
        $status = loaduwevent($project_id, $offset, $count);
        echo json_encode($status);
    }
    elseif (isset($_GET['loadwatchevent'])) {
        $offset = $_GET['offset'];
        $count = $_GET['count'];
        $status = loadwatchevent($project_id, $offset, $count);
        echo json_encode($status);
    }
    elseif (isset($_GET['loadpmevent'])) {
        $offset = $_GET['offset'];
        $count = $_GET['count'];
        $status = loadpmevent($project_id, $offset, $count);
        echo json_encode($status);
    }
}

function REST_do_PUT()
{

}

function REST_do_POST()
{

}

//取得其他事件資料
function loadotherevent($project_id, $offset, $count)
{
  $records = [];
  $db = new DB($project_id, 'base');
  $evReportCount = 0;
  $actContent = get_action_list();

  $db->query("SELECT * FROM FiredEvents ORDER BY datetime DESC LIMIT $offset, $count ");
  while($result = $db->fetch_array())
  {
    $record = [];
    $record['id'] = $result['id'];
    $record['PROJECT_ID'] = $result['PROJECT_ID'];
    $record['eventid'] = $result['eventid'];
    $record['nodeid'] = $result['eventjson'];
    $record['datetime'] = $result['datetime'];
    $records[$evReportCount] = $record;
    $evReportCount++;
  }

  return $records;
}
//取得燈泡事件資料
function loadlightevent($project_id, $offset, $count)
{
    $records = [];
    $db = new DB($project_id, 'base');
    $evReportCount = 0;

    $db->query("SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_light%' ORDER BY datetime DESC LIMIT $offset, $count ");
    while($result = $db->fetch_array())
    {
        //{"id":1,"name":"DT8_1","uniAddress":4,"state_onOff":0,"state_level":0,"state_cct":0}
        $record = json_decode($result['jsonvalue'], true);
        $record['time'] = $result['datetime'];
        $records[$evReportCount] = $record;
        $evReportCount++;
    }
    return $records;
}
//取得微波事件資料
function loaduwevent($project_id, $offset, $count)
{
    $records = [];
    $db = new DB($project_id, 'base');
    $evReportCount = 0;
    $db->query("SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_uw_2C48%' ORDER BY datetime DESC LIMIT $offset, $count ");
    while($result = $db->fetch_array())
    {
        $record = json_decode($result['jsonvalue'], true);
        $record['time'] = $result['datetime'];
        $records[$evReportCount] = $record;
        $evReportCount++;
    }
    return $records;
}
//取得智慧手錶事件資料
function loadwatchevent($project_id, $offset, $count)
{
    //writeLogFile(["123"], 'logfiletest.txt');

    $records = [];
    $db = new DB($project_id, 'base');
    $evReportCount = 0;
    $db->query("SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_watch_2EBD%' ORDER BY datetime ASC LIMIT $offset, $count ");
    while($result = $db->fetch_array())
    {
        $record = json_decode($result['jsonvalue'], true);
        $record['time'] = $result['datetime'];
        $records[$evReportCount] = $record;
        $evReportCount++;
    }
    return $records;
}
//取得智慧電錶事件資料
function loadpmevent($project_id, $offset, $count)
{
    $records = [];
    $db = new DB($project_id, 'base');
    $evReportCount = 0;
    $db->query("SELECT * FROM sensorinfo WHERE name LIKE 'wentaitek_pm_8291%' ORDER BY datetime DESC LIMIT $offset, $count ");
    while($result = $db->fetch_array())
    {
        $record = json_decode($result['jsonvalue'], true);
        $record['time'] = $result['datetime'];
        $records[$evReportCount] = $record;
        $evReportCount++;
    }
    return $records;
}

?>
