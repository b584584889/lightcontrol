
<?php

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

#echo '$elements[0]:' . $elements[0] .'<br>';

if(empty($elements[0]))
{                       // No path elements means home
    echo 'apis';
}
else
if(array_shift($elements))// api
{
    $target = $elements[0];

    #echo '$target:' . $target .'<br>';

    switch($target)             // Pop off first item and switch
    {
    // case 'super':
    //     include ('super.php');
    //     break;
    // case 'map':
    //     include ('map.php');
    //     break;
    case 'project':
        include ('project.php');
        break;
    case 'sensor':
        include ('sensorinfo.php');
        break;
    case 'pushnotify':
        include ('pushnotify.php');
        break;
    default:
        echo 'apis default';
    }

}

?>
