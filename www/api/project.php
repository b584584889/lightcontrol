<?php

require_once("../php/common.php");

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

//echo 'project.php $elements[0]:' . $elements[0] .'<br>';

if(empty($elements[0]) || empty($elements[1]))
{                       // No path elements means home
    print_fail_msg('Not Valid project api');
}
else
if(array_shift($elements))// api
{
    if($elements[0] != 'project')
    {
        print_fail_msg('Not Valid project api');
        return;
    }
    if(empty($elements[1]))
    {
        print_fail_msg('Not Valid project api');
        return;
    }

    $projectapi = $elements[0];
    $target = $elements[1];
    //$action = $elements[2];
    $action = explode('?',$elements[2])[0];

    //echo 'project.php $target:' . $target .'<br>';

    switch($target)             // Pop off first item and switch
    {
    case 'tag':

        //$action = $elements[2];
        //echo 'project.php ' .$projectapi. '/' . $target . '/'.$action.'<br>';

        do_node("TAG", $target, $action);

    break;
    case 'anchor':
        do_node("ANCHOR", $target, $action);
    break;
    case 'coord':
        do_node("COORD", $target, $action);
    break;
    case 'locator':
        do_node("LOCATOR", $target, $action);
    break;
    default:
        print_fail_msg('Not Valid project api');
    break;
    }
}

function do_node($PREFIX, $target, $action)
{
    switch($action)             // Pop off first item and switch
    {
    // case 'list':
    //     // http://localhost/api/project/tag/list?sort=id&order=desc&offset=11&limit=10
    //     $sort = $_GET['sort'];
    //     $order = $_GET['order'];
    //     $offset = $_GET['offset'];
    //     $limit = $_GET['limit'];
    //
    //     //echo 'sort:'.$sort.' order:'.$order.' offset:'.$offset.' limit:'.$limit.'<br>';
    //
    //     do_node_list($PREFIX, $target, $sort, $order, $offset, $limit);
    //
    //
    // break;
    case 'get':
        //  http://localhost/api/project/tag/get?macaddress=0421000000000302
        $macaddress = $_GET['macaddress'];
        do_node_get($PREFIX, $target, $macaddress);
    break;
    default:
        print_fail_msg('Not Valid project api');
    break;
    }
}

function do_node_get($PREFIX, $target, $macaddress)
{
    $status_str = '';

    $status_str = getServerInfo('single_'.$target.'_info='.$macaddress);
    //$status = packageJSON($status_str);
    echo $status_str;

    //$node_index = 'LOC_'.$PREFIX.'_INDEX_0';
    //$node_info = $status[$node_index];
    //$auotag = packageAUONode($PREFIX, $node_info);
    //echo json_encode($auotag);
}

function do_node_list($PREFIX, $target, $sort, $order, $offset, $limit)
{
    $status_str = '';
    if (strlen($offset) == 0 || strlen($limit) == 0)
    {
        $offset = 0;
        $limit = 0;
        $status_str = getServerInfo($target.'status');
    }
    else
    {
        $status_str = getServerInfo($target.'status='. $offset . ',' . $limit);
    }
    $status = packageJSON($status_str);

    $auonodelist = packageAUONodeList($PREFIX, $status, (int)$offset, (int)$limit);

    echo json_encode($auonodelist);
}

function packageAUONodeList($PREFIX, $status, $offset, $limit)
{
    //echo 'LOC_TAG_NUM:'.$status['LOC_TAG_NUM'] . '<br>';

    $json_array = array();
    $node_count = (int)$status['LOC_'.$PREFIX.'_NUM'];
    for ( $i=0; $i<$node_count ; $i++ )
    {
        $node_index = 'LOC_'.$PREFIX.'_INDEX_' . ($i+$offset);
        //echo 'tag_index:'.$tag_index.'<br>';

        $node_info = $status[$node_index];
        //echo 'tag_index:'.$tag_index.' tag_info:'.$tag_info.'<br>';

        $node = packageAUONode($node_info);
        array_push($json_array, $node);
    }

    return $json_array;
}

function packageAUONode($node_info)
{
    $node_info_list = explode(',', $node_info);

    //echo 'tag_info[3]:'.$tag_info[3].'<br>';

    $node = [];

    $node['id']=$node_info_list[0];
    $node['sourceId']=$node_info_list[3];
    $node['code']="";
    $node['mac']=$node_info_list[6];
    $node['projectId']=1;
    $node['status']="online";
    $node['pulseFrequency']=0;
    $node['rxDelay']=0;
    $node['txDelay']=0;
    $node['wakeFreq']=0;
    $node['refreshFreq']=0;
    $node['maxPower']=0;
    $node['manu']=null;
    $node['swVersion']=$node_info_list[12];
    $node['hwVersion']=null;
    $node['channel']=0;
    $node['frameType']=null;
    $node['crc']=0;
    $node['dataRate']=0;
    $node['pacSize']=0;
    $node['preambleLength']=0;
    $node['preambleCode']=0;
    $node['smartPower']=0;
    $node['frameCheck']=0;
    $node['upTime']=null;
    $node['addTime']=null;
    $node['addUser']=0;
    $node['alias']=$node_info_list[5];
    $node['catId']=0;
    $node['typeId']=0;
    $node['others']=null;
    $node['sort']=0;
    $node['cat']=null;
    $node['type']=null;
    $node['power']=0;
    $node['lastPosTime']=0;
    $node['x']=$node_info_list[7];
    $node['y']=$node_info_list[8];
    $node['z']=$node_info_list[9];
    $node['mapId']=0;
    $node['mapName']=null;

    return $node;
}
?>
