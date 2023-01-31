<?php

session_start();

require_once("../config.php");
require_once("../php/common.php");

// header('Content-Type: application/json');

// $headers = apache_request_headers();
//
// foreach ($headers as $header => $value)
// {
//     echo "$header: $value <br />\n";
// }

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
       SENSOR_do_GET();
    break;
    case 'PUT':
        SENSOR_do_PUT();
    break;
    case 'POST':
        SENSOR_do_POST();
    break;
    case 'DELETE':
        SENSOR_do_DELETE();
    break;
}

function SENSOR_do_GET()
{
    header('Content-Type: application/json');

    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    //
    // https://khh.smims.com/api/sensorinfo.php?loadevent=1&sensorid=LiDAR0f0032
    // https://khh.smims.com/api/sensorinfo.php?loadevent=1&sensorid=LiDAR0f0032&type=Arrival
    // https://khh.smims.com/api/sensorinfo.php?loadevent=1&sensorid=LiDAR0f0032&type=Departure
    // https://khh.smims.com/api/sensorinfo.php?loadevent=1&sensorid=LiDAR0f0032&offset=0&count=10
    //
    if (isset($_GET['loadevent']))
    {
        $validkey = FALSE;
        if (isset($_GET['key']))
        {
            $key = $_GET['key'];
            $validkey = check_valid_apikey($key);
        }

        if ( $validkey == TRUE)
        {
            $sensorid = "";
            $type = "";
            $count = 0;
            $offset = 0;

            if (isset($_GET['sensorid']))
                $sensorid = $_GET['sensorid'];
            if (isset($_GET['count']))
                $count = (int)$_GET['count'];
            if (isset($_GET['offset']))
                $offset = (int)$_GET['offset'];
            if (isset($_GET['type']))
                $type = $_GET['type'];
            $eventfired = getSensorFiredEvents($project_id, $sensorid, $type, $count, $offset);
            echo json_encode($eventfired);
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":""}';
        }
    }
    else
    if (isset($_GET['loadflyinfo']))
    {
        // https://khh.smims.com/api/sensorinfo.php?loadflyinfo=1&key=sotA6cY54U
        // https://khh.smims.com/api/sensorinfo.php?loadflyinfo=1&key=sotA6cY54U&day=2021-01-18
        // https://khh.smims.com/api/sensorinfo.php?loadflyinfo=1&key=sotA6cY54U&day=2021-01-18&type=Arrival
        // https://khh.smims.com/api/sensorinfo.php?loadflyinfo=1&key=sotA6cY54U&day=2021-01-18&type=Departure

        $validkey = FALSE;
        if (isset($_GET['key']))
        {
            $key = $_GET['key'];
            $validkey = check_valid_apikey($key);
        }

        if ( $validkey == TRUE)
        {
            $day = "";
            $type = "";
            $airLineCode = "";
            $airLineNum = "";
            $count = 0;
            $offset = 0;

            if (isset($_GET['count']))
                $count = (int)$_GET['count'];
            if (isset($_GET['offset']))
                $offset = (int)$_GET['offset'];
            if (isset($_GET['day']))
                $day = $_GET['day'];
            if (isset($_GET['type']))
                $type = $_GET['type'];
            if (isset($_GET['airLineCode']))
                $airLineCode = $_GET['airLineCode'];
            if (isset($_GET['airLineNum']))
                $airLineNum = $_GET['airLineNum'];

            $FlyInfos = getFlyInfo($project_id, $day, $type, $airLineCode, $airLineNum, $count, $offset);
            echo json_encode($FlyInfos);
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":""}';
        }
    }
}

function SENSOR_do_PUT()
{
}


// Sample command for Data Update
// $ curl 'https://xxx.xxx.xxx.xxx/' \
// -H 'User-Agent: Customer-Name' \
// -H "Authorization: Token XXXXX" \
// -F did="DEVICE ID"\
// -F lat=24.774457 \
// -F lng=121.0445673\
// -F HR=99\
// -F SPO2=99\
// -F Battery=90

// Sample command for Emergency Call
// $ curl 'https://xxx.xxx.xxx.xxx/' \
// -H 'User-Agent: Customer-Name' \
// -H "Authorization: Token XXXXX" \
// -F content="emergency report" \
// -F did="DEVICE ID"\
// -F late6=24.774457 \
// -F lnge6=121.0445673 \
// -F data_source="ProMOS" \
// -F category="category"
//
// Response:
// {"code":"200","msg":"ok","detail":""}

// curl http://192.168.99.101/api/sensorinfo.php -F did=m1234 -F lat=24.774457 -F lng=121.0445673 -F HR=99 -F SPO2=99 -F Battery=90
// curl http://192.168.99.101/api/sensorinfo.php -F did=m1234 -F late6=24.774457 -F lnge6=121.0445673 -F content="emergency report" -F data_source="ProMOS" -F category="category"

function SENSOR_do_POST()
{
    $project_id = 1;

    if (isset($_POST['content']))
    {
        $content = $_POST['content'];
        $did = $_POST['did'];
        $late6 = '';
        $lnge6 = '';
        $data_source = '';
        $category = '';

        if (isset($_POST['late6']))
            $late6 = $_POST['late6'];
        if (isset($_POST['lnge6']))
            $lnge6 = $_POST['lnge6'];
        if (isset($_POST['data_source']))
            $data_source = $_POST['data_source'];
        if (isset($_POST['category']))
            $category = $_POST['category'];

        $jsondata = [];
        $jsondata['category'] = $category;
        $jsondata['data_source'] = $data_source;
        $jsondata['content'] = $content;
        $json_string = json_encode($jsondata);

        update_sensor_data($project_id, $did, 'ProMOS Watch', 100, $late6, $lnge6, $json_string);

        $jsondata['did'] = $did;
        $jsondata['lat'] = $late6;
        $jsondata['lng'] = $lnge6;
        $json_string = json_encode($jsondata);

        $send_string = 'sensor_info='. $json_string;
        sendMessageToServer($send_string);

        echo '{"code":"200","msg":"ok","detail":""}';
    }
    else
    if (isset($_POST['did']))
    {
        $did = $_POST['did'];
        $lat = '';
        $lng = '';
        $HR = '';
        $SPO2 = '';
        $Battery = '';

        if (isset($_POST['lat']))
            $lat = $_POST['lat'];
        if (isset($_POST['lng']))
            $lng = $_POST['lng'];
        if (isset($_POST['HR']))
            $HR = $_POST['HR'];
        if (isset($_POST['SPO2']))
            $SPO2 = $_POST['SPO2'];
        if (isset($_POST['Battery']))
            $Battery = $_POST['Battery'];

        $jsondata = [];
        $jsondata['HR'] = $HR;
        $jsondata['SPO2'] = $SPO2;
        $jsondata['Battery'] = $Battery;
        $json_string = json_encode($jsondata);

        update_sensor_data($project_id, $did, 'ProMOS Watch', $Battery, $lat, $lng, $json_string);

        $jsondata['did'] = $did;
        $jsondata['lat'] = $lat;
        $jsondata['lng'] = $lng;
        $json_string = json_encode($jsondata);

        $send_string = 'sensor_info='. $json_string;
        sendMessageToServer($send_string);

        echo '{"code":"200","msg":"ok","detail":""}';
    }
    else
    if (isset($_POST['updateFlyInfo']))
    {
        $FDATE = '';
        $airLineNum = '';
        $airBoardingGate = '';
        $triggerTime = '';

        if (isset($_POST['FDATE']))
            $FDATE = $_POST['FDATE'];
        if (isset($_POST['airLineNum']))
            $airLineNum = $_POST['airLineNum'];
        if (isset($_POST['airBoardingGate']))
            $airBoardingGate = $_POST['airBoardingGate'];
        if (isset($_POST['triggerTime']))
            $triggerTime = $_POST['triggerTime'];

        updateFlyInfo($FDATE, $airLineNum, $airBoardingGate, $triggerTime);

    }
    else
    if (isset($_POST['updateFlyEvent']))
    {
        $eventid = '';
        $sensorid = '';
        $datetime = '';
        $triggerTime = '';

        if (isset($_POST['eventid']))
            $eventid = $_POST['eventid'];
        if (isset($_POST['sensorid']))
            $sensorid = $_POST['sensorid'];
        if (isset($_POST['datetime']))
            $datetime = $_POST['datetime'];
        if (isset($_POST['triggerTime']))
            $triggerTime = $_POST['triggerTime'];

        updateFlyEvent($eventid, $sensorid, $datetime, $triggerTime);

    }
    else
    {
        echo '{"code":"200","msg":"failed","detail":""}';
    }
}

function update_sensor_data($project_id, $did, $type, $value, $lat, $lng, $jsonvalue)
{
    $db = new DB($project_id, 'base');

    $day = date('Y-m-d');

    $db->query("INSERT INTO sensorinfo(name, day, value, jsonvalue, Longitude, Latitude) VALUES ('$did', '$day', '$value', '$jsonvalue', '$lng', '$lat') ");

    $newid = $db->get_insert_id();

    return $newid;
}

function getSensorFiredEvents($project_id, $sensorid, $type, $count, $offset)
{
    $db = new DB($project_id, 'base');

    $FiredEvents = [];

    if ($count == 0)
        $count = 10;
    if ($count > 100)
        $count = 100;

    $queryString = "type='Arrival'";

    if (strlen($type) > 0)
        $queryString = "type='$type'";

    if (strlen($sensorid) > 0)
        $queryString = $queryString . " AND sensorid='$sensorid'";

    $db->query("SELECT * FROM sensorevent WHERE $queryString ORDER BY datetime DESC LIMIT $offset,$count");

    $index = 0;
    while($result = $db->fetch_array())
    {
        $FiredEvents[$index] = json_decode($result['jsonvalue']);
        $FiredEvents[$index]->{'eventid'} = $result['id'];
        $FiredEvents[$index]->{'sensorid'} = $result['sensorid'];
        $FiredEvents[$index]->{'datetime'} = $result['datetime'];
        $index++;
    }
    return $FiredEvents;
}

function getFlyInfo($project_id, $day, $type, $airLineCode, $airLineNum, $count, $offset)
{
    $db = new DB($project_id, 'base');

    if ($count == 0)
        $count = 50;
    if ($count > 100)
        $count = 100;

    $FlyInfos = [];

    $FDATE = $day;
    if (strlen($day) == 0)
    {
        $FDATE = date('Y-m-d');
    }

    $queryString = "FDATE='$FDATE'";

    if (strlen($type) > 0)
        $queryString = $queryString . " AND type='$type'";
    if (strlen($airLineCode) > 0)
        $queryString = $queryString . " AND airLineCode='$airLineCode'";
    if (strlen($airLineNum) > 0)
        $queryString = $queryString . " AND airLineNum='$airLineNum'";

    $db->query("SELECT * FROM FlyInfo WHERE $queryString ORDER BY expectTime ASC LIMIT $offset,$count");

    $index = 0;
    while($result = $db->fetch_array())
    {
        $FlyInfo = json_decode($result['jsonvalue']);
        $FlyInfo->{'airBoardingGate'} = $result['airBoardingGate'];
        if ($result['triggerTime'] == null)
            $FlyInfo->{'triggerTime'} = '';
        else
            $FlyInfo->{'triggerTime'} = $result['triggerTime'];

        $FlyInfos[$index] = $FlyInfo;
        $index++;
    }

    return $FlyInfos;
}

function updateFlyInfo($FDATE, $airLineNum, $airBoardingGate, $triggerTime)
{
    $db = new DB(1, 'base');
    $db->query("UPDATE FlyInfo SET airBoardingGate='$airBoardingGate', triggerTime='$triggerTime'  WHERE FDATE='$FDATE' AND airLineNum='$airLineNum' ");
}

function updateFlyEvent($eventid, $sensorid, $datetime, $triggerTime)
{
    $db = new DB(1, 'base');

    $db->query("SELECT * FROM sensorevent WHERE id=$eventid ");
    if($result = $db->fetch_array())
    {

        $FlyInfo = json_decode($result['jsonvalue']);
        $FlyInfo->{'triggerTime'} = $triggerTime;

        $json_string = json_encode($FlyInfo);

        $db->query("UPDATE sensorevent SET sensorid='$sensorid', datetime='$datetime', jsonvalue='$json_string'  WHERE id=$eventid ");
    }
}
?>
