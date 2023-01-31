
<?php

require_once("../config.php");
require_once("../php/common.php");

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

//echo 'map.php $elements[0]:' . $elements[0] .'<br>';

if(empty($elements[0]) || empty($elements[1]))
{                       // No path elements means home
    echo 'Not Valid map api';
}
else
if(array_shift($elements))// api
{
    if($elements[0] != 'map')
    {
        echo 'Not Valid map api';
        return;
    }
    if(empty($elements[1]))
    {
        echo 'Not Valid map api';
        return;
    }

    $superapi = $elements[0];
    $target = $elements[1];

    //echo 'map.php ' .$mapapi. '/' . $action .'<br>';

    switch($target)             // Pop off first item and switch
    {
    case 'uploadImg':
        //echo 'map.php uploadImg';
        if ($_POST)
        {
            if (isset($_FILES['document_file']))
            {
                global $GL_CONFIG;
                $target_Path = $GL_CONFIG['IMAGE_UPLOAD_PATH'];
                $upload_img_name = $_FILES['document_file']['name'];
                $target_Path = $target_Path.basename( $upload_img_name );
                move_uploaded_file( $_FILES['document_file']['tmp_name'], $target_Path );
                if(strlen($upload_img_name) > 0)
                {
                    $url_img_file = '/upload/' . $upload_img_name;
                    print_success_uploadImg_msg($_FILES['document_file']['name'], $url_img_file);
                }
            }
            else
            {
                print_fail_msg('document_file not found!');
            }
        }
        else
        {
            print_fail_msg('Not support this method!');
        }
        break;
    case 'deleteImg':
        echo 'map.php deleteImg';
        break;
    default:
        echo 'map.php map api default';
    }

}

?>
