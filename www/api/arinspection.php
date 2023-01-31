<?php

session_start();

require_once("../config.php");
require_once("../php/common.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        AR_do_GET();
    break;
    case 'POST':
        AR_do_POST();
    break;
}

function AR_do_GET()
{
    header('Content-Type: application/json');

    if (isset($_GET['loadrecord']))
    {
        $validkey = FALSE;
        if (isset($_GET['key']))
        {
            $key = $_GET['key'];
            $validkey = check_valid_apikey($key);
        }

        if ( $validkey == TRUE)
        {
            $android_id = "";
            $type = "";
            $count = 0;
            $offset = 0;

            if (isset($_GET['android_id']))
                $android_id = $_GET['android_id'];
            if (isset($_GET['type']))
                $type = $_GET['type'];
            if (isset($_GET['count']))
                $count = (int)$_GET['count'];
            if (isset($_GET['offset']))
                $offset = (int)$_GET['offset'];

            $records = getInsectionRecords($android_id, $type, $count, $offset);
            echo json_encode($records);
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":""}';
        }
    }
    else
    {
        echo '{"code":"200","msg":"invalid action!","detail":""}';
    }
}

function AR_do_POST()
{
    header('Content-Type: application/json');

    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    if ($obj->uploadrecord == 1)
    {
        $validkey = FALSE;
        if ($obj->key != NULL)
        {
            $validkey = check_valid_apikey($obj->key);
        }

        if ( $validkey == TRUE)
        {
            if ($obj->encodeddata != NULL)
            {
                // create_AR_table();
                $retuMsg = create_arinspection_record($obj->recordid, $obj->android_id, $obj->filetype, $obj->encodeddata, $obj->Longitude, $obj->Latitude, $obj->created);
                echo '{"code":"200","msg":"valid key!","recordid":"'.$obj->recordid.'","created":"'.$obj->created.'","retuMsg":"'.$retuMsg.'","encodeddata_length":"'.strlen($obj->encodeddata).'"}';
            }
            else
            {
                echo '{"code":"200","msg":"valid key!","recordid":"'.$obj->recordid.'","created":"'.$obj->created.'","android_id":"'.$obj->android_id.'"}';
            }
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":"'.$obj->recordid.'"}';
        }
    }
    else
    {
        echo '{"code":"200","msg":"invalid action!","detail":"'.$obj->uploadrecord.'"}';
    }
}

function create_arinspection_record($recordid, $android_id, $filetype, $encodeddata, $Longitude, $Latitude, $created)
{
    $retuMsg = "";
    $db = new DB(1, 'arinspection');

    $db->query("SELECT * FROM inspection_record WHERE recordid=$recordid AND android_id='$android_id' AND created='$created' ");
    if($result = $db->fetch_array())
    {
        // already existed
        $db->query("UPDATE inspection_record SET filetype=$filetype, file='$encodeddata', Longitude='$Longitude', Latitude='$Latitude' WHERE recordid=$recordid AND android_id='$android_id' AND created='$created' ");
        $retuMsg = "UPDATE";
    }
    else
    {
        $db->query("INSERT INTO inspection_record(recordid, filetype, file, Longitude, Latitude, android_id, created) VALUES ($recordid, $filetype, '$encodeddata', '$Longitude', '$Latitude', '$android_id', '$created') ");
        $retuMsg = "INSERT";
    }
    return $retuMsg;
}

function getInsectionRecords($android_id, $type, $count, $offset)
{
    $db = new DB(1, 'arinspection');

    $Records = [];

    if ($count == 0)
        $count = 10;
    if ($count > 100)
        $count = 100;

    $queryString = "";

    if (strlen($type) > 0)
        $queryString = "filetype=$type";

    if (strlen($android_id) > 0)
        $queryString = $queryString . " AND android_id='$android_id'";

    if (strlen($queryString) == 0 )
        $db->query("SELECT * FROM inspection_record ORDER BY datetime DESC LIMIT $offset,$count ");
    else
        $db->query("SELECT * FROM inspection_record WHERE $queryString ORDER BY datetime DESC LIMIT $offset,$count ");

    $index = 0;
    while($result = $db->fetch_array())
    {
        $record = [] ;
        $record['id'] = $result['id'];
        $record['recordid'] = $result['recordid'];
        $record['filetype'] = $result['filetype'];
        $record['Longitude'] = $result['Longitude'];
        $record['Latitude'] = $result['Latitude'];
        $record['android_id'] = $result['android_id'];
        $record['created'] = $result['created'];
        $record['datetime'] = $result['datetime'];
        $record['file'] = $result['file'];

        $Records[$index] = $record;
        $index++;
    }
    return $Records;
}

function create_AR_table()
{
    $retuMsg = "";

    $db = new DB(1, 'arinspection');

    // Attempt create table query execution
    $sql = "CREATE TABLE inspection_record(
        id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
        recordid INT NOT NULL,
        filetype INT NOT NULL,
        file MEDIUMBLOB NOT NULL,
        Longitude VARCHAR(70),
        Latitude VARCHAR(70),
        android_id VARCHAR(70),
        created timestamp DEFAULT CURRENT_TIMESTAMP,
        datetime timestamp DEFAULT CURRENT_TIMESTAMP
    )";

    $db->query($sql);
    return $retuMsg;
}

?>
