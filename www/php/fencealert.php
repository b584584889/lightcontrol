<?php

require_once("common.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        FENCEALERT_do_GET();
    break;
    case 'PUT':
        FENCEALERT_do_PUT();
    break;
    case 'POST':
        FENCEALERT_do_POST();
    break;
    case 'DELETE':
        FENCEALERT_do_DELETE();
    break;
}

function FENCEALERT_do_GET()
{
    $project_id = "1";
    if (isset($_GET['project_id']))
        $project_id = $_GET['project_id'];

    if (isset($_GET['loadfencealert']))
    {
        $return_str = loadfencealert($project_id);
        echo json_encode($return_str);
    }
}

function FENCEALERT_do_PUT()
{
}

function FENCEALERT_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['delete_fencealertgroup']))
    {
        $group_id = (int)$_POST['group_id'];

        $json_array = [];
        $json_array['GroupID'] = $group_id;
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);

        $send_string = 'deletefencealert=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (!is_array($obj))
    {
        $project_id = $obj->project_id;

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['nodeid'] = $obj->anchor;
        $json_array['distance'] = (int)$obj->distance;

        $json_string = json_encode($json_array);

        $send_string = 'createfencealert=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($obj);
    }
    else
    {
        $issues = [];
        echo json_encode($issues);
    }
}

function FENCEALERT_do_DELETE()
{
    $event = [];
    if (isset($_GET['group_id']))
    {
        $group_id = (int)$_GET['group_id'];

        $json_array = [];
        $json_array['GroupID'] = $group_id;
        $json_array['project_id'] = $_GET['project_id'];
        $json_string = json_encode($json_array);

        $send_string = 'deletefencealert=' . $json_string;
        sendMessageToServer($send_string);
    }

    echo json_encode($event);
}

function loadfencealert($project_id)
{
    $fencealert = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM FenceAlert ");
    while($result = $db->fetch_array())
    {
        $alert = [];
        $alert['GroupID'] = $result['GroupID'];
        $alert['nodeid'] = $result['nodeid'];
        $alert['distance'] = $result['distance'];

        $fencealert[$result['GroupID']] = $alert;
    }

    return $fencealert;
}

?>
