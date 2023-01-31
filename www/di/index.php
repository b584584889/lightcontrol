<?php
include ('checksession.php');

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

// require_once("common.php");

if(empty($elements[0]))
{                       // No path elements means home
    //ShowHomepage();
    echo 'normal';
}
else
if(array_shift($elements))// di
{
    if(array_shift($elements))// index.php
    {
        if ( isset($elements[0]) )
            $target = $elements[0];
        else
            $target = $elements;

        $project_id = '1';

        if (empty($elements[1]))
        {
            if (is_numeric($target))
            {
                $project_id = $target;
            }
        }
        else
        {
            $project_id = explode('?',$elements[1])[0];// 1?RangeStartDate=2017:09:11:01:33:22&RangeEndDate=2017:09:13:01:33:22
        }

        switch($target)             // Pop off first item and switch
        {
        case 'login':
            include ('login.php');
            break;
        case 'di':
            include ('di.php');
            break;
        default:
            include ('di.php');
        }
    }
}

?>
