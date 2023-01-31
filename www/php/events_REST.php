<?php
require_once("common.php");

function REST_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadevents']))
    {
        $return_str = loadevents($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadactions']))
    {
        $actions = [];
        $actions[0]['actionid'] = 'Enter';
        $actions[0]['actionname'] = 'Enter';
        $actions[1]['actionid'] = 'Leave';
        $actions[1]['actionname'] = 'Leave';
        $actions[2]['actionid'] = 'Stay';
        $actions[2]['actionname'] = 'Stay';
        // $actions[3]['actionid'] = 'Alert';
        // $actions[3]['actionname'] = 'Alert';
        // $actions[4]['actionid'] = 'Cancel';
        // $actions[4]['actionname'] = 'Cancel';
        echo json_encode($actions);
    }
    else
    if (isset($_GET['loadissuetimes']))
    {
        $issues = [];
        $issues[0]['issueid'] = '0';
        $issues[0]['issuename'] = 'Immediately';
        $issues[1]['issueid'] = '1';
        $issues[1]['issuename'] = '1 minute';
        $issues[2]['issueid'] = '5';
        $issues[2]['issuename'] = '5 minutes';
        $issues[3]['issueid'] = '10';
        $issues[3]['issuename'] = '10 minutes';

        echo json_encode($issues);
    }
}

function REST_do_PUT()
{
}

function REST_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['delete_event']))
    {
        $eventid = (int)$_POST['eventid'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['eventid'] = $eventid;
        $json_string = json_encode($json_array);

        $send_string = 'deleteevent=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (!is_array($obj))
    {
        $json_array = [];
        $json_array['project_id'] = (int)$obj->project_id;
        $json_array['nodeid'] = $obj->nodeid;
        $json_array['action'] = $obj->action;
        $json_array['areaid'] = $obj->areaid;
        $json_array['issuetime'] = $obj->issuetime;

        $json_string = json_encode($json_array);

        $send_string = 'createevent=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    {
        $issues = [];
        echo json_encode($issues);
    }
}

function REST_do_DELETE()
{
    $event = [];
    if (isset($_GET['eventid']))
    {
        $eventid = (int)$_GET['eventid'];
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['eventid'] = $eventid;
        $json_string = json_encode($json_array);

        $send_string = 'deleteevent=' . $json_string;
        sendMessageToServer($send_string);
    }
    echo json_encode($event);
}

function loadevents($project_id)
{
    $events = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM events WHERE PROJECT_ID=".$project_id." ORDER BY datetime DESC");

    $index = 0;
    $events = [];

    while($result = $db->fetch_array())
    {
        $event = [];
        $event['eventid'] = $result['id'];
        $event['PROJECT_ID'] = $result['PROJECT_ID'];
        $event['nodeid'] = $result['nodeid'];
        $event['areaid'] = $result['areaid'];
        $event['action'] = $result['action'];
        $event['issuetime'] = $result['issuetime'];
        $event['nodename'] = get_node_name($project_id, $result['nodeid']);
        $event['areaname'] = get_area_name($project_id, $result['areaid']);
        $event['issuename'] = get_issuetime_name($result['issuetime']);

        $events[$result['id']] = $event;
        $index++;
    }
    //$db->close();

    return $events;
}

?>
