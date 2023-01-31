<?php
    // session_start();
    //
    // require_once("common.php");
    // require_once("User.php");
    require_once("configvils.php");

    $clientIP = $_SERVER['REMOTE_ADDR'];

    $register_email = '';
    $activate_failed = '';
    $activate_success = '';

    if ( !empty( $_GET ) )
    {
        if ( isset($_GET['a']) && isset($_GET['k']) )
        {
            $register_email = $_GET['a'];
            $register_token = $_GET['k'];

            list($existed_email, $registerinfo) = check_account_email_existed(1, $register_email);

            if ( $existed_email )
            {
                // 檢查
                if ( strcmp($register_token, $registerinfo['token']) == 0 )
                {
                    if ( $registerinfo['valid'] == 0 )
                    {
                        $project_id = 1;
                        //
                        // 確認啟動
                        //
                        $registerinfo['valid'] = 1;
                        update_register($project_id, $registerinfo);

                        //
                        // create new account to
                        //
                        list($userid, $datagroups) = create_new_account($project_id, $registerinfo);

                        create_new_UserSubgroupUIBlock($project_id, $datagroups);

                        $record = [];
                        $record['project_id'] = $project_id;
                        $record['userid'] = strval($userid);
                        $record['account'] = $register_email;

                        $json_string = json_encode($record);
                        $send_string = 'update_user='. $json_string;
                        sendMessageToServer($send_string);

                        $activate_success = '帳號啟動成功!'.$userid;
                    }
                    else
                    {
                        $activate_success = '帳號已啟動成功.';
                    }
                }
                else
                {
                    $activate_failed = '帳號啟動失敗!';
                }
            }
            else
            {
                $activate_failed = '帳號啟動失敗!';
            }
        }
    }

?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
        include ('../vils_html_head.php');
    ?>

<body class="hold-transition register-page">
    <div class="register-box">
      <div class="register-logo">
        <b><h2><?php echo $login_tilte;?></h2></b></a>
        <b><h2><?php echo $activate_account;?></h2></b></a>
      </div>

      <div class="register-box-body">

          <?php
              if (strlen($activate_failed) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-danger">'; echo $activate_failed; echo'</span></h4></div>';
              }
              if (strlen($activate_success) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-success">'; echo $activate_success; echo'</span></h4></div>';
              }
          ?>

        <form action="/login.php?account=<?php echo $register_email; ?>" method="post">

          <div class="row">
            <div class="col-xs-2">
                <span class='logo-mini'>
                    <div class='image'>
                      <img src='/images/VILS_logo_s.jpg' class='img-circle' alt='VILS Logo' id='logo'>
                    </div>
                </span>
            </div>
            <!-- /.col -->
            <div class="col-xs-10">
                <?php
                if (strlen($activate_failed) > 0)
                {
                    echo '<a href="/register.php" class="btn btn-danger btn-block btn-flat pull-right">'. $activate_reregister.'</a>';
                }
                ?>
            </div>

            <!-- /.col -->
            <div class="col-xs-10">
                <?php
                if (strlen($activate_success) > 0)
                {
                    echo '<button type="submit" class="btn btn-primary btn-block btn-flat pull-right">'. $activate_login.'</button>';
                }
                ?>
            </div>
            <!-- /.col -->
          </div>
        </form>

      </div>
      <!-- /.login-box-body -->
    </div>
    <!-- /.login-box -->
    <div align="center"><h4><span class="label">Your IP: </span><span class="label label-success"><?php echo $clientIP;?></span></h4></div>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <!-- <script src="/js/vils.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</body>

</html>
