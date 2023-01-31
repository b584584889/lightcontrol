<?php

session_start();

require_once("../config.php");
require_once("../php/common.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
       PUSHNOTIFY_do_GET();
    break;
    case 'PUT':
        PUSHNOTIFY_do_PUT();
    break;
    case 'POST':
        PUSHNOTIFY_do_POST();
    break;
    case 'DELETE':
        PUSHNOTIFY_do_DELETE();
    break;
}

// https://norgay.center/api/pushnotify.php?key=JtrXGqk1UE&target=NORGAY_USER_6&title="Norgay 通知 1"&content="你有資料待審"

function PUSHNOTIFY_do_GET()
{
    $project_id = 1;

    if (isset($_GET['key']) && isset($_GET['target']) && isset($_GET['title']) && isset($_GET['content']))
    {
        $key = $_GET['key'];
        $target = $_GET['target'];
        $title = $_GET['title'];
        $content = $_GET['content'];

        $validkey = check_valid_apikey($key);

        if ( $validkey == TRUE)
        {
            $jsondata = [];
            $jsondata['project_id'] = $project_id;
            $jsondata['key'] = $key;
            $jsondata['target'] = $target;
            $jsondata['title'] = $title;
            $jsondata['content'] = $content;
            $json_string = json_encode($jsondata);

            $send_string = 'api_pushnotify='. $json_string;
            sendMessageToServer($send_string);

            echo '{"code":"200","msg":"ok","detail":""}';
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":""}';
        }
    }
    else
    {
        echo '{"code":"200","msg":"failed","detail":""}';
    }
}

function PUSHNOTIFY_do_PUT()
{
}

// curl http://192.168.99.101/api/pushnotify.php -F key=JtrXGqk1UE -F target=SMIMSKAO_USER_1161 -F title="測試推播 API 1234" -F content="測試推播 內文 http://192.168.99.101/api/"
// curl https://norgay.center/api/pushnotify.php -F key=JtrXGqk1UE -F target=NORGAY_USER_6 -F title="Norgay 通知" -F content="你有資料待審"

function PUSHNOTIFY_do_POST()
{
    $project_id = 1;

    if (isset($_POST['key']) && isset($_POST['target']) && isset($_POST['title']) && isset($_POST['content']))
    {
        $key = $_POST['key'];
        $target = $_POST['target'];
        $title = $_POST['title'];
        $content = $_POST['content'];

        $validkey = check_valid_apikey($key);

        if ( $validkey == TRUE)
        {
            $jsondata = [];
            $jsondata['project_id'] = $project_id;
            $jsondata['key'] = $key;
            $jsondata['target'] = $target;
            $jsondata['title'] = $title;
            $jsondata['content'] = $content;
            $json_string = json_encode($jsondata);

            $send_string = 'api_pushnotify='. $json_string;
            sendMessageToServer($send_string);

            echo '{"code":"200","msg":"ok","detail":""}';
        }
        else
        {
            echo '{"code":"200","msg":"invalid key!","detail":""}';
        }

    }
    else
    {
        echo '{"code":"200","msg":"failed","detail":""}';
    }
}

?>
