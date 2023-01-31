<?php

require_once("common.php");

if (isset($_GET['getrangingdiagnosis']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['tagmacaddress'] = $_GET['getrangingdiagnosis'];
    $json_string = json_encode($json_array);
    $send_string = 'getrangingdiagnosis=' . $json_string;

    $status_str = getServerInfo($send_string);
    //$status_str = getServerInfo('getrangingdiagnosis=' . $_GET['getrangingdiagnosis']);
    $status = packageJSON($status_str);
    echo json_encode($status);
}
else
if (isset($_GET['getlocatorrssiinfo']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['LoRAMacaddress'] = $_GET['getlocatorrssiinfo'];
    $json_string = json_encode($json_array);
    $send_string = 'getlocatorrssiinfo=' . $json_string;

    $status_str = getServerInfo($send_string);
    //$status_str = getServerInfo('getrangingdiagnosis=' . $_GET['getrangingdiagnosis']);
    $status = packageJSON($status_str);
    echo json_encode($status);
}

?>
