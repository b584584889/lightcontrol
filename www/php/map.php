<?php

session_start();

require_once("common.php");

header('Content-Type: application/json');

if (isset($_GET['loadmap']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $session_maingroups = $_SESSION["maingroups"];
    $session_groupid = $_SESSION["groupid"];

    $maps = getMaps($project_id, $session_groupid, $session_maingroups);
    echo json_encode($maps);
}
else
if (isset($_GET['findlocation']))
{
    $address = '';
    if (isset($_GET['address']))
        $address = $_GET['address'];
    $status = getServerInfo('findlocation='.$address);
    echo $status;
}
else
if (isset($_POST["createmaplayer"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $layer_name = 'New Layer';
    if (isset($_POST['layer_name']))
        $layer_name = $_POST['layer_name'];

    $maingroupid = 1;
    $maingroups = explode(",", $_SESSION['maingroups']);
    if (count($maingroups) > 0)
        $maingroupid = (int)$maingroups[0];

    // write to mysql
    create_maplayer($project_id, 0, $maingroupid, $layer_name);

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'updatearea=' . $json_string;

    # send notification to server
    sendMessageToServer($send_string);

    echo json_encode($_POST);
}
else
if (isset($_POST["deletemaplayer"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    // write to mysql
    delete_maplayer($project_id, $maplayerid);

    delete_maplayerarea_all($project_id, $maplayerid);

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'updatearea=' . $json_string;

    # send notification to server
    sendMessageToServer($send_string);

    echo json_encode($_POST);
}
else
//if (isset($_POST["NEW_AREA"]))
if (isset($_POST["createmaplayerarea"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    $type = 0;
    if (isset($_POST['type']))
        $type = (int)$_POST['type'];

    // write to mysql
    //update_to_MySQL($_POST['areaid'], $_POST['areaname'], $project_id, $_POST['geojson'], $type);
    update_maplayerarea($project_id, $maplayerid, $_POST['areaid'], $_POST['areaname'], $_POST['geojson'], $type);

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'updatearea=' . $json_string;

    # send notification to server
    sendMessageToServer($send_string);

    echo json_encode($_POST);
}
else
//if (isset($_POST["EDIT_AREA"]))
if (isset($_POST["updatemaplayerarea"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    $type = -1;
    if (isset($_POST['type']))
        $type = (int)$_POST['type'];
    // write to mysql
    if ($type == -1)
        update_maplayerarea_shape($project_id, $maplayerid, $_POST['areaid'], $_POST['areaname'], $_POST['geojson']);
    else
        update_maplayerarea($project_id, $maplayerid, $_POST['areaid'], $_POST['areaname'], $_POST['geojson'], $type);


    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'updatearea=' . $json_string;
    # send notification to server
    sendMessageToServer($send_string);

    echo json_encode($_POST);
}
else
//if (isset($_POST["DELETE_AREA"]))
if (isset($_POST["deletemaplayerarea"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    // delete from mysql
    //delete_from_MySQL($_POST['areaid'], $project_id);
    delete_maplayerarea($project_id, $maplayerid, $_POST['areaid']);

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'updatearea=' . $json_string;
    # send notification to server
    sendMessageToServer($send_string);

    echo json_encode($_POST);
}
else
if (isset($_POST["updateorig"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    $maplayer = getMapLayer($project_id, $maplayerid);

    if ($maplayer['layerid'] != 0)
    {
        $maplayer['POSX'] = $_POST['POSX'];
        $maplayer['POSY'] = $_POST['POSY'];
        updateMapLayer($maplayer);

        echo json_encode($maplayer);
    }
    else
    {
        echo json_encode($_POST);
    }

}
else
if (isset($_POST["updatemap"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    $maplayer = getMapLayer($project_id, $maplayerid);

    $maplayer['SCALE'] = $_POST['SCALE'];
    $maplayer['COEFF'] = $_POST['COEFF'];
    updateMapLayer($maplayer);

    echo json_encode($maplayer);
}
else
if (isset($_POST["updatemaplayerinfo"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maplayerid = 1;
    if (isset($_POST['maplayerid']))
        $maplayerid = (int)$_POST['maplayerid'];

    $maplayer = getMapLayer($project_id, $maplayerid);

    $data_key = $_POST['data_key'];
    $data_value = $_POST['data_value'];

    $maplayer[$data_key] = $data_value;
    updateMapLayer($maplayer);

    echo json_encode($maplayer);
}
else
if (isset($_POST["updatemapfile"]))
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $maingroupid = 1;
    $maingroups = explode(",", $_SESSION['maingroups']);
    if (count($maingroups) > 0)
        $maingroupid = (int)$maingroups[0];

    $session_groupid = $_SESSION["groupid"];

    $maps = getMaps($project_id, $session_groupid, $maingroupid);
    $map = $maps[0];

    $map['FILE'] = $_POST['FILE'];
    $map['MAPWIDTH'] = $_POST['MAPWIDTH'];
    $map['MAPHEIGHT'] = $_POST['MAPHEIGHT'];
    updateMap($map['layerid'], $map['maingroupid'], $project_id, $map['layer_name'], $map['POSX'], $map['POSY'], $map['SCALE'], $map['COEFF'], $map['FILE'], $map['MAPWIDTH'], $map['MAPHEIGHT']);

    echo json_encode($map);
}
else
if (isset($_GET["loadLocationArea"]))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $session_maingroups = $_SESSION["maingroups"];
    $session_groupid = $_SESSION["groupid"];

    $type = 0;
    if (isset($_GET['type']))
        $type = (int)$_GET['type'];

    $areaconfig = loadmaplocationareas($project_id, $type, $session_groupid, $session_maingroups);

    echo json_encode($areaconfig);
}
else
if (isset($_GET['loadmaplayerarea']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $maplayerid = 1;
    if (isset($_GET['maplayerid']))
        $maplayerid = (int)$_GET['maplayerid'];

    $maplayerareas = loadmaplayerarea($project_id, $maplayerid);
    echo json_encode($maplayerareas);
}
else
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $session_maingroups = $_SESSION["maingroups"];
    $session_groupid = $_SESSION["groupid"];

    $type = 0;
    $areaconfig = loadmaplocationareas($project_id, $type, $session_groupid, $session_maingroups);
    echo json_encode($areaconfig);
}
// else
// {
//     $project_id = 1;
//     if (isset($_GET['project_id']))
//         $project_id = (int)$_GET['project_id'];
//
//     $areaconfig = loadmapareas($project_id);
//     echo json_encode($areaconfig);
// }

function update_maplayerarea_shape($project_id, $maplayerid, $areaid, $areaname, $geojson)
{
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['layerid'] = $maplayerid;
    $json_array['areaid'] = $areaid;
    $json_array['areaname'] = $areaname;
    $json_array['geojson'] = $geojson;
    $json_string = json_encode($json_array);

    //$send_string = 'updatemapareashape=' . $json_string;
    $send_string = 'updatemaplayerareashape=' . $json_string;
    sendMessageToServer($send_string);
}

function update_maplayerarea($project_id, $maplayerid, $areaid, $areaname, $geojson, $type)
{
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['layerid'] = $maplayerid;
    $json_array['areaid'] = $areaid;
    $json_array['areaname'] = $areaname;
    $json_array['geojson'] = $geojson;
    $json_array['type'] = $type;
    $json_string = json_encode($json_array);

    //$send_string = 'updatemaparea=' . $json_string;
    $send_string = 'updatemaplayerarea=' . $json_string;
    sendMessageToServer($send_string);
}

function delete_maplayerarea($project_id, $maplayerid, $areaid)
{
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['layerid'] = $maplayerid;
    $json_array['areaid'] = $areaid;
    $json_string = json_encode($json_array);

    //$send_string = 'deletemaparea=' . $json_string;
    $send_string = 'deletemaplayerarea=' . $json_string;
    sendMessageToServer($send_string);
}

function loadmaplocationareas($project_id, $type, $session_groupid, $session_maingroups)
{
    $areaList = [];

    $db = new DB($project_id, 'base');

    $QueryString = '';
    if ($session_groupid == '5' || $session_groupid == '6')
    {
        // maingroup user
        $layeridarray = getMapLayerIds($project_id, $session_groupid, $session_maingroups);
        $layerids = implode(",", $layeridarray);
        $QueryString = "AND layerid in($layerids)";
    }

    if ($type != 0)
    {
        $db->query("SELECT * FROM maplayerarea WHERE type=$type $QueryString");
    }
    else
    {
        $db->query("SELECT * FROM maplayerarea WHERE type != 999 $QueryString");
    }

    while($result = $db->fetch_array())
    {
        $areaList[$result['areaid']] = $result['geojson'];
    }
    ////$db->close();

    return $areaList;
}

// function loadmapareas($project_id)
// {
//     $areaList = [];
//
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT * FROM MapArea WHERE PROJECT_ID=".$project_id." ");
//     while($result = $db->fetch_array())
//     {
//         $areaList[$result['areaid']] = $result['geojson'];
//     }
//     //$db->close();
//
//     return $areaList;
// }

function loadmaplayerarea($projectid, $maplayerid)
{
    $maplayerareas = [];

    $db = new DB($projectid, 'base');
    $db->query("SELECT * FROM maplayerarea WHERE layerid=".$maplayerid." ");

    while($result = $db->fetch_array())
    {
        $maplayerareas[$result['areaid']] = $result['geojson'];
    }

    return $maplayerareas;
}


?>
