
<?php

require_once("../php/common.php");

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

//echo 'super.php $elements[0]:' . $elements[0] .'<br>';

if(empty($elements[0]) || empty($elements[1]))
{                       // No path elements means home
    print_fail_msg('Not Valid super api');
}
else
if(array_shift($elements))// api
{
    if($elements[0] != 'super')
    {
        print_fail_msg('Not Valid super api');
        return;
    }
    if(empty($elements[1]))
    {
        print_fail_msg('Not Valid super api');
        return;
    }

    $superapi = $elements[0];
    $target = $elements[1];
    $action = $elements[2];

    //echo 'super.php ' .$superapi. '/' . $target .'<br>';

    switch($target)             // Pop off first item and switch
    {
    case 'map':
        //echo 'super.php ' .$superapi. '/' . $target . '/'.$action.'<br>';
        do_map($action);

        break;
    default:
        echo '[]';
    }

}

function do_map($action)
{
    switch($action)             // Pop off first item and switch
    {
    case 'list':
        $maps = getMaps(1);
        $map = $maps[0];
        //echo 'mapid:' . $map->{'mapid'};

        $auomap = packageAUOMap($map->{'mapid'}, 1, $map->{'mapname'}, $map->{'POSX'}, $map->{'POSY'}, $map->{'SCALE'}, $map->{'COEFF'}, $map->{'FILE'}, $map->{'MAPWIDTH'}, $map->{'MAPHEIGHT'});

        echo json_encode($auomap);

    break;
    case 'save':

        //echo $action . ' $method:' . $method . ' id:'.$_POST['id'];

        if ($_POST)
        {
            if (isset($_POST['id']))
            {
                // echo '/super/map/save id:' . $_POST['id'];
                if (!isset($_POST['cname']))
                {
                    print_fail_msg('cname not found!');
                }
                else
                {
                    // AUO data
                    $cname = $_POST['cname'];
                    $sort = $_POST['sort'];
                    $IsActive = $_POST['IsActive'];
                    $pixelLength = $_POST['pixelLength'];
                    $pixelWidth = $_POST['pixelWidth'];
                    $realLength = $_POST['realLength'];
                    $realWidth = $_POST['realWidth'];

                    $project_id = $_POST['id'];

                    // SMIMS VILS data
                    $json_array = [];
                    $json_array['project_id'] = $project_id;
                    $json_array['PROJECT_NAME'] = $cname;
                    $json_array['PROJECT_DESCRIPTION'] = $cname;
                    $json_array['WGS48OriginX'] = '0';
                    $json_array['WGS48OriginY'] = '0';
                    $json_array['MAP_North'] = '0';
                    $json_array['BuildingOriginX'] = '0';
                    $json_array['BuildingOriginY'] = '0';
                    $json_array['BuildingXLength'] = '0';
                    $json_array['BuildingYLength'] = '0';
                    $json_array['BuildingID'] = '1';
                    $json_array['BuildingFloor'] = '1';
                    $json_string = json_encode($json_array);
                    sendMessageToServer('updateconfig='.$json_string);

                    print_success_msg($_POST['id']);
                }
            }
            else
            {
                print_fail_msg('id not found!');
            }
        }
        else
        {
            print_fail_msg('Not support this method!');
        }
    break;
    default:
        print_fail_msg('Not Valid super api');
    break;
    }
}

function packageAUOMap($mapid, $projectid, $mapname, $POSX, $POSY, $SCALE, $COEFF, $FILE, $MAPWIDTH, $MAPHEIGHT)
{
    $maps = array();

    $json_array = [];
    $json_array['cname'] = $mapname;
    $json_array['filePath'] = $FILE;
    $json_array['id'] = (int)$mapid;
    $json_array['incline'] = 0;
    $json_array['isActive'] = 1;
    $json_array['origin_x'] = (float)$POSX;
    $json_array['origin_y'] = (float)$POSY;
    $json_array['origin_z'] = 0;
    $json_array['pixelLength'] = (int)$MAPHEIGHT;
    $json_array['pixelWidth'] = (int)$MAPWIDTH;
    $json_array['realLength'] = (int)$MAPHEIGHT;
    $json_array['realWidth'] = (int)$MAPWIDTH;
    $json_array['rotation'] = 0;
    $json_array['scale'] = (float)$SCALE;
    $json_array['sort'] = 1;
    $json_array['theme'] = null;

    array_push($maps, $json_array);

    return $maps;
}


?>
