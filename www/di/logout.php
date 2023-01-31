<?php
    session_start();

    unset($_SESSION["authenticated"]);

    $redir = '/index.php';

    // 重導到相關頁面
    header("Location: $redir");
?>
