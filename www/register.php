<?php
    session_start();

    require_once("config.php");
    require_once("./php/common.php");
    require_once("./php/User.php");

    if ($GL_CONFIG_ENABLE_EMAIL_ACCOUNT == 0)
    {
        $redir = '/index.php';

        // 重導到相關頁面
        header("Location: $redir");
        exit;
    }

    $clientIP = $_SERVER['REMOTE_ADDR'];

    $register_error = '';
    $register_send = '';
    $register_email = '';
    $register_groupname= '';
    $register_password = '';
    $existed_email = false;

    if ( !empty( $_POST ) )
    {
        if ( isset( $_POST['email'] ) && isset( $_POST['password'] ) && isset( $_POST['passwordretype'] ) )
        {
            $post_email = $_POST['email'];
            $post_password = $_POST['password'];
            $post_passwordretype = $_POST['passwordretype'];

            $post_email = str_replace(' ', '', $post_email);
            $post_password = str_replace(' ', '', $post_password);
            $post_passwordretype = str_replace(' ', '', $post_passwordretype);

            $json_array = [];
            $json_array['email'] = $post_email;
            $json_array['groupname'] = $_POST['groupname'];
            $json_array['password'] = $post_password;
            $json_array['passwordretype'] = $post_passwordretype;

            $register_email = $json_array['email'];
            $register_groupname = $json_array['groupname'];
            $register_password = $json_array['password'];

            if ( strlen($json_array['email']) == 0)
            {
                $register_error = '請輸入email帳號';
            }
            else
            if ( strcmp($json_array['password'], $json_array['passwordretype']) != 0)
            {
                $register_error = '密碼不符,請重新輸入';
            }
            else
            {
                // ikki@smims.com
                list($existed_email, $registerinfo) = check_account_email_existed(1, $register_email);

                if($existed_email)
                {
                    $register_error = '此email已申請過, 請確認email是否正確!';
                }
                else
                {
                    $register_send = '此email尚未申請過.';

                    //
                    // create key
                    //
                    $ERP_MD5_KEY = "5kNAKSDPTmkKCueWFQynFtmuycb5r2Y84bVkBzxbVtXkWKrFgTKyaaCYBEMHrDX2";
                    $password_with_salt = $register_password . $ERP_MD5_KEY;
                    $pwd = strtoupper(MD5($password_with_salt));

                    $token = bin2hex(openssl_random_pseudo_bytes(64));

                    //
                    // save record
                    //
                    $newid = create_regitser(1, $register_email, $pwd, $token, $register_groupname);

                    $actual_link = "https://norgay.center/php/activate.php?a=" . $register_email . "&k=". $token;
        			$subject = "norgay.center 使用者註冊啟動帳號";
        			//$content = "點擊啟動帳號. <a href='" . $actual_link . "'>啟動帳號</a>";

                    // echo $content;

                    $from = 'register@norgay.center';
                    $to = $register_email;

                    // To send HTML mail, the Content-type header must be set
                    $headers  = 'MIME-Version: 1.0' . "\r\n";
                    $headers .= 'Content-type: text/html; charset=UTF-8' . "\r\n";
                    // Create email headers
                    $headers .= 'From: '.$from."\r\n".
                                'Reply-To: '.$from."\r\n" .
                                'X-Mailer: PHP/' . phpversion();

                    // Compose a simple HTML email message
                    $message = '<html><body>';
                    $message .= '<h1 style="color:#80c9fe;">諾格免費版[NFC for Everyone]帳號申請通知</h1>';
                    $message .= '<h1 style="color:#f40;">點擊啟動帳號</h1>';
                    $message .= '<a href="' . $actual_link . '">啟動帳號</a>';
                    $message .= '</body></html>';

                    //$headers = "From: register@norgay.center";

                    if(mail($to, $subject, $message, $headers))
                    {
                        $register_send = '申請已送出, 請接收email, 以完成申請帳號流程.';
                        $register_email = '';
                        $register_password = '';
                    }
                    else
                    {
                        $register_error = '無法寄送此email, 請確認email是否正確!';
                    }
                }
            }
        }
    }
    else
    if ( !empty( $_GET ) )
    {
        if ( isset( $_GET['resend'] ) )
        {
            //echo 'resernd:' . $_GET['resend'];
            $register_email = $_GET['resend'];
            list($existed_email, $registerinfo) = check_account_email_existed(1, $register_email);

            if ($existed_email)
            {
                //
                // 重新寄送
                //
                $actual_link = "https://norgay.center/php/activate.php?a=" . $register_email . "&k=". $registerinfo['token'];
                $subject = "norgay.center 使用者註冊啟動帳號";
                $content = "點擊啟動帳號-> <a href='" . $actual_link . "'>啟動帳號</a>";

                $from = 'register@norgay.center';
                $to = $register_email;

                // To send HTML mail, the Content-type header must be set
                $headers  = 'MIME-Version: 1.0' . "\r\n";
                $headers .= 'Content-type: text/html; charset=UTF-8' . "\r\n";
                // Create email headers
                $headers .= 'From: '.$from."\r\n".
                            'Reply-To: '.$from."\r\n" .
                            'X-Mailer: PHP/' . phpversion();

                // Compose a simple HTML email message
                $message = '<html><body>';
                $message .= '<h1 style="color:#80c9fe;">諾格免費版[NFC for Everyone]帳號申請通知</h1>';
                $message .= '<h1 style="color:#f40;">點擊啟動帳號</h1>';
                $message .= '<a href="' . $actual_link . '">啟動帳號</a>';
                $message .= '</body></html>';

                if(mail($to, $subject, $message, $headers))
                {
                    $register_send = '申請已送出, 請接收email, 以完成申請帳號流程.';
                    $register_email = '';
                    $register_password = '';

                    //
                    // 更新傳送次數
                    //
                    $registerinfo['send'] = $registerinfo['send'] + 1;
                    update_register(1, $registerinfo);
                }
                else
                {
                    $register_error = '無法寄送此email, 請確認email是否正確!';
                }
            }
            else
            {
                $register_error = '此email尚未申請過.';
            }
        }
    }

?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
        include ('vils_html_head.php');
    ?>

<body class="hold-transition register-page">
    <div class="register-box">
      <div class="register-logo">
        <b><h2><?php echo $login_tilte;?></h2></b></a>
        <b><h2><?php echo $register_new_account;?></h2></b></a>
      </div>

      <div class="register-box-body">

          <?php
              if (strlen($register_error) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-danger">'; echo $register_error; echo'</span></h4></div>';
              }
              if (strlen($register_send) > 0)
              {
                  echo '<div align="center"><h4><span class="label label-success">'; echo $register_send; echo'</span></h4></div>';
              }
          ?>

        <form action="/register.php" method="post">
            <div class="form-group has-feedback">
                <input type="email" name="email" class="form-control" placeholder=<?php echo $register_new_email;?> value="<?php echo $register_email;?>" required>
                <span class="glyphicon glyphicon-envelope form-control-feedback"></span>
            </div>
            <div class="form-group has-feedback">
                <input type="text" name="groupname" class="form-control" placeholder=<?php echo $register_new_groupname;?> value="<?php echo $register_groupname;?>" required>
                <span class="glyphicon glyphicon-flag form-control-feedback"></span>
                <?php echo $register_groupname_explain;?>
            </div>

          <div class="form-group has-feedback">
            <input type="password" name="password" class="form-control" placeholder=<?php echo $register_new_password;?>  value="<?php echo $register_password;?>" required>
            <span class="glyphicon glyphicon-lock form-control-feedback"></span>
          </div>

          <div class="form-group has-feedback">
            <input type="password" name="passwordretype" class="form-control" placeholder=<?php echo $register_retype_password;?> required>
            <span class="glyphicon glyphicon-log-in form-control-feedback"></span>
          </div>

          <div class="row">
            <div class="col-xs-2">
                <span class='logo-mini'>
                    <div class='image'>
                      <img src='/images/VILS_logo_s.jpg' class='img-circle' alt='VILS Logo' id='logo'>
                    </div>
                </span>
            </div>
            <!-- /.col -->
            <div class="col-xs-3">
              <a href="login.php">已有帳號</a>
            </div>
            <div class="col-xs-4">
                <?php
                    if ( $existed_email && strlen($register_email) > 0)
                    {
                        echo "<a href='register.php?resend=".$register_email."'>重寄認證信</a>";
                    }
                ?>
            </div>
            <!-- /.col -->
            <div class="col-xs-3">
              <button type="submit" class="btn btn-primary btn-block btn-flat pull-right"><?php echo $register_apply;?></button>
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

</body>

</html>
