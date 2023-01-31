<?php
// Report simple running errors
error_reporting(E_ERROR | E_WARNING | E_PARSE);

if (session_status() != PHP_SESSION_ACTIVE)
{
    session_start();
}

if($_SESSION["authenticated"] == true)
{
    if( isset( $_GET['user'] ) )
    {
        if ( $_GET['user'] != $_SESSION['SO_user'] )
        {
            $_SESSION['SO_user'] = $_GET['user'];
            header('Location: /login.php');
            exit;
        }
    }
    else
    if ( isset( $_POST['user'] ) )
    {
        if ( $_POST['user'] != $_SESSION['SO_user'] )
        {
            $_SESSION['SO_user'] = $_GET['user'];
            header('Location: /login.php');
            exit;
        }
    }
}
else
{
    if ( isset( $_GET['user'] ) )
    {
        $_SESSION['SO_user'] = $_GET['user'];
    }
    else
    if ( isset( $_POST['user'] ) )
    {
        $_SESSION['SO_user'] = $_POST['user'];
    }
    else
    {
        $_SESSION['SO_user'] = '';
    }
    header('Location: /login.php');
    exit;
}
?>
